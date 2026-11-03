There are multiple integer types for varying ranges of values.
## I64 (INT)
Holds a 64-bit signed integer.  It has a range of 9,223,372,036,854,775,808 to 9,223,372,036,854,775,807. This is the default type when typing out a number.

```python
1; 2; 3;
-1; -2; -3;

var INT a;
var I64 b;
```
## I32
Holds a 32-bit signed integer. It has a range of -2,147,483,648 to 2,147,483,647.
## I16
Holds a 16-bit signed integer. It has a range of -32,768 to 32,767.
## I8
Holds an 8-bit signed integer. It has a range of -128 to 127.
## U64 (UINT)
Holds a 64-bit unsigned integer. It has a range of 0 to 18,446,744,073,709,551,616. You can define a `UINT` literally by adding a `u` character to the end of a number literal. For example: `120u`.
## U32
Holds a 32-bit unsigned integer. It has a range of 0 to 4,294,967,296.
## U16
Holds a 16-bit unsigned integer. It has a range of 0 to 65,535.
## U8
Holds an 8-bit unsigned integer. It has a range of 0 to 255.
