For Ity, memory management is in a sort of middle-ground between low-level compiled languages & high-level interpreted languages. This is because the user doesn't have to worry about things like the stack or heap or the lifetime of the object, but you *still* have the ability to utilize certain tricks that are usually only available in languages like Rust, C or C++.

# Lifetime
The goal (as with all high-level languages) is to make memory management something that the user never has to worry about. That's why Ity stores **everything** on the stack & calls memory pointers a thing of the past!

No reference counting, no convoluted garbage collection. Your data simply goes whenever you exit the scope it was declared in. If you want permanent data, put it somewhere in the global scope, as simple as that.

# Persistence
Anytime you have to pass data to a new location, it is *always* going to be copied. However this works differently inside expression fields. Everything is treated as a reference when inside of an expression!

You can make use of the `REF` data type to store a reference to a variant storage cell (variable name, array item, or map item), which avoids copying the data you want to reference. However `REF`s will return a `noneref` if the variable it references no longer exists.

# Moving data
Anytime you have to pass data to a new location in the scope, it is *always* going to be copied. However, you can transfer the ownership of a value by using the "move" assignment operator (`<<=`). Here's an example of that in use:

```python
# Simple script to measure the performance cost of moving
# large blobs of data versus copying it into an array.
import Time;

var all_the_data = [];
const do_copy = prompt:'Do copy (true or false)? ' -> BOOL;
print:['Picked: ', (do_copy->STR), '\n'];

const start = Time.now.ms;


for i in 3;
	print:i;
	var big_data = range:1_000_000;
	if do_copy;
		all_the_data.append:[big_data];
	/;
	else;
		all_the_data.append:none;
		all_the_data:(-1) <<= big_data;
	/;
/;


print:['\nDone in ', ((Time.now.ms-start) / 1000.0), 's.'];
print:['Data size: ', (size:[all_the_data]), ' bytes.'];
```
