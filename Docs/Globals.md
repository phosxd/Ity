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

### (MAP) SIGNAL
A map of all supported system signals represented as an `INT`.

Map keys:
- `interrupt`: `2`
- `terminate`: `15`

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

# Standard Global Functions

### NONE signal (INT signal_code, MAP(f) function)
A function that connects the given function to a system signal. Refer to the [SIGNAL](#map-signal) member for valid signal codes.

```python
func NONE on_signal_interrupt_reveived;
	print:['Received interrupt signal!'];
	exit;
/;


signal:[(SIGNAL.interrupt), on_signal_interrupt_reveived];
sleep:[999];

# Program will end when given the interrupt signal, but it will call the connected function first.
```

Note: connecting to a signal will override it's system default behavior. Which means connecting to signals which would normally exit the program, will no longer do so, the functionality needs to be recreated. In simpler terms, always manually exit the program in your connected signal function for signals that would normally do so.

### INT system (STR command)
A function that runs a system command string then returns the exit status code.

```python
system:['echo Hello World!']; # Returns 0.

system:['NonExistentCommand']; # Returns 32512.
# sh: line 1: NonExistentCommand: command not found
```

### NONE sleep (INT/FLOAT seconds)
A function that pauses thread execution by the given number of seconds.

```python
sleep:[1]; # Pause for 1 second.
sleep:[0.5]; # Pause for half of a second.
```

### MAP get\_state ()
A function that returns a `MAP` representing the state of the caller scope. In other words, a map of all variables / functions in the current scope.

One use for this, is to check if a name is currently being used in the scope or to iterate on every variable in the scope.

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


# Standard Global Type Methods

### NONE `ARR`.erase (INT index)
Delete an item inside the array.

### NONE `MAP`.erase (STR key)
Delete a key-value pair inside the map.

### ARR `MAP`.keys ()
Get an unsorted array of `STR` values representing keys inside of the map.

### BOOL `MAP`.has (STR key)
Returns whether or not the given key exists inside the map.

### MAP(f) `MAP(f)`.bind (ARR args)
Returns a new function object with the given `ARR` added it's bound arguments.

Calling a function with bound arguments will pass them as the first arguments into the function, user specified arguments will be last.

```python
func BOOL flip; arg BOOL value;
	if value; return false; /;
	return true;
/;


const MAP bound_flip_function = ( flip.bind:[[true]] );
bound_flip_function:[]; # Returns false, because we bound true to the first argument.
```
