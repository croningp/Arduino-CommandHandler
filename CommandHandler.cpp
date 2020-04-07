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

#include "CommandHandler.h"

/**
 * Constructor allowing to change default delim and term
 * Example: SerialCommand sCmd(" ", ';');
 * Default are COMMANDHANDLER_DEFAULT_DELIM and COMMANDHANDLER_DEFAULT_TERM
 */
CommandHandler::CommandHandler(const char *newdelim, char newterm)
  : commandList(NULL),
    commandCount(0),
    relayList(NULL),
    relayCount(0),
    defaultHandler(NULL),
    pt2defaultHandlerObject(NULL),
    wrapper_defaultHandler(NULL),
    term(newterm),           // asssign new terminator for commands
    last(NULL),
    delim(newdelim) // assign new delimitor
{
  inCmdStream = &Serial;
  outCmdStream = &Serial;

  commandHeader = String("");
  commandDecimal = 2;

  clearBuffer();
}

/**
 * Adds a "command" and a handler function to the list of available commands.
 * This is used for matching a found token in the buffer, and gives the pointer
 * to the handler function to deal with it.
 */
void CommandHandler::addCommand(const char *command, void (*function)()) {
  #ifdef COMMANDHANDLER_DEBUG
    Serial.print("Adding command (");
    Serial.print(commandCount);
    Serial.print("): ");
    Serial.println(command);
  #endif

  commandList = (CommandHandlerCallback *) realloc(commandList, (commandCount + 1) * sizeof(CommandHandlerCallback));
  strncpy(commandList[commandCount].command, command, COMMANDHANDLER_MAXCOMMANDLENGTH);
  commandList[commandCount].function = function;
  commandCount++;
}

/**
 * Adds a "command" and a handler function to the list of available relay.
 * This is used for matching a found token in the buffer, and gives the pointer
 * to the handler function to deal with the remaining of the command
 */
void CommandHandler::addRelay(const char *command, void (*function)(const char *, void*), void* pt2Object) {
  #ifdef COMMANDHANDLER_DEBUG
    Serial.print("Adding relay (");
    Serial.print(relayCount);
    Serial.print("): ");
    Serial.println(command);
  #endif

  relayList = (RelayHandlerCallback *) realloc(relayList, (relayCount + 1) * sizeof(RelayHandlerCallback));
  strncpy(relayList[relayCount].command, command, COMMANDHANDLER_MAXCOMMANDLENGTH);
  relayList[relayCount].pt2Object = pt2Object;
  relayList[relayCount].function = function;
  relayCount++;
}

/**
 * This sets up a handler to be called in the event that the receveived command string
 * isn't in the list of commands.
 */
void CommandHandler::setDefaultHandler(void (*function)(const char *)) {
  defaultHandler = function;
}

void CommandHandler::setDefaultHandler(void (*function)(const char *, void*), void* pt2Object) {
  pt2defaultHandlerObject = pt2Object;
  wrapper_defaultHandler = function;
}

/**
 * Assign the default serial
 */
void CommandHandler::setInCmdSerial(Stream &inStream) {
  inCmdStream = &inStream;
}

/**
 * Check the default Serial
 */
void CommandHandler::processSerial() {
  processSerial(*inCmdStream);
}

/**
 * This checks the Serial stream for characters, and assembles them into a buffer.
 * When the terminator character (default COMMANDHANDLER_DEFAULT_TERM) is seen, it starts parsing the
 * buffer for a prefix command, and calls handlers setup by addCommand() member
 */
void CommandHandler::processSerial(Stream &inStream) {
  while (inStream.available() > 0) {
    char inChar = inStream.read();   // Read single available character, there may be more waiting
    #ifdef COMMANDHANDLER_DEBUG
      Serial.print("Serial: ");
      Serial.println(inChar);   // Echo back to serial stream
    #endif
    processChar(inChar);
  }
}

/**
 * This iterate on a String char by char, and push them into a buffer.
 * When the terminator character (default COMMANDHANDLER_DEFAULT_TERM) is seen, it starts parsing the
 * buffer for a prefix command, and calls handlers setup by addCommand() member
 */
void CommandHandler::processString(const char *inString) {
  for (int i = 0; i < strlen(inString); i++){
    char inChar = inString[i];
    #ifdef COMMANDHANDLER_DEBUG
      Serial.print("String: ");
      Serial.println(inChar);   // Echo back to serial stream
    #endif
    processChar(inChar);
  }
}

/**
 * This add a characters to the buffer, and analyse the buffer.
 * When the terminator character (default COMMANDHANDLER_DEFAULT_TERM) is seen, it starts parsing the
 * buffer for a prefix command, and calls handlers setup by addCommand() member
 */
