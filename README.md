# arduino-i2c128x64-display
Arduino project that writes strings to display per display frame  
Allows for a total of 144 characters to be displayed on the display, in a 18x8 format (18 characters, 8 lines)

Tested with the following hardware:  
`ELEGOO Nano Board CH340/ATmega328P`  
`I2C 0.96-inch OLED Display SSD1306 128x64 Pixels IIC 3.3V 5V White`  

## Arduino Code
A fairly simple implimentation and uses the `Adafruit_SSD1306` combined with the `Adafruit_GFX` Arduino libraries.  
Listens over serial for strings and writes them to the display.  
The string must end with a single `\n` character. (displayable strings should be 145 characters including `\n`)  
Strings may be more or less then 144 characters, however anything after 144 characters will be ignored.
This only works with strings and ascii characters up to 255, and ignores any `\n` characters

## Python - example: 1
This is to demonstrate how serial communication can be acheived with python.  
The objective is to show how to manage and send (and ultimately display) simpe textual messages on the screen.

## Python - example: 2
This similarly demonstrates `python - example: 1`, with the addition of the accompanying module that I have written.  

The `Frame` Python module builds and handles formatting of textual inputs destined to the Arduino display.  

After a `Frame` instance has been instantiated, each line of the frame thereafter must also be instantiated and defines the number of columns available for the line.  
Columns for a line cannot be changed after instantiation; thus a new frame must be declared and defined.  

Defined column text can be redefined, along with textual alignment.

See below for a small example of the module API:

```python
from frame.frame import Frame

# init new frame
frame = Frame()

frame.init_line(1, 1)  # init line '1' with 1 column
frame.line(1).setcolumntext(1, 'test')  # align defaults to 'left'

frame.init_line(2, 2)  # init line '2' with 2 columns
frame.line(2).setcolumntext(1, 'test', 'center')
frame.line(2).setcolumntext(2, 'test', 'center')

frame.init_line(3, 3)
frame.line(3).setcolumntext(1, 'test', 'left')
frame.line(3).setcolumntext(2, 'test', 'center')
frame.line(3).setcolumntext(3, 'test', 'right')

frame.init_line(4, 1)
frame.line(4).setcolumntext(1, 'test', 'center')

frame.init_line(5, 2)
frame.line(5).setcolumntext(1, 'test', 'center')
frame.line(5).setcolumntext(2, 'test', 'center')

result = frame.getstring_encoded()  # returns string in bytes

# ...handle result ...
```

