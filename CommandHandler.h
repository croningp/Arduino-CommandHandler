/**
 * CommandHandler - A Wiring/Arduino library to tokenize and parse commands
 * received in different forms, serial, string, or char.
 *
 * Copyright (C) 2015 Cronin Group http://www.chem.gla.ac.uk/cronin/
 * Copyright (C) 2012 Stefan Rado
 * Copyright (C) 2011 Steven Cogswell <steven.cogswell@gmail.com>
 *                    http://husks.wordpress.com
 *
 * Version 20151029
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CommandHandler_h
#define CommandHandler_h

#if defined(WIRING) && WIRING >= 100
  #include <Wiring.h>
#elif defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif
#include <string.h>

// Size of the input buffer in bytes (maximum length of one command plus arguments)
#define COMMANDHANDLER_BUFFER 64
// Maximum length of a command excluding the terminating null
#define COMMANDHANDLER_MAXCOMMANDLENGTH 8
// Default delimitor and terminator
#define COMMANDHANDLER_DEFAULT_DELIM ","
#define COMMANDHANDLER_DEFAULT_TERM ';'
// The null term for string
#define STRING_NULL_TERM '\0'

// Uncomment the next line to run the library in debug mode (verbose messages)
//#define COMMANDHANDLER_DEBUG

#ifdef COMMANDHANDLER_DEBUG
extern void *__brkval;
#endif


class CommandHandler {
  public:
    CommandHandler(const char *newdelim = COMMANDHANDLER_DEFAULT_DELIM, const char newterm = COMMANDHANDLER_DEFAULT_TERM);   // Constructor
    void addCommand(const char *command, void(*function)());  // Add a command to the processing dictionary.
    void addRelay(const char *command, void (*function)(const char *, void*), void* pt2Object = NULL);  // Add a command to the relay dictionary. Such relay are given the remaining of the command. pt2Object is the reference to the instance associated with the callback, it will be given as the second argument of the callback function, default is NULL
    void setDefaultHandler(void (*function)(const char *));   // A handler to call when no valid command received.
    void setDefaultHandler(void (*function)(const char *, void*), void* pt2Object);   // A handler to call when no valid command received.

    void setInCmdSerial(Stream &inStream); // define to which serial to send the read commands
    void processSerial();  // Process what on the in stream
    void processSerial(Stream &inStream);  // Process what on the designated stream
    void processString(const char *inString); // Process a String
    void processChar(char inChar); //Process a char
    void clearBuffer();   // Clears the input buffer.
    char *remaining();         // Returns pointer to remaining of the command buffer (for getting arguments to commands).
    char *next();         // Returns pointer to next token found in command buffer (for getting arguments to commands).

    // helpers to cast next into different types
    bool argOk; // this variable is set after the below function are run, it tell you if thing went well
    bool readBoolArg();
    int readIntArg();
    long readLongArg();
    float readFloatArg();
    double readDoubleArg();
    char *readStringArg();
    bool compareStringArg(const char *stringToCompare);

    //helpers to create a message
    void setCmdHeader(const char *cmdHeader, bool addDelim = true); // setting a char to be added at the start of each out message (default "")
    void initCmd(); // initialize the command buffer  to build next message to be sent

    void clearCmd(); // clear the output command
    void addCmdDelim();
    void addCmdTerm();

    void addCmdBool(bool value);
    void addCmdInt(int value);
    void addCmdLong(long value);

    void setCmdDecimal(byte decimal);
    void addCmdFloat(double value);
    void addCmdFloat(float value, byte decimal);
    void addCmdDouble(double value);
    void addCmdDouble(double value, byte decimal);

    void addCmdString(const char *value);

    char* getOutCmd(); // get pointer to command buffer

    void setOutCmdSerial(Stream &outStream); // define to which serial to send the out commands
    void sendCmdSerial(); //send current command thought the Stream
    void sendCmdSerial(Stream &outStream); //send current command thought the Stream

  private:

    // Command/handler dictionary
    struct CommandHandlerCallback {
      char command[COMMANDHANDLER_MAXCOMMANDLENGTH + 1];
      void (*function)();
    };                                    // Data structure to hold Command/Handler function key-value pairs
    CommandHandlerCallback *commandList;   // Actual definition for command/handler array
    byte commandCount;

    // Relay/handler dictionary
    struct RelayHandlerCallback {
      char command[COMMANDHANDLER_MAXCOMMANDLENGTH + 1];
      void* pt2Object;
      void (*function)(const char *, void*);
    };                                 // Data structure to hold Relay/Handler function key-value pairs
    RelayHandlerCallback *relayList;   // Actual definition for Relay/handler array
    byte relayCount;

    // Pointer to the default handler function
    void (*defaultHandler)(const char *);
    void* pt2defaultHandlerObject;
    void (*wrapper_defaultHandler)(const char *, void*);

    const char *delim; // null-terminated list of character to be used as delimeters for tokenizing (default " ")
    char term;     // Character that signals end of command (default '\n')

    char buffer[COMMANDHANDLER_BUFFER + 1]; // Buffer of stored characters while waiting for terminator character
    byte bufPos;                        // Current position in the buffer
    char *last;                         // State variable used by strtok_r during processing

    char remains[COMMANDHANDLER_BUFFER + 1]; // Buffer of stored characters to pass to a relay function

    String commandString; // Out Command
    String commandHeader; // header for out command
    byte commandDecimal;


    // in and out default strem
    Stream *inCmdStream;
    Stream *outCmdStream;
};

#endif //CommandHandler_h
