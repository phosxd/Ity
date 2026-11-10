Holds a string of raw bytes.

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

You can get an individual byte from a string by using the accessor operator on it.

```python
const STR my_str = 'abcdefg';
my_str:0;  # Gives "a".
my_str:5;  # Gives "f".
'abc':1;   # Gives "b".
```

Adding any number type to a string will append the ASCII representation of that number to the string.

```python
var STR my_str = 'ab';
my_str += 99; # 99 = 'c'.
# my_str = 'abc'
```
