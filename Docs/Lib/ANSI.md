Implements common [ANSI](https://en.wikipedia.org/wiki/ANSI_escape_code) sequences. This can be used for 16-color & RGB text coloring, text styling, & terminal controls.

# Styles:

- `reset`
- `bold`
- `dim`
- `italic`
- `underline`
- `blinking`
- `reverse`
- `hidden`
- `strike`

Prepend with `reset_` to reset the given style.

# Colors

- `black`
- `red`
- `green`
- `yellow`
- `blue`
- `purple` (magenta)
- `cyan`
- `white`
- `default`

Prepend with `fg_` to get a foreground color, prepend with `bg_` to get a background color.

If your terminal supports it, you may be able to get RGB colors using the `fg_rgb` or `bg_rgb` functions which accepts exactly 3 integer arguments representing red, green & blue.

# Controls

- `clear_screen`
- `cursor_off`
- `cursor_on`

### STR move (INT x, INT y)
Move the cursor to the given coordinate values.

### STR move_\[up,down,left,right\] (INT value)
Relatively move the cursor by the given amount.
