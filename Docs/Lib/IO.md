IO module provides the essentials for communicating via the process standard in & standard out pipes. IO also has other useful properties & methods which go beyond simple pipe communication.

# Properties
### (MAP) SIGNAL
A map of all supported system signals represented as an `INT`.

Map keys:
- `interrupt`: `2`
- `terminate`: `15`

# Methods

### STR in
Waits for then returns an input line `STR` via `stdin` pipe.

```python
# Print raw input.
const STR input = in:[];
print:input;

# Print input converted to a number.
const num_input = in:[] -> FLOAT;
print:num_input; # 0.0 if failed conversion.
```

### STR key_in
Same as `in` except it doesn't wait for a new line, instantly returns the next character in the `stdin` buffer.

```python
# Example script that accumulates input characters.
merge IO;

var chars = [];
while true;
	const ch = key_in:[];
	chars.append:ch;
	# If received ASCII code 3, then exit the loop.
	if ch.raw:[] == 3; break; /;
/;

# Print accumulated characters.
print:['\nGot: ', chars];
```

### STR prompt (STR text)
Same as `in` except it outputs the given text before accepting input.

```python
# Without `prompt`:
out:'Enter your name: ';
const name = in:[];

# With `prompt`:
const name = prompt:'Enter your name: ';
```

### NONE out (ANY ...var)
Output the stringified variants to `stdout` directly without modification. Instantly flushes output for immediate display in the terminal.

```python
out:'Hello World!\n';
# Hello World!
```

### NONE err (ANY ...var)
Same as `out` except it writes to `stderr` instead. Text is *not* colorized.

### NONE print (ANY ...var)
Outputs the stringified variants to `stdout` with a new line appended to the output. Instantly flushes output.

```python
print:'Hello World!';
# Hello World!
```

### NONE signal (INT signal_code, MAP(f) function)
Connects the given function to a system signal. Refer to the [SIGNAL](#map-signal) member for valid signal codes.

```python
func NONE on_signal_interrupt_reveived;
	print:'Received interrupt signal!';
	exit;
/;

signal:[(SIGNAL.interrupt), on_signal_interrupt_reveived];
sleep:999;

# Program usually ends when given the interrupt signal, however we override it's default behavior when connecting to it so we have to manually call exit.
#
# Anything the script is currently doing will be paused to call the signal callback function & resumed when the callback returns.
```

Note: connecting to a signal will override it's system default behavior. Which means connecting to signals which would normally exit the program, will no longer do so, the functionality needs to be recreated. In simpler terms, always manually exit the program in your connected signal function for signals that would normally do so.
