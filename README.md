
<div align="middle">

**Version:** 0.0.1

Ity is a light-weight interpreted programming language built with C++.

</div>

---


# Build interpreter
The build process for the Ity interpreter is very straight-forward. After cloning this repository you should notice a `build.sh` bash script inside the root directory.
Run the build script by typing `./build.sh` in your terminal, doing so will build the source code using the GCC compiler on your system.

After building has finished, it will immediately try to run test scripts with the interpreter to make sure everything is working fine.
If you are building from a published version you can disable these tests by running `./build.sh no` instead.


# Interpreter usage
After building the interpreter, you should see a file called `Ity.bin` which is the standalone interpreter executable.
You can run this in your terminal with `./Ity.bin`.

## Source mode
If you run the interpreter with the path of an Ity script file, it will parse then execute it without printing anything (unless debug flags are set).

## Interactive mode
Running the interpreter without passing a script file path will run it in "interactive" mode, similar to the Python interpreter's interactive mode, it allows you to immediately parse & execute the code you give it.

```bash
./Ity.bin -d-expr-result

>> 1+1

Expression result: {t=INT, d=2, m=0}

>> 
```

Inputting "exit" will stop the interpreter.

## Flags
You can pass flags to the interpreter to change how it behaves & select what information it will output.

Note: all flags must be passed before the source file path (if any).

Flags:

- `-d-full`:
	Enable all debug flags.
- `-d-expr-seq`:
	Print expressions as ExprTokens as soon as they are parsed.
- `-d-expr-result`:
	Print results from expressions as soon as they are executed.
- `-d-assign-data`:
	Print "set_data" calls on the current state.
