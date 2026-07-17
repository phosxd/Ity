Ity is a simple language compared to the larger languages like Python, C++, & even JavaScript. This means a lot of advanced features you would normally see in those languages don't exist here, in this part of town we call that  **B L O A T**.

The language is however still meant to be a real alternative, so all the essentials are here & are designed to be as readable & usable as possible.

# Understanding the layout

An Ity script is first split up into individual instruction tokens. You can think of an instruction token like it's a line in Python, but the difference is that instead of splitting each instruction by line, Ity splits each instruction with a semicolon `;` character instead.

But what actually is an instruction token? It's an arbitrary sequence of arguments split up by a space character (unless the space is inside of a string literal). The very first argument is the name of the instruction to use, the rest of the arguments are passed onto the instruction for it's own uses.

```python
<instruction> <arg1> <arg2> <...>;
<instruction> <arg1> <arg2> <...>;
And so on...
```

This is exactly how commands are lied out in your terminal, which makes it very easy to remember, however most modern programming languages don't work like this so it might go against some of your habits.

## Comments
Comments are defined by the "#" character. When a comment is defined, all characters after it until the new line are ignored. In this regard, it acts just the same as Python.

```python
# This is a comment
"# this is not a comment, we're in a string";
```

## Expressions

An instruction may make use of the expression system for some parts of their logic, you can also run expressions alone, without a call to an instruction.

Expressions don't have any strict rules on spacing & can be used for defining literals, calling functions, & performing mathematical operations.

`1+1;`

You can use named variables inside expressions.

`a+b;`

You can use parenthesis to evaluate an expression inside of another expression.

`(1+1) + 2; # Equals 4;`

Evaluating a named variable resolves into a copy of the original value. To get a pointer to the original value instead of copying it, start your expression with the "$" character.
You must be careful when using pointers, if the original variable name gets deleted then the pointer will just return a "NONE" type value.

When setting the value of a pointer variable, you should use the "set" *method* (`a.set:['value']`) instead of the "set" *instruction* (`set a = 'value'`). The instruction will overwrite the pointer itself, while the method will set the actual value of the pointer.
The reason it works like this is because the "set" instruction acts on named variables in the scope data, not on the actual value of the variant.

```python
# "a" is affected, "a" & "b" are linked;
var INT a = 5;
var PTR b = $a;
b.set:[1];
a; # Resolves to 1;

# "c" is unaffected;
var INT c = 5;
var INT d = a;
d.set:[1];
c; # Resolves to 5;
```

---
# Types / Literals
## NONE
This type holds no value. You can pass it by typing `none`.

This cannot be operated on in any way, the only operator this can be used with is the equality operator (`==`).

## BOOL
Holds a binary value (0 or 1). You can pass it by typing `true` or `false`.

```python
true;
false;
```

## INT
Holds a 32-bit signed integer. It has a maximum value of 2,147,483,647 & will wrap around or potentially crash if this limit is exceeded. 

```python
1; 2; 3;
-1; -2; -3;
```

## FLOAT
Holds a 32-bit signed floating point number. 

```python
0.0; 1.34291;
-1.0; -29.476;
```

## STR
Holds a string of UTF-8 encoded characters.

```python
"This is a string";
'This is also a string';

"Strings can have
	multiple lines!
";
```

Strings can make use of `\` (back-slash) to escape characters.

```python
"Line 1\nLine 2";
"Give me my \"double quotes\"!";
```

You can get an individual character from a string by using the accessor operator on it.

```python
const STR my_str = 'abcdefg';
my_str:0;  # Gives "a";
my_str:5;  # Gives "f";
'abc':1;   # Gives "b";
```

## ARR
Holds an array of variants which can of any type.

```python
[1,2,3];
["Hello", " ", "World"];

[
	[1,2,3],
	[4,5,6],
];
```

You can get an element from the array by using the accessor operator on it.

```python
const ARR my_arr = [1,2,3];
my_arr:0; # Gives 1;

# Get last element;
my_arr:(length:[my_arr]-1);
```

## MAP
Holds an unordered hash table in which the keys are strings & the values are variants which can of any type. Maps can also represent complex objects like functions or interfaces, however it is discouraged to try to declare an object using map declaration syntax.

Unlike dictionaries you would find in most other languages, maps are declared like so: `{'key', 'value'}` instead of the convention `{'key': 'value'}`. It is done this way for the sake of simplicity.

```python
{'a',1, 'b':2};
{
	'integer_field', 900_000,
	'float_field', 0.28450,
	'string_field', "Hello there!",
	'keys can have spaces', 'However, it wont be accessible with the "." symbol'
};
```

---

# Operators
An operator takes 2 variants & processes them to make a new value in an expression. There are only 4 primary operator types in Ity, each one has a unique purpose.

## Arithmetic
### +
Add to a value. This works on numbers, arrays, & strings.
### -
Subtract from a value. Only works on numbers.
### \*
Multiply a value. This works on numbers, arrays, & strings.
### /
Divide a value. Only works on numbers.
### %
(Modulo) Divide a value & return the remainder. Only works on numbers.

## Comparison
Comparison operators return a boolean `true` or `false` value.
### ==   !=
Check if 2 values are exactly the same.
Or if the 2 values are not the same.

```python
true == false; # Returns false
false == false; # Returns true
```
### >   >=
Check if the first value is larger than the second value.
Or if the first value is equal to the second value.

```python
1 > 0; # Returns true
0 >= 1; # Returns false
```
### <   <=
Check if the first value is smaller  than the second value.
Or if the first value is equal to the second value.

```python
1 < 0; # Returns false
1 <= 1; # Returns true
```
### &&   ||
Check if first value & second value are true. Doesn't evaluate second value if first value is false.
Or check if first value or second value are true. Doesn't evaluate second value if first value is true.

```python
1 == 1 && true; # Returns true
1 == 0 && true; # Returns false
1 == 1 || false; # Returns true
1 == 0 || true; # Returns false
```

## Access ( : )
This operator allows you to get object properties, array/string elements, & allows you to call functions.

Other languages usually split the function call, property access, & element lookup syntax across 3 symbols when in most cases it can be 1.

```python
my_func:[arg_1, arg_2]; # Runs "my_func" then returns the result
my_array:1; # Returns the second element in "my_array"
my_string:1; # Returns the second character in "my_string"

