Holds an unordered hash table in which the keys are strings & the values are variants which can of any type. Maps can also represent complex objects like functions, interfaces, or custom types with their own iterators & methods.

If you require a map that can hold other simple data types as a key (not just `STR`), use the `VarMap` module.

Unlike dictionaries you would find in most other languages, maps are declared like so: `{'key', 'value'}` instead of the convention `{'key': 'value'}`.

```python
{'a',1, 'b':2};
{
	'integer_field', 900_000,
	'float_field', 0.28450,
	'string_field', "Hello there!",
	'keys can have spaces', 'However, it wont be accessible with the "." symbol',
};
```

You can get a value in a dictionary key by using the accessor operator, or "." accessor syntax.

```python
var MAP my_map = {'a',1, 'b',2};
my_map.a; # Gives value at "a", which is 1.
my_map:'a'; # Does the same thing.
```

To set the value of a key in a map, you can use the accessor operator in conjunction with the assignment operator if the key is already in the map. If the key-value pair does not already exist, this will not work.

To add a brand new pair, instead of overwriting an existing one, use the `set` type method or the `+=` assignment operator to add multiple pairs into a new map.

```python
var MAP my_map = {'a',1, 'b',2};

# Overwrite map item.
my_map.a = 100;
# my_map = {'a',100, 'b',2}

# Add to map.
my_map.set:['c',3];
my_map += {'d',4, 'e',5};
# my_map = {'a',100, 'b',2, 'c',3, 'd',4, 'e',5}
```

If you want to check if a key exists in the map, you will need to use the `has` type method on the map.

```python
const MAP my_map = {'a',1, 'b',2};
my_map.has:'a'; # Returns true.
```

Removing a key-value pair from a map also requires a type method. Use `erase` to do this.

```python
my_map.erase:'a';
# my_map = {'b',2, 'c',3}
```
