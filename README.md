at2xtkbd
========

Simple Arduino based AT to XT keyboard converter

This is a fork from kesrut/tinyPS2toXTArduino.

I did a lot of refactoring on the original code and added some new stuff:
- support for extended keyboard keys (arrows, insert/delete/home/end/pgup/pgdown)
- turning leds on and off when caps/num/scroll lock are pressed
- in progress support for print screen and pause break keys.
  They are detected at the AT keyboard side, but are probably being mapped wrongly to XT codes.

Also, it seems that I've messed up the keyboard self test check. At least on my XT it doesn't work anymore.

This is still a work in progress.
The issues above need to be fixed, and more testing needs to be done.

There are pin mapping comments on the .ino file.

Use it at your own risk!