var INT i = 5;
my_array:i; # Returns the fifth element of "my_array", as that's the value of "i"
```

## TypeCast ( -> )
The type-cast operator will attempt to translate the first value to the target type. This works for every built-in type except for `NONE`, `ARR` & `MAP`.

```python
'true' -> BOOL;  # Gives boolean true
1 -> BOOL;       # Gives boolean true
0 -> BOOL;       # Gives boolean false

9_350 -> STR;    # Gives "9350"
'10250' -> INT;  # Gives 10_250
```

---
# Instructions
The standard set of instructions in Ity provide you with everything you need to make programs of any ranging complexity.

## Var / Const / Set
There are 3 instructions to assign data in Ity, but which one you use depends on your use case.

### Var
If you need to declare a name that holds a value that can be changed after declaration, use the `var` instruction:

`var <type> <varName> <assignmentOperator> <expression>`

**Arguments**:
- "type":
	Required argument. This tells the instruction the type of value that "expression" should return & the type that the variable will be permanently set as. 
	If the "type" is passed as `*`, then the type will be automatically set as the expression output type (this is not the same as the `ANY` type).
- "varName":
	Required argument. The name given to the variable so that it can be accessed from other instructions in the scope, the name should not match any other name declared in the scope otherwise an error will be thrown, it should also not have any symbols in it (numbers & underscores are fine if used properly).
- "assignmentOperator":
	Required argument. The operator which indicates how the expression output is applied to the variable. The only correct value for this argument is the `=` character.
- "expression":
	Required argument. The output of this expression will be applied to variable, spaces do not matter here because it is at the end of the instruction.

**Examples**:

```python
var ANY this_can_be_anything_but_has_no_value_right_now;
var NONE this_has_no_value;

var NONE this_also_has_no_value = none;
var BOOL is_true = true;
var INT the_number_one = 1;
var FLOAT a_very_specific_number = 125.98765;
var STR some_text = "Hello World!";
var * my_inferred_var = "This can be any value at declaration";
```

### Const
If you need to declare a name that has a value which **cannot** be modified or overwritten after declaration, use the `const` instruction. It has the same syntax as the `var` instruction.

**Examples**:
```python
const INT my_constant_value = 20;

# This is not allowed;
const ANY some_constant_value = 20;

# This is fine;
const * inferred_constant = true;
```

Because constants cannot be modified, it would be useless to assign it the "ANY" type, doing so will throw an error.

### Set
To change the value of a named variable you need to use the `set` instruction. This will overwrite the variable at the specified name with a new variant, discarding the old one.
If you want to preserve the variant itself while just changing the value (useful for preserving pointers references), then you should use the `set` *method* on the variable.

`set <varName> <assignemntOperator> <expression>`

**Arguments**:
- "varName":
	Required argument. The name of the variable you are trying to access. Will throw an error if the name does not exist.
- "assignmentOperator":
	Required argument. The operator which indicates how the expression output is applied to the variable. This can be the `=` operator to overwrite the previous value, & for certain types you can use the `+=`  /  `-=` operators to add or remove value from the current value.
- "expression":
	Required argument. The output of this expression will be applied to variable, spaces do not matter here because it is at the end of the instruction. If the output does not match the pre-defined type of the variable then an error is thrown.

**Examples**:

```python
var ANY variable = 0;
set variable = 1;
set variable += 1; # Now 2;
set variable = "The variable type is now string!";
set variable += " Added more string to the string!";
set variable = 8;
set variable /= 2; # Now 4;
set variable *= 2; # Now 8;
```

This instruction only works on named variables, this means you cannot set array elements, map keys/values, or object properties using the "set" keyword. Each type has their own methods for updating properties.

## If / Elif / Else
These instructions can be used to test values for conditionally executing code. These are **composite** instructions, meaning in addition to taking arguments like a normal instruction, it also consumes all instructions that are defined after it, up until the `/` (end) instruction.

```python
var INT a = 0;
var INT b = 5;

# This will not run;
if a == b;
	# ... ;
/;

# This will also fail;
elif a > 10;
	# ... ;
/;

# This will succeed because all other conditions failed;
else;
	# ... ;
/;


# You can nest conditionals;
if a != b;
	if a > 1;
		# ... ;
	/; else;
		# ... ;
	/;
/;
```

## While
You can loop sections of code in 2 different ways.

The first way is to use the dedicated looping instruction "while" which will run all instructions composited below it as over & over until the given condition is no longer met. The condition is checked before the beginning of each iteration.

```python
# Loop forever, the expression returns the literal "true" so the condition will always be met;
# The loop can still be ended with the "break" instruction;
while true;
	IO.print:["Hello! You're stuck with me forever >:)"];
/;

# Count to 100 thousand;
var INT i = 0;
while i < 100_000;
	IO.print:[i];
	set i += 1;
/;
```


# Libraries & Globals
A list of global variables & functions can be found [here](Docs/Globals.md).
Libraries enable you to actually do more complex things with relative ease, here are all the built-in libraries:
- [IO](Docs/Lib/IO.md)
- [Time](Docs/Lib/Time.md)
- [Math](Docs/Lib/Math.md)
- [FileAccess](Docs/Lib/FileAccess.md)
