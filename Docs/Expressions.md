Expressions don't have any strict rules on spacing & can be used for defining literals, calling functions, & performing mathematical operations.

Expressions have operators, each with their own purpose & functionality. However you should keep note that Ity does not keep a precedence, operators are treated equally. Every operator is evaluated one after another with the result of the previous operator feeding into the next.

For example, in the expression `2*4+4`, the order of operations is `2*4 -> 8+4` & it goes like this regardless of the operator being used, which may be something to get used to as it is done differently in some languages.

# Grouping
Expressions can be grouped using parenthesis characters `(` & `)`. An empty grouping produces the value `none`.

Here is an example of how grouping may be used to change the result of an expression.

```
2*4+4; # This equals 12.
2*(4+4); # This equals 16, same variables, same order, different grouping.
```

Grouping is essential when using certain operators. Using assignment operators (Like `=`, `+=`, etc) especially need to be used with groupings as remember, operators take no precedence. So usually in other languages `x = 1+1` would work just fine, but in Ity you need to use a grouping `x = (1+1)` otherwise the order of operations would be `x = 1 -> x+1`.
