
<div align="middle">

**Version:** 0.2.0

Ity is a work-in-progress, light-weight & embeddable interpreted programming language built with C++.

</div>

---


# Table of contents
- [Features](#features)
- [Build](#build)
  - [Build flags](#build-flags)
  - [Optimization profiles](#optimization-profiles)
  - [Installation](#installation)
- [Usage](#usage)
  - [Source mode](#source-mode)
  - [Interactive mode](#interactive-mode)
  - [Interpreter flags](#interpreter-flags)
- [Language](#language)
  - [Docs](Docs/Main.md)
  - [Examples](Scripts/)


# Features
- **Tiny & portable.** 124KB executable. All self-contained.
- **Efficient memory usage.** 400KB runtime (Python: 11.2MB, Node: 45MB, Bash: 3.2MB).
- **Platform agnostic.** No platform dependent source code.
- **No build setup.** Requires no third-party libraries.
- **Practical.** I/O, file management, time, & math modules all built-in. With many more pre-built opt-in modules.
- **Modular.** Designed to be torn apart & pieced back together.
- **Extendible.** Build your own modules in Ity itself to extend your capability.

This is my first project in C++, I'm sure there are things that I have done wrong or have messed up, don't be afraid to open an issue to tell me!

**NOTE:** No AI was used in the development process at all. Purely human made, & will continue to be as such.
	AI assisted issues or PRs will be rejected.


# Build
The build process for the Ity interpreter is very straight-forward. After cloning this repository you should notice a `build.sh` bash script inside the root directory.
Run the build script by typing `./build.sh` in your terminal, doing so will build the source code using the GCC compiler on your system.

## Build flags
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
- `-nosh` or `--no-shell`
	Remove interactive capability.
- `-t` or `--test`:
	Run tests after compilation.

## Optimization profiles
As mentioned in the flags section, you can choose from 3 optimization profiles. Balanced is the default & is what is used for benchmarks, but it is not the most performant.
If you're looking to get the most speed out of Ity, use the "speed" profile which increases overall speed by about 28% but comes at the cost of a slightly larger binary (still under 250kb).
On the other hand, if you are *really* constrained with file size & are willing to deal with an about 9% slower runtime then you can use the "size" profile, which decreases the final binary size to almost exactly 100kb.

The overall differences between each profile is largely insignificant in most real-word cases, so unless you are working with tight constraints I wouldn't worry about changing the optimization profile.
Modifying the source code to remove built-ins or even entire operators & instructions that you don't use in your codebase, will probably be much more valuable if you need to make the most of every kilobyte.

## Installation
You can install Ity on your system as a command called `ity` by running the `install.sh` script.


# Usage
After building or downloading the interpreter, you should see a file called `ity.bin` which is the standalone interpreter executable.
You can run this in your terminal with `./ity.bin`.

## Source mode
If you run the interpreter with the path of an Ity script, it will parse then execute it.

```
~$: ity my_script.ity
```

Alternatively you can add a shebang to your script & execute it directly.

```python
#!/usr/local/bin/ity
import IO;
IO.print:'Hello World!';
```

```
~$: ./hello_world.ity
Hello World!
```

## Interactive mode
Running the interpreter without passing a script path will run it in "interactive" mode. It allows you to immediately parse & execute the code you give it.

```bash
./ity.bin
* Ity (0.2.0)
* Runing interactive mode interpreter.
* Type "quit" or "q" to stop.

>> 1+1
2
>> var a = 'Hello '; var b = 'World!'

>> a+b
Hello World!
>>
```

## Interpreter flags
You can pass flags to the interpreter to change how it behaves & select what information it will output.

Flags:

- `-s` or `--safe`:
	Prevent the interpreter from importing potentially unsafe modules or running unsafe functions.
	Basically anything that can mess with your system.

- `-c` or `--codes`:
	Print simple error / warning codes instead of detailed messages.
- `--nowarn`:
	Don't emit any warning messages.
- `--step`:
	Wait for confirmation before running the next instruction.

- `-d` or `--d-full`:
	Enable all debug flags.
- `--d-result`:
	Print program times & other information when completed.
- `--d-inst`:
	Print instructions as InstTokens as they're executed.
- `--d-expr`:
	Print expressions as ExprTokens as they're executed.
- `--d-expr-result`:
	Print results from expressions as soon as they are executed.
- `--d-assign-data`:
	Print "set_data" calls on the current state.
- `--d-scoping`:
	Print scope in/out/alt calls.

- `-t=` or `--tabs=*`:
	(Default: 4). Change this to the tab width you use in your text editor for more accurate error column counting.

---

# Language
The full documentation is available [here](Docs/Main.md). This explains everything in great detail.

But if you prefer to be lazy, here are some quick examples:

### Declare
```python
# Types:
# 	ANY, NONE, BOOL, INT, FLOAT, STR, ARR, MAP, REF.

var a = 0; # Type is inferred as INT.
var INT b; # Value defaults to 0.

var ANY c;     # Value defaults to none.
print:(type:c)
c = 99;        # Type is now INT, because he's can be whatever he wants. If he wants to grow up to be a string, god damn it let him be string.
print:(type:c)
```

### Loop
```python
merge IO;

# Based af loop.
var INT i=0; while i < 100; i+=1;
	print:i;
/;

# Loop for Python kiddies.
for i in 100;
	print:i;
/;

# Loop through an array like a based person.
const ARR arr = [1,'2',3,'4',5];
var INT i=-1; while i < (length:[arr]-1); i+=1;
	print:(arr:i);
/;

# Actually, maybe this is better to read...
for item in arr;
	print:item;
/;
```

### Reuse
```python
func INT add; arg INT a=0; arg INT b=0;
	return a + b;
/;

add:[1,1];
add:[4,5];
```

### Calculate
```python
# fibonacci.ity
import IO;
const n = IO.prompt:'Number: ' -> INT;

var FLOAT a=0.0; var FLOAT b=1.0;

for i in n;
	const c = a+b;
	a = b; b = c;
	IO.print:a;
/;
```
