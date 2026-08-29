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

### (STR) \_\_SCRIPT\_PATH\_\_
A string representing the path of the script file which the interpreter is running.
This will be empty if the interpreter is running interactive mode.

Note: this may not be an absolute file path, it is often relative to the current working directory.

### (ARR) \_\_CMD\_ARGS\_\_
An array representing the string arguments passed to the script via the command line.
This does not include any interpreter flags or interpreter arguments.

### (BOOL) \_\_IMPORTED\_\_
A boolean representing whether the script is running as a module or as the main program.

```python
if __IMPORTED__ == false;
	print:'Running as main program!';
/;
```

### \[Types\]
Every built-in data type, represented as a string. They share the same names as the ones you would use to declare a variable.

E.g. `NONE` = "NONE"

# Standard Global Functions

### INT system (STR command)
Run a system command string then return the exit status code.

```python
system:'echo Hello World!'; # Returns 0.

system:'NonExistentCommand'; # Returns 32512.
# sh: line 1: NonExistentCommand: command not found
```

### NONE sleep (INT/FLOAT seconds)
Pause thread execution by the given number of seconds.

```python
sleep:1; # Pause for 1 second.
sleep:0.5; # Pause for half of a second.
```

### STR type_name (INT type)
Return a `STR` representation of the given type number .

```python
type_name:(type:'Hello World!'); # Returns "STR".

const INT type_value = type:123;
type_name:type_value; # Returns "INT".
```

### INT type (ANY var)
Return an `INT` representing the active type of a given variable. Equal to built-in type constants (E.g. `BOOL`, `INT`, `FLOAT`, etc).

The literal `INT` value returned by `type` should not be relied upon as they may vary across different versions of Ity. Always compare against the type constants or use `type_name`.

```python
type:'Hello World!'; # Returns 12 (STR).

merge IO;

if type:'string' == STR;
	print:'Found string!';
/;
```

### INT length (ARR/STR var)
Return an `INT` representing the length of the given `ARR` / `STR` variable.

```python
length:'Hello World!'; # Returns 12.
length:[[1,2,3,4]]; # Returns 4.
```

### INT size (ANY var)
Return an `INT` representing the size (in bytes) of a given variable. Including variant type & mode data (2 bytes extra).

```python
size:0;              # Returns 4 (32-bit int) + 2 (type & mode info).
size:'Hello World!'; # Returns the number of characters in the string + 2.
size:[[1,2,3,4]];    # Returns 50.

# Arrays & maps are particularly bulky.
size:{'a',1, 'b',2, 'c',3, 'd',4}; # Returns 86.
size:{'array value',[1,2,3,4]};    # Returns 119.
```

### ARR range (INT start, INT end, INT step=1)
A function that returns an `ARR` of `INT` values in sequential range of "start" & "end", with the given "step".

```python
range:[0, 5];    # Gives [0, 1, 2, 3, 4].
range:[0, 5, 2]; # Gives [0, 2, 4].
```


# Standard Global Type Methods

### NONE `REF`.reassign (REF to)
Reassign a `REF`. Will not work on constant reference variants.

```python
const a = 1;
const b = 2;

var ref = @a; # References `a`.
ref.reasssign:@b; # Now references `b`.

const c_ref = @a;
ref.reassign:@b; # Fails, `c_ref` is permanently linked to `a`.
```

### INT `REF`.type ()
Return the data's type referenced by the `REF` or `PTR`.

```python
var a = 99;
var REF = @a;

type:ref == REF; # True, checking type of the REF itself.
ref.type:[] == REF; # False, data is INT.

```

### INT `STR`.raw ()
Get an ASCII code for the first character in the string.

```python
const STR string = 'a';
string.raw:[]; # Returns 97.
```

### NONE `ARR`.erase (INT index)
Delete an item inside the array.

```python
var ARR array = [1,2,3];
array.erase:0;
# array = [2,3]
```

### NONE `ARR`.append (ANY var)
Add a variant to the end of the array.

```python
var ARR array = [1,2,3];
array.append:4;
# array = [1,2,3,4]
```

### NONE `MAP`.erase (STR key)
Delete a key-value pair inside the map.

```python
var MAP map = {'a',1, 'b',2, 'c',3};
map.erase:'a';
# map = {'b'2, 'c',3}
```

### NONE `MAP`.set (STR key, ANY var)
Add or overwrite a key-value pair to the map.

```python
var MAP map = {'a',1, 'b',2};
map.set:['c',3];
# map = {'c',3, 'b',2, 'a',1}
```

### ARR `MAP`.keys ()
Get an unsorted array of `STR` values representing keys inside of the map.

```python
const MAP map = {'a',1, 'b',2, 'c',3};
map.keys:[]; # Returns ["c", "b", "a"].
```

### BOOL `MAP`.has (STR key)
Returns whether or not the given key exists inside the map.

### FUNC `FUNC`.bind (ANY ...args)
Returns a new function object with the given arguments added to it's bound arguments.

Calling a function with bound arguments will pass them as the first arguments into the function, user specified arguments will be last.

```python
func BOOL flip; arg BOOL value;
	if value; return false; /;
	return true;
/;


const FUNC bound_flip_function = flip.bind:true;
bound_flip_function:[]; # Returns false, because we bound true to the first argument.
```