void CommandHandler::processChar(char inChar) {
  if (inChar == term) {     // Check for the terminator (default '\r') meaning end of command
    #ifdef COMMANDHANDLER_DEBUG
      Serial.print("Received: ");
      Serial.println(buffer);
    #endif

    char *command = strtok_r(buffer, delim, &last);   // Search for command at start of buffer
    if (command != NULL) {
      boolean matched = false;
      // searching in commands
      for (int i = 0; i < commandCount; i++) {
        #ifdef COMMANDHANDLER_DEBUG
          Serial.print("Comparing [");
          Serial.print(command);
          Serial.print("] to [");
          Serial.print(commandList[i].command);
          Serial.println("]");
        #endif

        // Compare the found command against the list of known commands for a match
        if (strncmp(command, commandList[i].command, COMMANDHANDLER_MAXCOMMANDLENGTH) == 0) {
          #ifdef COMMANDHANDLER_DEBUG
            Serial.print("Matched Command: ");
            Serial.println(command);
          #endif

          // Execute the stored handler function for the command
          (*commandList[i].function)();

          #ifdef COMMANDHANDLER_DEBUG
            int free_mem = (int) AVR_STACK_POINTER_REG - (int) __brkval;
            Serial.print("Free SRAM: ");
            Serial.println(free_mem);
          #endif

          matched = true;
          break;
        }
      }
      // searching in relays
      for (int i = 0; i < relayCount; i++) {
        #ifdef COMMANDHANDLER_DEBUG
          Serial.print("Comparing [");
          Serial.print(command);
          Serial.print("] to [");
          Serial.print(relayList[i].command);
          Serial.println("]");
        #endif

        // Compare the found command against the relay list of known commands for a match
        if (strncmp(command, relayList[i].command, COMMANDHANDLER_MAXCOMMANDLENGTH) == 0) {
          #ifdef COMMANDHANDLER_DEBUG
            Serial.print("Matched Relay: ");
            Serial.println(command);
          #endif

          // Execute the stored handler function for the command
          (*relayList[i].function)(remaining(), relayList[i].pt2Object);

          #ifdef COMMANDHANDLER_DEBUG
            int free_mem = (int) AVR_STACK_POINTER_REG - (int) __brkval;
            Serial.print("Free SRAM: ");
            Serial.println(free_mem);
          #endif
          matched = true;
          break;
        }
      }
      if (!matched){
        if (defaultHandler != NULL) {
          (*defaultHandler)(command);
        } else if (pt2defaultHandlerObject != NULL) {
          (*wrapper_defaultHandler)(command, pt2defaultHandlerObject);
        }
      }
    }
    clearBuffer();
  }
  else if (isprint(inChar)) {     // Only printable characters into the buffer
    if (bufPos < COMMANDHANDLER_BUFFER) {
      buffer[bufPos] = inChar;  // Put character into buffer
      buffer[bufPos+1] = STRING_NULL_TERM;      // Null terminate
      bufPos++;
      #ifdef COMMANDHANDLER_DEBUG
      Serial.print("Current buffer: ");
      Serial.println(buffer);
      #endif
    } else {
      #ifdef COMMANDHANDLER_DEBUG
        Serial.println("Line buffer is full - increase COMMANDHANDLER_BUFFER");
      #endif
    }
  }
}

/*
 * Clear the input buffer.
 */
void CommandHandler::clearBuffer() {
  buffer[0] = STRING_NULL_TERM;
  bufPos = 0;
}

/**
 * Retrieve the next token ("word" or "argument") from the command buffer.
 * Returns NULL if no more tokens exist.
 */
char *CommandHandler::next() {
  return strtok_r(NULL, delim, &last);
}

/**
 * Returns char* of the remaining of the command buffer (for getting arguments to commands).
 * Returns NULL if no more tokens exist.
 */
char *CommandHandler::remaining() {

  //reinit the remains char
  remains[0] = STRING_NULL_TERM;

  char str_term[2];
  str_term[0] = term;
  str_term[1] = STRING_NULL_TERM;

   // Search for the remaining up to next term
  char *command = strtok_r(NULL, str_term, &last);

  // forge term in string format
  strcpy(remains, command);
  strcat(remains, str_term);

  // clear the buffer now, we emptied the current command
  // the remaining is might be given to another handler
  // or it might used by the same commandHandler instance
  // hence the buffer should be emptied now
  clearBuffer();

  return remains;
}



/*****************************************
 * Helpers to read args and cast them into specific type, strongly inspired by CmdMessenger
 *****************************************/

/**
 * Read the next argument as int16
 */
int CommandHandler::readIntArg() {
  char *arg;
  arg = next();
  if (arg != NULL) {
    argOk = true;
    return atoi(arg);
  }
  argOk = false;
  return 0;
}

/**
 * Read the next argument as int32
 */
