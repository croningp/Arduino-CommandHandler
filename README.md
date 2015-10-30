## CommandHandler

A Wiring/Arduino library to tokenize and parse commands received either by the serial port, as a string, or char by char.

The message format is:
```
CmdString,arg1,[...],argN;
```

Although the field separator ',' and command separator ';' can be changed.

The library can:
- Attach callback functions to received command
- Parse a command char by char
- Parse a string command
- Receive commands through the serial port
- Both read multiple arguments
- Both read all primary data types

We expect to support soon:
- Forging of string packet with multiple argument of different primary type

The most important feature of this library is the ability to read from string, while not being at all a technical challenge to implement, it is useful to modularize your code. Commands can be nested one into another.

Imagine your main program receiving "M1,P,2000;" command through Serial. The main program could redirect the "P,2000;" string command to the a sub-device called "M1". The M1 device can then use its own commandHandler to handle the sub command "P,2000;". To extract the remaining of a command, you can use the function remaining(), give that remaining to another commandHandler instance (e.g. the one associated to M1) through the processString() function. The command handler of M1 can then parse it and enjoy all the feature of the commandHandler, i.e. callback and casting of arguments.

Please refer, and read through, the [Demo example](examples/Demo/Demo.ino) for practical usage of this library.

### Inspiration

This is derived from the SerialCommand library whose original version was written by [Steven Cogswell](http://husks.wordpress.com) (published May 23, 2011 in his blog post ["A Minimal Arduino Library for Processing Serial Commands"](http://husks.wordpress.com/2011/05/23/a-minimal-arduino-library-for-processing-serial-commands/)). It is based on the [SerialCommand heavily modified version with smaller footprint and a cleaned up code by Stefan Rado](https://github.com/kroimon/Arduino-SerialCommand).

Some features are also extracted from [CmdMessenger](https://github.com/thijse/Arduino-CmdMessenger), such as the function returning next arg already casted in types, e.g. readInt16Arg

This code has been written by Jonathan Grizou while working in the CroninGroup.

### License

[![LGPL V3](http://www.gnu.org/graphics/lgplv3-147x51.png)](http://www.gnu.org/licenses/lgpl-3.0.en.html)
