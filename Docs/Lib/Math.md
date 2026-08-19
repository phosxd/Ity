# Methods

### ANY \[abs,log,sqrt\] (INT/FLOAT number)
Operates on the given number & returns the result. The output type will match the input type.

```python
merge Math;

abs: -5; # Returns 5.
abs:5; # Returns 5.

log:100; # Returns 4.
sqrt:100; # Returns 10.
```

### INT \[round,floor,ceil\] (FLOAT number)
Returns the number rounded to the nearest, lowest, or highest integer.

```python
merge Math;

round:1.3; # Returns 1.
floor:2.7; # Returns 2.
ceil:2.1; # Returns 3.
```

### FLOAT pow (INT/FLOAT base, INT/FLOAT exp)
Returns `base` to the power of `exp` as a `FLOAT` regardless of the input types.

### ANY sum (INT/FLOAT ...numbers)
Returns the accumulation of all the given `INT`/`FLOAT` numbers. The return type will be `INT` if all inputs are `INT`, if even a single input is `FLOAT`, then the result will be `FLOAT`.

```python
import Math;

Math.sum:[2,2,4,4]; # Returns 12.
Math.sum:[0.5, 1,1,1]; # Returns 3.5.
# Same as this:
var sum = (0.5 + 1 + 1 + 1);
# sum = 3.5
```

### INT rand (INT min, INT max)
A function that returns an `INT`, which is a random number between the given minimum & maximum values. Uses the standard C `rand` function internally.

```python
rand:[0, 10]; # Some random value from 0 to 10.
```

### NONE set_seed (INT seed)
A function that overrides the randomization seed for use in `rand`. The seed is by default set to the script start time according to the host system.

```python
set_seed:0;
rand:[0,100_000]; # ALWAYS returns 71_341 on first call.
```
