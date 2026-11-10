The standard set of instructions in Ity provide you with everything you need to make programs of any ranging complexity.
## Var
Declare a name which holds a value that can be changed after declaration.

`var [type] <name> [<assignmentOperator> <expr>]`

**Examples**:

```python
var ANY this_can_be_anything_but_has_no_value_right_now;
var NONE this_has_no_value;
var . this_also_has_no_value;

var NONE this_has_no_value_but_its_explicitly_set = none;
var BOOL is_true = true;
var INT the_number_one = 1;
var FLOAT a_very_specific_number = 125.98765;
var STR some_text = "Hello World!";
var * my_inferred_var = "This can be any value at declaration";
var also_an_inferred_var = "Not specifiying a type will make it inferred";

# You don't need spaces between the name, assignment operator, & expression.
var BOOL some_bool=true;
```

To modify the value of a variable, you can use the assignment operators in an expression.

```python
var ANY a = 100;
a *= 2; # "a" is now 200
a = 0; # "a" is now 0
a = ('Hello ' + 'World');
```

You need to be careful evaluating complex expressions after the assignment operator. In the example `a = 'Hello ' + 'World'`, execution happens in this order:
- Get reference to "a".
- Set reference "a" to the next item in the expression (`'Hello '`) through the `=` operator.
- Operator `+` is taking the output of everything before it `a = 'Hello '`
- Operator `+` adds the previous output to the next output.
The result of this expression is `Hello World` but `a` only gets set to `'Hello '` because we added `World` to the *output* of `a = 'Hello '`. You can get around this by grouping multi-part expressions after your assignment operator like so: `a = ('Hello ' + 'World')`.

## Const
Declare a name that has a value which **cannot** be modified or overwritten after declaration.

`const [type] <name> [<assignmentOperator> <expr>]`

**Examples**:
```python
const INT my_constant_value = 20;

# This is not allowed.
const ANY some_constant_value = 20;

# This is fine.
const * inferred_constant = true;
const inferred_constant_2 = true;
```

Because constants cannot be modified, it would be useless to assign it the "ANY" type, doing so will throw an error.

## If / Elif / Else
These instructions can be used to test values for conditionally executing code. These are **composite** instructions, meaning in addition to taking arguments like a normal instruction, it also consumes all instructions that are defined after it, up until the `/` (end) instruction.

`if <boolExpr`

`elif <boolExpr`

`else`

**Examples:**
```python
var INT a = 0;
var INT b = 5;

# This will fail, & not run.
if a == b;
	# ...
/;

# This will also fail.
elif a > 10;
	# ...
/;

# This will succeed because all other conditions failed.
else;
	# ...
/;


# You can nest conditionals.
if a != b;
	if a > 1;
		# ...
	/; else;
		# ...
	/;
/;
```

## Func / Arg / Return
Declare a named block of code which can be called upon at a later time.

`func [type] <name>`

`arg [type] <name> [<assignmentOperator> <expr>]`

`return <expr>`

**Examples:**

```python
# Adds 2 numbers together then returns the result, guaranteed to be an `INT`.
func INT add_numbers; arg INT a; arg INT b;
	return a + b;
/;


## Adds any 2 values together then returns the result. This can cause an error as some types do not support addition.
func ANY add_anything; arg a; arg b;
	return a + b;
/;


# If a function does not return a value, you do not need to give it a type.
func do_something;
	# ...
/;
```

Call a function by using the "access" (`:`) operator.

```python
var INT x = 0;

func do_something;
	x += 1;
/;

# Execute the function, without passing any arguments.
do_something:[]; # "f:()" also works.


func INT add; arg INT a; arg INT b;
	return a + b;
/;

# Execute the function with arguments.
add:[5,5];
```

## While / Continue / Break
You can repeatedly execute blocks of code using the `while` composite instruction.
Before each iteration, the given expression gets evaluated. If it returns `true` then the code block will run & return back to the beginning for the next iteration.

`while <boolExpr>`

