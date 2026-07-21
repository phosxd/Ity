Ity is a simple language compared to the larger languages like Python, C++, & even JavaScript. This means a lot of advanced features you would normally see in those languages don't exist here, in this part of town we call that  **B L O A T**.

The language is however still meant to be a real alternative, so all the essentials are here & are designed to be as readable & usable as possible.

After reading, or even before, I highly suggest looking at some of the example scripts in the repository. Pick apart what you can from the examples & try making something on your own!
As a little challenge, try one of these starter projects:
- Print a readable & formatted current date using the `IO` & `Time` modules. The time module only provides an integer representing the current time, good luck!
- Command line utility that sorts then returns every given argument in alphabetical order.

# Understanding the layout

An Ity script is first split up into individual instruction tokens. You can think of an instruction token like it's a line in Python, but the difference is that instead of splitting each instruction by line, Ity splits each instruction with a semicolon `;` character instead.

But what actually is an instruction token? It's an arbitrary sequence of arguments split up by a space character (unless the space is inside of a string literal). The very first argument is the name of the instruction to use, the rest of the arguments are passed onto the instruction for it's own uses.

```python
<instruction> <arg1> <arg2> <...>;
<instruction> <arg1> <arg2> <...>;
And so on...
```

This is exactly how commands are lied out in your terminal, which makes it very easy to remember, however most modern programming languages don't work like this so it might go against some of your habits.

There is one more thing to note with this. Some instructions may make use of expression syntax, in which case spacing your arguments will not matter wherever the expression is located.

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

Evaluating a named variable resolves into a reference of the original value, allowing operators to modify it directly.
However, setting a variable to a reference of another variable does not link the two, because the assignment operators do not set the target to the actual given variant, but instead copies the data from the given variant into the already existing variant inside the target.

```python
# "a" is unaffected
var INT a = 5;
var INT b = a;
b = 10;
a; # Resolves to 5
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
Holds a 64-bit signed floating point number.  It has a range of 1.7e−308 to 1.7e+308.

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

Strings can make use of `\` (back-slash) for escape sequences. Here is each one that is supported:
- (`\n`) New line.
- (`\t`) Tab.
- (`\e`) Escape character. Can be used to input ANSI.

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
my_arr:[(length:[my_arr]-1)];
```

## MAP
Holds an unordered hash table in which the keys are strings & the values are variants which can of any type. Maps can also represent complex objects like functions or interfaces, however it is discouraged to try to declare an object using map declaration syntax.

Unlike dictionaries you would find in most other languages, maps are declared like so: `{'key', 'value'}` instead of the convention `{'key': 'value'}`.

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

Are the returned variants from the access operator *copies* or *references*?
The answer is, it depends. For complex types like `ARR` & `MAP` the returned item is always a reference, not a copy. Meaning if you were to modify it, the change would directly affect the item inside the data structure.

The only other type that can be accessed is `STR`, & due to how strings are stored, the accessed character is always a copy of the one inside the string.

## TypeCast ( -> )
The type-cast operator will attempt to translate the first value to the target type. This works for every built-in type except for `NONE`, `ARR` & `MAP`.

```python
'true' -> BOOL;  # Gives boolean true
1 -> BOOL;       # Gives boolean true
0 -> BOOL;       # Gives boolean false

9_350 -> STR;    # Gives "9350"
'10250' -> INT;  # Gives 10_250
```

## Assignment
Assignment operators allow you to modify the value of a variant. This is useful for setting the value of named variables after declaration.
### =
Overwrite the data of the variant.
### +=
Add to the variant.
### -=
Subtract from the variant.
### \*=
Multiply the variant.
### /=
Divide the variant.
### %=
Modulo the variant.


```python
var INT number = 1;
number += 1;       # "number" is now 2
number -= 2;       # "number" is now 0
number = (100/2);  # "number" is now 50
number /= 2;       # "number" is now 25

var INT number_2 = 10;
number = number_2;  # "number" copies the value of "number_2" (10)
number_2 = 20;      # "number" is unchanged.
```

---
# Instructions
The standard set of instructions in Ity provide you with everything you need to make programs of any ranging complexity.

## Var / Const
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

To modify the value of a variable, you can use the assignment operators in an expression.

```python
var ANY a = 100;
a *= 2; # "a" is now 200
a = 0; # "a" is now 0
a = ('Hello ' + 'World');
```

Although you need to be careful evaluating complex expressions after the assignment operator. In the example `a = 'Hello ' + 'World'`, execution happens in this order:
- Get reference to "a".
- Set reference "a" to the next item in the expression (`'Hello '`) through the `=` operator.
- Operator `+` is taking the output of everything before it `a = 'Hello '`
- Operator `+` adds the previous output to the next output.
The result of this expression is `Hello World` but `a` only gets set to `'Hello '` because we added `World` to the *output* of `a = 'Hello '`. You can get around this by grouping multi-part expressions after your assignment operator like so: `a = ('Hello ' + 'World')`.

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

## While / Continue / Break
You can repeatedly execute blocks of code using the `white` composite instruction.
Before every iteration the given expression gets evaluated. If it returns `true` then the code block will run & return back to the beginning for the next iteration.

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
	i += 1;
/;
```

You can skip to the next iteration by using the `continue` instruction.

```python
import IO;

const STR string = "Hello World!";
# Print each character in the string, with some delay.
var INT i = -1; while i < (length:[string]-1); i+=1;
	# Skip white space characters.
	if string:i == " ";
		continue;
	/;

	# Print then wait...
	IO.out:[(string:i)];
	sleep:[0.1];
/;


IO.out:['\n'];
```

The `break` instruction will stop the loop completely.

```python
merge IO;

var INT i = -1; while true; i += 1;
	# Break at 10.
	if i == 10;
		break;
	/;
	print:[i];
/;
```

## Throw / Exit
You can terminate the program safely, or with an error, using `throw` or `exit`.

This example throws an error once recursion depth reaches 1,000:

```python
func NONE recurse; arg INT n = 0;
	if n == 1000;
		throw "Okay, let's stop now: " + (n->STR); # Stop at 1000.
	/;
	recurse:[(n+1)];
/;

recurse:[];
```

Using `exit` will quit the program with exit code `0` & print nothing.

```python
exit;
```

# Libraries & Globals
A list of global variables & functions can be found [here](Globals.md).
Libraries enable you to actually do more complex things with relative ease, here are all the built-in libraries:
- [IO](Lib/IO.md)
- [Time](Lib/Time.md)
- [Math](Lib/Math.md)
- [FileAccess](Lib/FileAccess.md)
