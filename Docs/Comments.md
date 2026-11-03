Comments are defined by the "#" character. When a comment is defined, all characters after it until the new line are ignored. In this regard, it acts just the same as Python.

```python
# This is a comment
```

The reason for using a "#" character instead of "//" or "/\*" is because it's simple, plays nicely with [shebangs](https://en.wikipedia.org/wiki/Shebang_%28Unix%29) while being familiar.

Comments can be inlined anywhere. In arrays, maps, expressions, you name it.

```python
[
	1, # Hello!
	2, # This is an inline comment.
	3,
];
```

Strings capture all characters inside of it, so you don't have to worry about accidentally creating a comment.

```python
"This is a string, `#` characters don't create comments, just like `;` characters don't end the statement";
```
