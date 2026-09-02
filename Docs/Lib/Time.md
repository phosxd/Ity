Time is a simplistic module providing only the necessary functions for program & system time measurement. This *does not* provide sleep functions or other utilities, the `sleep` function is a built-in.

Time measurement functions will require a `mode` parameter, which dictates the format in which the time is returned. Here are the expected values:
- `us` Microseconds
- `ms` Milliseconds
- `s` Seconds
- `m` Minutes
- `h` Hours
- `d` Days
- `w` Weeks
- `M` Months
- `y` Years

# Methods

### INT snow (STR mode)
Get the current real world time according to the host system clock. The returned value is counted up from the year 1970, therefor a value of 0 would represent some time in the year 1970.

```python
snow.y; # Returns 56, for 2026.
```

### INT now (STR mode)
Get the current program time since start-up.

```python
now.y;  # Returns 0.
now.ms; # Returns 0.
now.us; # Returns around 50-150 microseconds.

sleep:1; # Wait 1 second.

now.s; # Returns 1 second.
```
