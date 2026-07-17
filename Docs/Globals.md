Ity is packed with a ton of global constants & functions that any script can access without importing anything.

# Standard Global Properties
### (ARR) \_\_VERSION\_\_
An array of integer values representing the interpreter version.
This should have exactly 4 items, with the first 3 being the actual version numbers & the last one being the release type.

The release type can be `0` for release, `1` for beta / pre-release, or `2` for experimental / custom.

### (STR) \_\_VERSION_STRING\_\_
A string representing the interpreter version. Does not include release type.

### (STR) \_\_OS\_NAME\_\_
A string representing the host OS' name. This will vary based on the platform the interpreter was compiled for, which is not always the platform in which the interpreter is currently running.

List of possible values:
- `linux`
- `windows`
- `apple_mac`
- `apple_iphone`
- `unix`
- `posix`
- `unknown`

### (STR) \_\_SCRIPT\_FILE_NAME\_\_
A string representing the name of the script file which the interpreter is running.
This will be empty if the interpreter is running interactive mode.

Note: this is not the full file path, rather it is just the file name (extension included).

### (INT) \_\_SCRIPT\_START\_TIME\_MS\_\_
An integer representing the time (in milliseconds) when the script started running. This uses the host system last epoch time.

### (ARR) \_\_CMD\_ARGS\_\_
An array representing the string arguments passed to the script via the command line.
This does not include any interpreter flags or interpreter arguments.

### \[Types\]
Every built-in data type, represented as a string. They share the same names as the ones you would use to declare a variable.

E.g. `NONE` = "NONE"

The only types that are not included here are meta types like `ANY` & `*` (inferred).

### (MAP) ANSI
A map of common [ANSI](https://en.wikipedia.org/wiki/ANSI_escape_code) escape codes represented as a string. This is useful for printing stylized text in the terminal.

Map keys:
- `reset`
- `bold`
- `black`
- `red`
- `green`
- `orange`
- `blue`
- `purple`
- `white`
- `yellow`

### MAP get\_state ()
A function that returns a `MAP` representing the state of the caller scope. In other words, a map of all variables / functions in the current scope.

One use for this, is to check if a name is currently being used in the scope or to iterate on every variable in the scope.

### NONE sleep (INT/FLOAT seconds)
A function that pauses thread execution by the given number of seconds.

```python
sleep:[1]; # Pause for 1 second.
sleep:[0.5]; # Pause for half of a second.
```

### STR type_name (ANY var)
A function that returns a `STR` representing the active type of a given variable.

```python
type_name:['Hello World!']; # Returns "STR".


merge IO;

var ANY var = 4; # Var is INT.
const STR var_type = type_name:[var];

if (var_type == 'INT') || (var_type == 'FLOAT');
	print:['Var is a number!'];
/;
elif type_name:[var] == 'STR';
	print:['Var is a string!'];
/;
```

### INT length (ARR/STR var)
A function that returns an `INT` representing the length of the given `ARR` / `STR` variable.

```python
length:['Hello World!']; # Returns 12.
length:[[1,2,3,4]]; # Returns 4.
```

### INT size (ANY var)
A function that returns an `INT` representing the size (in bytes) of a given variable.

```python
size:[0]; # Usually returns 4 (assuming a 32-bit integer).
size:['Hello World!']; # Returns the number of characters in the string.
size:[[1,2,3,4]]; # Returns 52.

size:[{'a',1, 'b',2, 'c',3, 'd',4}]; # Returns 52 as well.
size:[{'array value',[1,2,3,4]}]; # Returns 71.
```
