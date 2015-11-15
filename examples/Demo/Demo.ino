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

  Serial.begin(9600); // setting up the serial port to 9600 baud

  // Setup callbacks for SerialCommand commands
  cmdHdl.addCommand("HELLO", sayHello);        // Echos the string argument back
  cmdHdl.addCommand("FWD",   forwardRemaining);// Fwd the remaining of the command to the cmdHdl
  cmdHdl.addCommand("P",     processCommand);  // Converts two arguments, first to double and echos them back
  cmdHdl.addCommand("GUESS", guessMyName);     // A game for guessing my name, used to test compareStringArg
  cmdHdl.addCommand("PING", pongMesssage);     // A function that use the packet forging tool to send a random ping time
  cmdHdl.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")

  // You can process string directly into the arduino
  cmdHdl.processString("HELLO,I am ready!;");
  // The above command send give a string to the command parser
  // This string happens to be a, HELLO command
  // Thus the arduino will execute the sayHello function
  // The sayHello function can then access the argument of the command

  // MORE EXPLANATION ABOUT CALLBACK POSSIBILITIES IN THE loop()

  // You can also forge packet to send
  // This is useful to talk to another device having a command handler
  // For example talking to another arduino board
  // Or to you computer using the Python-CommandHandler module [add link once online]
  
  // First you can define a header for all you message, e.g. the name of the device
  cmdHdl.setCmdHeader("FEEDBACK"); // here we call it FEEDBACK, a delim will automatically be added after the header
  // use cmdHdl.setCmdHeader("FEEDBACK", false); if you do not want a delimiter

  // always start by initiating your message, it just set things up
  cmdHdl.initCmd(); 
  // now create the message you like
  cmdHdl.addCmdString("ALIVE"); // add a string
  cmdHdl.addCmdDelim(); // add a delim
  cmdHdl.addCmdBool(true); // add a boolean
  cmdHdl.addCmdDelim(); // add a delim
  cmdHdl.addCmdInt32(938); //add a int
  cmdHdl.addCmdDelim(); // add a delim
  cmdHdl.addCmdDouble(-2147.483647, 3); // add a double, printed with 3 decimal
  // if unspecified decimal, default is 2)
  // you can change the default decimal to N using cmdHdl.setCmdDecimal(N)
  cmdHdl.addCmdTerm(); //finally end your message with the term char

  // once the message is ready, send it
  // either by getting it and sending it however you want
  Serial.println(cmdHdl.getOutCmd()); // that will print the message on Serial
  // or by using the Serial using the directly embeded Serial send
  cmdHdl.sendCmdSerial(); // also send current Cmd to Serial (by default)
  // you can also set the default Out Serial to use by:
  // cmdHdl.setOutCmdSerial(Serial); // default is Serial
  Serial.println(); // just adding this so the ouput on the serial terminal looks nice
}

void loop() {
  //Of course we can use the same functionnality by readng from the Serial
  cmdHdl.processSerial(Serial);
  // Try send "HELLO,yourname;" to your board via serial
  // you can also set the default In Serial to use by:
  // cmdHdl.setInCmdSerial(Serial); // default is Serial
  // cmdHdl.processSerial();

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
  // PING;
  // GUESS,Maurice;
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

void pongMesssage() {

  Serial.println("Received PING, pausing for a random time..."); // for the demo only!
  
  unsigned long start = millis();
  delay(random(1000));
  unsigned long elasped = millis() - start;
  
  cmdHdl.initCmd();
  cmdHdl.addCmdString("PONG");
  cmdHdl.addCmdDelim();
  cmdHdl.addCmdInt32(elasped);
  cmdHdl.addCmdTerm();
  cmdHdl.sendCmdSerial();

  Serial.println(); // for the demo only! so the output look nice
  Serial.println("Above is the feedback command indicating the pause time.");
}

// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.println("What?");
}
