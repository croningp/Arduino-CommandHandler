## CommandHandler

A Wiring/Arduino library to tokenize, parse and relay commands received either by the serial port, as a string, or char by char.

The message format is:
```
CmdString,arg1,[...],argN;
```

Although the field separator ',' and command separator ';' can be changed.

The library can:
- Attach callback functions to received command
- Relay the remaining of a command to attached callback functions (typically another CommandHandler)
- Parse a command char by char
- Parse a string command
- Receive commands through the serial port
- Read multiple arguments
- Read all primary data types
- Forging of string packet with multiple arguments of different primary type


#### Features and main difference with [SerialCommand](https://github.com/kroimon/Arduino-SerialCommand) and [CmdMessenger](https://github.com/thijse/Arduino-CmdMessenger)

Unlike [CmdMessenger](https://github.com/thijse/Arduino-CmdMessenger), the command identifier can be any string, which make your code more readable

It differs with [SerialCommand](https://github.com/kroimon/Arduino-SerialCommand) by the ability to nest commandHandlers. This has been made possible by a simple change: adding the ability to read from string. While not being at all a technical challenge to implement, it can be really useful to modularize your code.

Imagine your main program receiving "M1,P,2000;" command through Serial. The main program could redirect the "P,2000;" string command to the a sub-device called "M1". The M1 device can then use its own commandHandler to handle the sub command "P,2000;". To extract the remaining of a command, you can use the function remaining(), give that remaining to another commandHandler instance (e.g. the one associated to M1) through the processString() function. The command handler of M1 can then parse it and enjoy all the feature of the commandHandler, i.e. callback and casting of arguments.

Please refer, and read through, the [Demo example](examples/Demo/Demo.ino) for practical usage of this library.

All the above steps can be encapsulated by registering relay callback function. When triggered by the associated command, the command handler with call the relay command, passing in argument the remaining of the command. This behavior is illustrated in the [Arduino-CommandAccelStepper](https://github.com/croningp/Arduino-CommandAccelStepper) library.

We are trying to implement Command[Name] devices. We also implemented a [CommandManager](https://github.com/croningp/Arduino-CommandManager) that standardize and ease the use and setting up of Command[Name] devices.

### Getting started

Download or clone this repository, rename the folder as CommandHandler and move it to your Arduino libraries folder. You need to restart the Arduino IDE for the library to be loaded and recognized by Arduino.

Please refer to https://www.arduino.cc/en/Guide/Libraries#toc5 for manual installation of libraries.

### Inspiration

This is derived from the SerialCommand library whose original version was written by [Steven Cogswell](http://husks.wordpress.com) (published May 23, 2011 in his blog post ["A Minimal Arduino Library for Processing Serial Commands"](http://husks.wordpress.com/2011/05/23/a-minimal-arduino-library-for-processing-serial-commands/)). It is based on the [SerialCommand heavily modified version with smaller footprint and a cleaned up code by Stefan Rado](https://github.com/kroimon/Arduino-SerialCommand).

Some features are also extracted from [CmdMessenger](https://github.com/thijse/Arduino-CmdMessenger), such as the function returning next arg already casted in types, e.g. readInt16Arg

### Author

[Jonathan Grizou](http://jgrizou.com/) while working in the [CroninGroup](http://www.chem.gla.ac.uk/cronin/).

### License

[![GPL V3](https://www.gnu.org/graphics/gplv3-127x51.png)](https://www.gnu.org/licenses/gpl.html)
