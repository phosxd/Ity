IO module provides the essentials for communicating via the process standard in & standard out pipes. IO also has other useful properties & methods which go beyond simple pipe communication.

# Properties
### (MAP) SIGNAL
A map of all supported system signals represented as an `INT`.

Map keys:
- `interrupt`: `2`
- `terminate`: `15`

### (MAP) ANSI
A map of common [ANSI](https://en.wikipedia.org/wiki/ANSI_escape_code) escape codes represented as a string. This is useful for printing stylized text in the terminal.

Map keys:
- `reset`
- `bold`
- `black`
- `red`
- `green`
- `orange`
- `blue`
- `purple`
- `white`
- `yellow`

# Methods
### NONE signal (INT signal_code, MAP(f) function)
A function that connects the given function to a system signal. Refer to the [SIGNAL](#map-signal) member for valid signal codes.

```python
merge IO;

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
