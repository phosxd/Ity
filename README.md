
<div align="middle">

**Version:** 0.0.1

Ity is a light-weight interpreted programming language built with C++.

</div>

---


# Build interpreter
The build process for the Ity interpreter is very straight-forward. After cloning this repository you should notice a `build.sh` bash script inside the root directory.
Run the build script by typing `./build.sh` in your terminal, doing so will build the source code using the GCC compiler on your system.

After compilation has finished, the build script will display the time it took to compile & the final build size along with the difference in size compared to the previous build.

```
Building prodution binary...
Done in 2s.
Final size: 93288 bytes. (+0)
```

If the final build size is larger than 100,000 bytes it will give you a warning message.

```
Binary size is over the goal of 100000.
```

## Flags
You can pass flags to the build script.

Flags:

- `-d` or `--debug`:
	Build with debug symbols in-tact.
- `-t` or `--test`:
	Run tests after compilation.


# Interpreter usage
After building the interpreter, you should see a file called `Ity.bin` which is the standalone interpreter executable.
You can run this in your terminal with `./Ity.bin`.

## Source mode
If you run the interpreter with the path of an Ity script file, it will parse then execute it without printing anything (unless debug flags are set).

## Interactive mode
Running the interpreter without passing a script file path will run it in "interactive" mode, similar to the Python interpreter's interactive mode, it allows you to immediately parse & execute the code you give it.

```bash
./Ity.bin
* Ity (0.0.1)
* Runing interactive mode interpreter. Run Ity code directly in the terminal!
* Type "quit" or "q" to stop.

>> 1+1
2
>> 
```

Inputting "exit" will stop the interpreter.

## Flags
You can pass flags to the interpreter to change how it behaves & select what information it will output.

Note: all flags must be passed before the source file path (if any).

Flags:

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
