$RESET="`e[0m"
$BOLD="`e[1m"
$BLACK="`e[30m"
$RED="`e[31m"
$GREEN="`e[32m"
$ORANGE="`e[33m"

$GPP="g++"
$BIN_SIZE_LIMIT=150000
$COMMON_BUILD_ARGS="-std=c++26 -Wall -flto=4 -fno-exceptions -fno-rtti -fno-unwind-tables -fno-asynchronous-unwind-tables -fgcse-las -fno-plt"

$DO_TESTS=0
$DEBUG=0
$RUNTIME_DEBUG=1
$INCLUDE_SHELL=1
$DO_CAPTURE=0
$OPTIM="balanced"

$OPTIM_balanced="-O2 -finline-limit=4"
$OPTIM_speed="-Ofast"
$OPTIM_size="-Os -finline-limit=0"

$LINKS=".Ity_tmp_generated.cpp -o ity.o Main.cpp -o ity.bin"


# Parse command-line arguments.
foreach ($argument in $args) {
    switch -Regex ($argument) {
        "^-t$|^--test$" {
            $DO_TESTS = $true
        }
        "^-d$|^--debug$" {
            $DEBUG = $true
        }
        "^-s$|^--static$" {
            $COMMON_BUILD_ARGS="-static"+$COMMON_BUILD_ARGS
        }
        "^-o=(.+)$|^--optimize=(.+)$" {
            $OPTIM=($argument -split "=", 2)[1]
        }
        "^-srd$|^--strip-runtime-debug$" {
            $RUNTIME_DEBUG=0
        }
        "^-nosh$|^--no-shell$" {
            $INCLUDE_SHELL=0
        }
        "^--capture$" {
            $DO_CAPTURE=1
        }
        default {
            Write-Host "${RED}Unknown option `"$argument`".$RESET"
            exit 1
        }
    }
}


# Parse "BuildWithLibs.txt" to decide which libraries to include in the build.
$libraryNames = @()
if (Test-Path "BuildWithLibs.txt") {
    foreach ($line in Get-Content "BuildWithLibs.txt") {
        # Only add lines beginning with ".".
        if ($line.StartsWith(".")) {
            $libraryNames+=$line.Substring(1)
        }
    }
}


# Choose optimization profile...
$optim=""
switch ($OPTIM) {
    "balanced" {
        $optim=$OPTIM_balanced
    }
    "speed" {
        $optim=$OPTIM_speed
    }
    "size" {
        $optim=$OPTIM_size
    }
}

Write-Host "(Optimization: $OPTIM)"




# Put everything into a final BUILD_ARGS variable.
$BUILD_ARGS=$optim+" "+$COMMON_BUILD_ARGS+" "+$LINKS

if ($RUNTIME_DEBUG) {
    $BUILD_ARGS+=" -DRUNTIME_DEBUG"
}
if ($INCLUDE_SHELL) {
    $BUILD_ARGS+=" -DINCLUDE_SHELL"
}




# Generate new source file with library definitons inserted...
$sourceLines = Get-Content "src/Ity.cpp"
$newSource = [System.Collections.Generic.List[string]]::new()
foreach ($line in $sourceLines) {
    $newSource.Add($line)
    if ($line -eq "//BUILDER_INSERT: Lib Includes") {
        foreach ($libraryName in $libraryNames) {
            $newSource.Add("#include `"Lib/$libraryName/m.hpp`"")
        }
    }
    if ($line -eq "//BUILDER_INSERT: Lib Names") {
        foreach ($libraryName in $libraryNames) {
            $newSource.Add("LIB_$libraryName,")
        }
    }
}

$newSource | Set-Content -Path "src/.Ity_tmp_generated.cpp" -Encoding utf8

if ($DEBUG) {
    Write-Host "${BOLD}Building debug binary...$RESET"
}
else {
    Write-Host "${BOLD}Building production binary...$RESET"
}

$start = Get-Date

Push-Location "src"
$result = 0

if ($DEBUG) {
    & $GPP "-g" @($BUILD_ARGS -split '\s+')
    $result = $LASTEXITCODE
}
else {
    if ($DO_CAPTURE) {
        & $GPP @($BUILD_ARGS -split '\s+') 2> "log.txt"
    }
    else {
        & $GPP @($BUILD_ARGS -split '\s+')
    }
    $result = $LASTEXITCODE
    if ($result -eq 0) {
        & strip "ity.bin" # Better results than "-s" flag for gcc.
        & objcopy `
            "--remove-section=.gnu.version" `
            "--remove-section=.note.ABI-tag" `
            "--remove-section=.note.gnu.property" `
            "--remove-section=.note.stapsdt" `
            "--remove-section=.comment" `
            "--remove-section=.annobin.notes" `
            "--remove-section=.gnu.build.attributes" `
            "--remove-section=.eh_frame_hdr" `
            "--remove-section=.eh_frame" `
            "ity.bin" `
            "ity.bin"
    }
}

if ($result -ne 0) {
    Write-Host "${RED}Build failed... Resolve errors then try again.${RESET}"
    exit 1
}

Pop-Location
Move-Item -Force "src/ity.bin" "ity.bin"
Remove-Item -Force "src/.Ity_tmp_generated.cpp"




# Print results...

$elapsed=(Get-Date)-$start
$seconds=[math]::Floor($elapsed.TotalSeconds)
Write-Host "Done in ${seconds}s."

# Get size difference between this & the last build.
$binSize = (Get-Item "ity.bin").Length
$previousBinSize = 0
if (Test-Path ".last_build_size") {
    $previousBinSize = [int64](Get-Content ".last_build_size")
}
$sizeDifference = $binSize - $previousBinSize
if ($sizeDifference -lt 0) {
    $differenceText = "${GREEN}$sizeDifference$RESET"
}
elseif ($sizeDifference -eq 0) {
    $differenceText = "${GREEN}+$sizeDifference$RESET"
}
else {
    $differenceText = "${RED}+$sizeDifference$RESET"
}
# Save size of current build for next diff.
Set-Content ".last_build_size" $binSize

# Print results...
Write-Host "Final size: ${ORANGE}${binSize}${RESET} bytes. ($differenceText)"
if ($binSize -gt $BIN_SIZE_LIMIT) {
    Write-Host "${RED}Binary size is over the goal of `"$BIN_SIZE_LIMIT`".$RESET"
}




# Run tests...

if ($DO_TESTS) {
    Write-Host
    Write-Host "Testing is not available on Windows at the moment."
}