`continue`

`break`

```python
# Loop forever, the expression returns the literal "true" so the condition will always be met.
# The loop can still be ended with the "break" instruction.
while true;
	IO.print:"Hello! You're stuck with me forever >:)";
/;

# Count to 100 thousand.
var INT i = 0;
while i < 100_000;
	IO.print:i;
	i += 1;
/;
```

You can skip to the next iteration by using the `continue` instruction.

```python
import IO;

const STR string = "Hello World!";
# Print each character in the string, with some delay.
var INT i = -1; while i < (string.length:() - 1); i+=1;
	# Skip white space characters.
	if string:i == " ";
		continue;
	/;

	# Print then wait...
	IO.out:(string:i);
	sleep:0.1;
/;


IO.out:'\n';
```

The `break` instruction will stop the loop completely.

```python
merge IO;

var INT i = -1; while true; i += 1;
	# Break at 10.
	if i == 10;
		break;
	/;
	print:i;
/;
```

## For
A more convenient way to iterate over a set of items.

`for <name> in <iterExpr>`

```python
merge IO:

# Print numbers 0 to 9.
for i in 10;
	print:i;
/;


# Print items 1, 2, "3".
for i in [1,2,'3'];
	print:i;
/;


# Print numbers 0, 2, 4, 6, 8.
for i in range:[0,10,2];
	print:i;
/;


# Print keys "c", "b", "a".
# Order cannot be guaranteed.
const MAP map = {'a',1, 'b',2, 'c',3};
for key in map.keys:();
	print:key;
/;
```

The `range` function can be useful, but it comes at a cost of allocating a literal array with numbers inside, which is bad for large ranges. For larger ranges you should consider using the `RangeIter` script-module which uses a method that does not allocate memory.

```python
# Safer range iteration.
import RangeIter;

# Print numbers 0, 2, 4, 6, 8.
for i in RangeIter.new:[0,10,2];
	print:i;
/;
```

## Throw / Exit
Terminate the program safely, or with an error.

`exit [code=0]`

`throw [errorMessage]`

This example throws an error once recursion depth reaches 1,000:

```python
func NONE recurse; arg n = 0;
	if n == 1000;
		throw "Okay, let's stop now: " + (n->STR); # Stop at 1000.
	/;
	recurse:(n+1);
/;

recurse:();
```

Using `exit` will quit the program with the given exit code (default `0`).

```python
exit;
exit 1; # Exit with code 1.
```

## Import / Merge
Modules allow you to expand the capabilities of your script, use the `import` instruction to add a module to the current scope. You can use the "as" keyword followed by a name to import the module & assign it under your custom name.

`import <module> [as <alias>]`

`merge <module>`

```python
import IO; # Import IO built-in module.
IO.print:'Hello World!';

import IO as custom_name; # Import IO, but assign it a different name.
custom_name.print:();

# Importing the same module more than once works as long as they have unique names.
```

You can dynamically import a module using the "@" symbol to specify a string variable to use as the module name.

```python
const STR module_name = 'IO';
import @module_name as some_module; # Import dynamically.

# As good practice, you should verify the member you're looking for exists in "some_module" before trying to access it from a dynamcally imported module if you can't be absolutely sure it has the member.
if some_module.has:'print';
	some_module.print:'Hello There!';
/;
```

Import modules located anywhere on your file system.

```python
import my_modules/foo;
import 'my modules/bar'; # Wrap in quotes if path has a space.
foo.hello:();
bar.hello:();

# These do the same things.
import /usr/local/share/ity/Modules/RangeIterator as RI;
import RangeIterator as RI_2;
```

If you don't want to assign a name to an imported module at all, use the `merge` instruction which will directly add all the module's members to the current scope.

However this should be used with caution, if you merge a module that just so happens to have a member with the same name as something else in the current scope then you will get a "name taken" error.

```python
merge IO;
print:'Hello World!';
```

The `merge` instruction will not work with dynamic names ("@") or aliases ("as").
