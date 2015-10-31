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
// #define COMMANDHANDLER_DEBUG


class CommandHandler {
  public:
    CommandHandler(char *newdelim = COMMANDHANDLER_DEFAULT_DELIM, char newterm = COMMANDHANDLER_DEFAULT_TERM);   // Constructor
    void addCommand(const char *command, void(*function)());  // Add a command to the processing dictionary.
    void addRelay(const char *command, void (*function)(const char *));  // Add a command to the relay dictionary. Such relay are given the remaining of the command.
    void setDefaultHandler(void (*function)(const char *));   // A handler to call when no valid command received.

    void processSerial(Stream &comms);  // Process what on the stream
    void processString(const char *inString); // Process a String
    void processChar(char inChar); //Process a char
    void clearBuffer();   // Clears the input buffer.
    char *remaining();         // Returns pointer to remaining of the command buffer (for getting arguments to commands).
    char *next();         // Returns pointer to next token found in command buffer (for getting arguments to commands).

    // helpers to cast next into different types
    bool argOk; // this variable is set after the below function are run, it tell you if thing went well
    int16_t readInt16Arg();
    int32_t readInt32Arg();
    bool readBoolArg();
    float readFloatArg();
    double readDoubleArg();
    char *readStringArg();
    bool compareStringArg(const char *stringToCompare);

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
      void (*function)(const char *);
    };                                 // Data structure to hold Relay/Handler function key-value pairs
    RelayHandlerCallback *relayList;   // Actual definition for Relay/handler array
    byte relayCount;

    // Pointer to the default handler function
    void (*defaultHandler)(const char *);

    char *delim; // null-terminated list of character to be used as delimeters for tokenizing (default " ")
    char term;     // Character that signals end of command (default '\n')

    char buffer[COMMANDHANDLER_BUFFER + 1]; // Buffer of stored characters while waiting for terminator character
    byte bufPos;                        // Current position in the buffer
    char *last;                         // State variable used by strtok_r during processing
};

#endif //CommandHandler_h
