Holds a reference to a variable stored in the scope. This will act exactly like the referenced variable for most cases.

`PTR` (different from `REF`) is more of an internal type which should not be used if possible. The only time is should be used is when it is fully required, usually in type methods & iterator functions.

Setting, accessing, comparing, or in any way operating on a reference/pointer will most of the time apply to the referenced variable, not the reference itself. Think of it as the 2 variables are hard-linked. This makes it really convenient to work with. You can dereference a `REF` or `PTR` by using the `~` (tilda) (deref) unary operator.

```python
merge IO;

var my_var = 100;
const my_ref = @my_var; # "@" unary operator creates a reference to a variable name.

print:(~my_ref); # 100.
my_var = 101;
print:(~my_ref); # 101.
my_ref = 99;
print:my_ref; # "REF:my_var".
print:my_var; # 99.

# my_ref & my_var are essentially interchangable in most cases.
```

The `@` unary operator can only be used on variable names to create a `REF` to it. To create a `PTR` you must use the `@>` unary operator. What makes this so powerful (and dangerous) is that pointers can reference *any* value located *anywhere* (array items, map items, values in other modules, etc).

```python
const INT my_var = 100;
const PTR ptr = @>my_var;
print:(~ptr); # 100.

# Point to variant in a container...
const ARR array = [1,2,3];
const PTR arr_item_ptr = @>(array:0);
print:(~arr_item_ptr); # 1.
array:0 = 11;
print:(~arr_item_ptr); # 11.
```

The `type` built-in function if used on a `REF` or `PTR` will return the `REF` or `PTR` type, not the type of the value being referenced. To get the proper type you can use `type` as a type-method on the `REF` / `PTR` itself.

```python
merge IO;

const INT my_var = 100;
const REF my_ref = @my_var;

# Get type of referenced value in pointer.
print:(type_name:(type:my_ref)); # Prints "REF", that's not what we want.
print:(type_name:(type:(~my_ref))); # Prints "INT", the actual held value type.
print:(type_name:(my_ref.type:[])); # Prints "INT", this is better for performance, not passing a copy of the value to the `type` function.
```

To reassign the reference you can use the `reassign` type-method. `PTR` variants cannot be reassigned currently.

```python
merge IO;

const a = 'a';
const b = 'b';

var my_ref = @a;
print:(~my_ref); # Prints "a".
my_ref.reassign:(@b);
print:(~my_ref); # Prints "b".
```

If the referenced value ever gets destroyed or goes out of scope, then the `REF` will be reset to reference `none`. `PTR` on the other hand is *unsafe*, destroying the referenced value then trying to access it through a pointer is undefined behavior & should never be done. **Always** ensure that a pointer is not used after it's value is destroyed.

```python
merge IO;

const STR other_global_value = 'Other Global';


# Define a function that reassigns our `REF`.
func NONE reassign_ref; arg REF ref; arg local=false;
	if local;
		const STR local_value = 'Local';
		(~ref).reassign:(@local_value); # We deref so we don't reassign the copy given in the function argument.
	/;
	else;
		(~ref).reassign:(@other_global_value);
	/;
/;


const STR global_value = 'Global';
var REF my_ref = @global_value; # Initialize with some global constant value.
print:(~my_ref); # Prints "Global".

reassign_ref:[(@my_ref), false]; # Reassign to another global value which is *not* destroyed after the function returns.
print:(~my_ref); # Prints "Other Global";
reassign_ref:[(@my_ref), true]; # Reassign to a local value which *is* destroyed after the function returns.
print:(~my_ref); # Prints none. The pointer was assigned to a variable that got destroyed.
```
