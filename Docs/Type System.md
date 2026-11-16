Ity's has a standard set of native types you can use in your programs, see below:

- [NONE](Types/NONE.md)
- [BOOL](Types/BOOL.md)
- [INT](INT.md)
- - I64 (INT)
- - I32
- - I16
- - I8
- - U64 (UINT)
- - U32
- - U16
- - U8
- [FLOAT](Types/FLOAT.md)
- - F64 (FLOAT)
- [STR](Types/STR.md)
- [ARR](Types/ARR.md)
- [MAP](Types/MAP.md)
- [REF/PTR](Types/REF.md)
- [FUNC](Types/FUNC.md)

These are all the essentials types needed to create any program. Ity does not have classes or "struct" features. The closest thing to a class we have is the `MAP` type which can hold named variables inside as well as functions.

# Type Methods
Each type has their own set of methods that can be called on them. These are called "type methods". For example, this is how you would call the `get` method on a `MAP`:

```python
const MAP m = {'a',1, 'b',2};
m.get:'a';
```

Type methods are not completely baked in. They can change during run-time by modifying the global `__tm__` map which contains keys in the format of `<type>:<method_name>` & `FUNC` values. You can add, overwrite, or remove any type methods at your own risk. Here's an example of adding a new type method & using it:

```python
# Multiplies the string by 2.
func STR_double;
	__S.append:(~__S); 
/;

__tm__.set:['STR:double', STR_double];

var my_str = 'Hello World!';
my_str.double:(); # my_str = 'Hello World!Hello World!'
```

The `__S` variable holds a `PTR` to the object you called the type-method on. This pointer is only valid inside of the function call, do not attempt to store or use it outside of the function scope.
