// Demo Code for CommandHandler Library
// Jonathan Grizou
// October 2015

#include <CommandHandler.h>

//Create a CommandHandler Instance
CommandHandler cmdHdl;

// If needed you can change the delimitor string and terminator char
// default are delim="," and term=';'
//CommandHandler cmdHdl(" ", '\n');


void setup() {

  Serial.begin(9600);

  // Setup callbacks for SerialCommand commands
  cmdHdl.addCommand("HELLO", sayHello);        // Echos the string argument back
  cmdHdl.addCommand("FWD",   forwardRemaining);// Fwd the remaining of the command to the cmdHdl
  cmdHdl.addCommand("P",     processCommand);  // Converts two arguments, first to double and echos them back
  cmdHdl.addCommand("GUESS", guessMyName);     // A game for guessing my name, used to test compareStringArg
  cmdHdl.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")

  // You can process string directly into the arduino
  cmdHdl.processString("HELLO,I am ready!;");
  // The above command send give a string to the command parser
  // This string happens to be a, HELLO command
  // Thus the arduino will execute the sayHello function
  // The sayHello function can then access the argument of the command
}

void loop() {
  //Of course we can use the same functionnality by readng from the Serial
  cmdHdl.processSerial(Serial);
  // Try send "HELLO,yourname;" to your board via serial

  // The above processSerial function is simply doing the following
  //if (Serial.available() > 0){
  //  cmdHdl.processChar(Serial.read());
  //}
  // Thus, depending on your needs, you can also use the command handler
  // by feeding it char by char using the processChar

  // Why having three different process function?
  // In some cases you might have to parse a string, for example when nesting command handlers
  // Imagine your main program receiving "M1,P,2000;" command throught Serial
  // The main program could redirect the "P,2000;" string command to the a device called "M1"
  // The M1 device can then use its own commandHandler to handle the sub command "P,2000;"
  // To extract the remaining of a command, you can use the function remaining()
  // This is illustrated in the forwardRemaining() function
  // that is associated to the FWD command in this example
  // Send "FWD,P,2000;" to the board, it should forward the "P,2000;" to itself
  // (thanks to the processString() function)
  // As "P" is assocviated with the processCommand() function, it will run it

  // Below are a bunch of serial command you can try sending
  // Try predicting what it will do
  // P,3.5;
  // FWD,P,3.5;
  // FWD,FWD,FWD,P,3.5;
}

void sayHello() {
  char *arg;
  arg = cmdHdl.next();    // Get the next argument from the SerialCommand object buffer
  if (arg != NULL) {    // As long as it existed, take it
    Serial.print("Hello ");
    Serial.println(arg);
  }
  else {
    Serial.println("Hello, whoever you are");
  }
}

void forwardRemaining() {
  char *remaining;
  remaining = cmdHdl.remaining();    // Get the next argument from the SerialCommand object buffer
  if (remaining != NULL) {    // As long as it existed, take it
    Serial.print("Forwarding ");
    Serial.println(remaining);
    cmdHdl.processString(remaining);
  } else {
    Serial.print("Nothing to forward");
  }
}

void processCommand() {
  Serial.println("We're in processCommand");

  double aNumber;

  aNumber = cmdHdl.readDoubleArg();
  if (cmdHdl.argOk) {
    Serial.print("First argument was: ");
    Serial.print(aNumber, 10);
    Serial.println(" (converted to double, printed with 10 decimals)");
  }
  else {
    Serial.println("No arguments");
  }

  char *aString;
  aString = cmdHdl.readStringArg();
  if (cmdHdl.argOk) {
    Serial.print("Second argument was: ");
    Serial.println(aString);
  }
  else {
    Serial.println("No second argument");
  }
}

void guessMyName() {
  if (cmdHdl.compareStringArg("Jonathan")) {
    Serial.println("Yes! My name is Jonathan");
  }
  else {
    Serial.println("Nop, that's not my name, try again..");
  }
}

// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.println("What?");
}
