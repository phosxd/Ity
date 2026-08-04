
<div align="middle">

**Version:** 0.1.0

Ity is a work-in-progress, light-weight interpreted programming language built with C++.

</div>

---


# Overview
- **Tiny & portable.** < 150KB executable. All self-contained.
- **Efficient memory usage.** 400KB runtime (Python: 11.2MB, Node: 45MB, Bash: 3.2MB).
- **Platform agnostic.** No platform dependant code.
- **No build setup.** Requires no third-party libraries.
- **Practical.** I/O, ~~file management~~, time, & math modules all built-in.

This is my first project in C++, I'm sure there are things that I have done wrong or have messed up, don't be afraid to open an issue to tell me!

**NOTE:** No AI was used in the development process at all. Purely human made, & will continue to be as such.
	AI assisted issues or PRs will be rejected.


# Build interpreter
The build process for the Ity interpreter is very straight-forward. After cloning this repository you should notice a `build.sh` bash script inside the root directory.
Run the build script by typing `./build.sh` in your terminal, doing so will build the source code using the GCC compiler on your system.

After compilation has finished, the build script will display the time it took to compile & the final build size along with the difference in size compared to the previous build.

```
(Optimization: balanced)
Building prodution binary...
Done in 4s.
Final size: 116384 bytes. (+0)
```

If the final build size is larger than 150,000 bytes it will give you a warning message.

```
Binary size is over the goal of 150000.
```

## Flags
You can pass flags to the build script.

- `-o=` or `--optimize=`:
	Set optimization level.
	"balanced", "speed", or "size".
- `-s` or `--static`:
	Statically link final binary. Comes at no performance benefiet & increases binary size about 1.5MB.
- `-d` or `--debug`:
	Build with debug symbols in-tact.
- `-srd` or `--strip-runtime-deubg`
	Remove runtime error strings & debug flag branches.
	Error codes & ln/col tracking are still kept.
	Use this to save an extra few kilobytes.
- `-t` or `--test`:
	Run tests after compilation.

## Optimization profiles
As mentioned in the flags section, you can choose from 3 optimization profiles. Balanced is the default & is what is used for benchmarks, but it is not the most performant.
If you're looking to get the most speed out of Ity, use the "speed" profile which increases overall speed by about 23% but comes at the cost of a slightly larger binary (still under 200kb).
On the other hand, if you are *really* constrained with file size & are willing to deal with an about 19% slower runtime then you can use the "size" profile, which decreases the final binary size to somehwere around 82-85kb.

The overall differences between each profile is largely insignificant in most real-word cases, so unless you are working with tight constraints I wouldn't worry about changing the optimization profile.

## Installation
You can install Ity on your system as a command called `ity` by running the `install.sh` script.


# Interpreter usage
After building the interpreter, you should see a file called `ity.bin` which is the standalone interpreter executable.
You can run this in your terminal with `./ity.bin`.

## Source mode
If you run the interpreter with the path of an Ity script file, it will parse then execute it without printing anything (unless debug flags are set or manual print calls are executed inside the script).

Passing a command line argument after the file path, without using the "-" prefix (indicating interpreter option), will make that argument available to the script under the `__CMD_ARGS__` global.

## Interactive mode
Running the interpreter without passing a script file path will run it in "interactive" mode, similar to the Python interpreter's interactive mode, it allows you to immediately parse & execute the code you give it.

```bash
./ity.bin
* Ity (0.1.0)
* Runing interactive mode interpreter.
* Type "quit" or "q" to stop.

>> 1+1
2
>> 
```

Entering "quit" or "q" will stop the interpreter.

The last expression that gets executed in your line will be printed to the console automatically.

## Flags
You can pass flags to the interpreter to change how it behaves & select what information it will output.

Flags:

- `-safe`:
	Prevent the interpreter from importing potentially unsafe modules.
	With this option enabled, only a select few modules can be imported:
		IO, Time, Math.
	
	This will also disable certain members/functions:
		system.

- `-codes`:
	Print simple error / warning codes instead of detailed messages.
- `-nowarn`:
	Don't emit any warning messages.
- `-step`:
	Wait for confirmation before running the next instruction.

- `-d-full`:
	Enable all debug flags.
- `-d-result`:
	Print program times & other information when completed.
- `-d-inst-seq`:
	Print instructions as InstTokens as soon as they are parsed.
- `-d-expr-seq`:
	Print expressions as ExprTokens as soon as they are parsed.
- `-d-expr-result`:
	Print results from expressions as soon as they are executed.
- `-d-assign-data`:
	Print "set_data" calls on the current state.
- `-d-scoping`:
	Print scope in/out/alt calls.

- `-tabs=*`:
	(Default: 4). Change this to the tab width you use in your text editor for more accurate error column counting.


# Language usage
The full documentation is available [here](Docs/Main.md). This explains the syntax of the language in great detail. It also goes over all built-in types & how to use them.
