An operator takes 2 variants & processes them to make a new value in an expression. There are only 4 primary operator types in Ity, each one has a unique purpose.

## Arithmetic
### +
Add to a value. This works on numbers, arrays, maps & strings.
### -
Subtract from a value. Only works on numbers.
### \*
Multiply a value. This works on numbers, arrays & strings.
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
my_func:arg; # Runs "my_func" then returns the result
my_func:[arg_1, arg_2]; # You can pass multiple arguments by wrapping in an array.

my_array:1;  # Returns the second element in "my_array"
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

### Ternary ( ?  -- )
The ternary operation is split across 2 operator symbols. The first symbol (`?`) returns the second operand only if the first operand is `true`, otherwise an internal variant is returned, which cannot be used.

The second symbol (`--`) returns the second operand only if the first operand is *not* an internal variant.

In conjunction, these 2 operators form a [ternary conditional operator](https://en.wikipedia.org/wiki/Ternary_conditional_operator).

```python
const some_value = true ? 1 -- 2;
# some_value = 1;

# Condtional function calls with ternary...
import IO;
func NONE f1; IO.print:1 /;
func NONE f2; IO.print:2; /;

const x = 100;
x==99 ? (f1:[]) -- (f2:[]);
# 2
```

### Reference ( `@` )
The reference operation creates a new `REF` variant which references the variant path given on the right-side of the operator. A path can be a simple variable name, or nested keys / indices.

```python
const INT x = 99;
const REF ref = @(x);

var ARR arr = [1,[2,[3,4]]];
const REF nested_ref = @(arr:0:0:1); # References `4`.
```
### Dereference ( `~` )
The dereference unary operator takes a `REF` or `PTR` & returns a copy of the referenced variant. For `REF`, if the path is no longer valid, returns `none`. Trying to dereference an invalid `PTR` will result in unexpected behavior or a crash.

## Assignment
Assignment operators allow you to modify the value of a variant. This is useful for setting the value of named variables after declaration.
### =
Overwrite the data of the variant.

### <<=
Move data into the target variant, this resets the source variant.

What happens to the source variant depends on it's type. If it's an `ARR`,  `MAP`, or `STR`, then all items are moved out. If it's a simple type like `NONE`, `BOOL`, `INT`, or `FLOAT`, then the value is copied & stays the same.

You cannot move data in to or out of a constant variable.

```python
var ARR a = [1,2,3];
var ANY b;
b <<= a; # Move "a" into "b"

# a = []
# b = [1,2,3]

var INT c = 99;
var ANY d;
d <<= c;

# c = 99
# d = 99
```
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