long CommandHandler::readLongArg() {
  char *arg;
  arg = next();
  if (arg != NULL) {
    argOk = true;
    return atol(arg);
  }
  argOk = false;
  return 0L; // 'L' to force the constant into a long data format
}

/**
 * Read the next argument as bool
 */
bool CommandHandler::readBoolArg() {
  return (readIntArg() != 0) ? true : false;
}

/**
 * Read the next argument as float
 */
float CommandHandler::readFloatArg() {
  char *arg;
  arg = next();
  if (arg != NULL) {
    argOk = true;
    return strtod(arg, NULL);
  }
  argOk = false;
  return 0;
}

/**
 * Read the next argument as double
 */
double CommandHandler::readDoubleArg() {
  char *arg;
  arg = next();
  if (arg != NULL) {
    argOk = true;
    return strtod(arg, NULL);
  }
  argOk = false;
  return 0;
}

/**
 * Read next argument as string.
 */
char* CommandHandler::readStringArg() {
  char *arg;
  arg = next();
  if (arg != NULL) {
    argOk = true;
    return arg;
  }
  argOk = false;
  return STRING_NULL_TERM;
}

/**
 * Compare the next argument with a string
 */
bool CommandHandler::compareStringArg(const char *stringToCompare) {
  char *arg;
  arg = next();
  if (arg != NULL) {
    return (strcmp(stringToCompare, arg) == 0) ? true : false;
  }
  return false;
}

/*****************************************
 * Forging and sending output commands
 *****************************************/


/**
 * Set an header for the output command
 */
void CommandHandler::setCmdHeader(const char *cmdHeader, bool addDelim) {

  commandHeader = String(cmdHeader);

  if (addDelim == true) {
    commandHeader = commandHeader + String(delim);
  }

  #ifdef COMMANDHANDLER_DEBUG
    Serial.print("Out Command Header is now ");
    Serial.println(commandHeader);
  #endif
}

void CommandHandler::initCmd() {

  commandString = commandHeader;

  #ifdef COMMANDHANDLER_DEBUG
    Serial.print("Out command is now ");
    Serial.println(commandString);
  #endif
}

void CommandHandler::addCmdDelim() {

  commandString = commandString + String(delim);

  #ifdef COMMANDHANDLER_DEBUG
    Serial.print("Out command is now ");
    Serial.println(commandString);
  #endif
}
//
void CommandHandler::addCmdTerm() {

  commandString = commandString + String(term);

  #ifdef COMMANDHANDLER_DEBUG
    Serial.print("Out command is now ");
    Serial.println(commandString);
  #endif
}

void CommandHandler::addCmdBool(bool value) {

  commandString = commandString + String(value);

  #ifdef COMMANDHANDLER_DEBUG
    Serial.print("Out command is now ");
    Serial.println(commandString);
  #endif
}

void CommandHandler::addCmdInt(int value) {

  commandString = commandString + String(value, DEC);

  #ifdef COMMANDHANDLER_DEBUG
    Serial.print("Out command is now ");
    Serial.println(commandString);
  #endif
}

void CommandHandler::addCmdLong(long value) {

  commandString = commandString + String(value, DEC);

  #ifdef COMMANDHANDLER_DEBUG
    Serial.print("Out command is now ");
    Serial.println(commandString);
  #endif
}


void CommandHandler::setCmdDecimal(byte decimal) {
  commandDecimal = decimal;
}

void CommandHandler::addCmdFloat(double value) {
  addCmdFloat(value, commandDecimal);
}

void CommandHandler::addCmdFloat(float value, byte decimal) {


  commandString = commandString + String(value, decimal);

  #ifdef COMMANDHANDLER_DEBUG
    Serial.print("Out command is now ");
    Serial.println(commandString);
  #endif
}

void CommandHandler::addCmdDouble(double value) {
  addCmdDouble(value, commandDecimal);
}

void CommandHandler::addCmdDouble(double value, byte decimal) {

  commandString = commandString + String(value, decimal);

  #ifdef COMMANDHANDLER_DEBUG
    Serial.print("Out command is now ");
    Serial.println(commandString);
  #endif
}

void CommandHandler::addCmdString(const char *value) {

  commandString = commandString + String(value);

  #ifdef COMMANDHANDLER_DEBUG
    Serial.print("Out command is now ");
    Serial.println(commandString);
  #endif

}

char* CommandHandler::getOutCmd() {

  char command[COMMANDHANDLER_BUFFER + 1];
  commandString.toCharArray(command, COMMANDHANDLER_BUFFER + 1);
  return command;
}

void CommandHandler::setOutCmdSerial(Stream &outStream) {
  outCmdStream = &outStream;
}

void CommandHandler::sendCmdSerial() {
  sendCmdSerial(*outCmdStream);
}

void CommandHandler::sendCmdSerial(Stream &outStream) {
  outStream.print(commandString);
}
