
#include <CommandHandler.h>

#include <AccelStepper.h>
#include <CommandAccelStepper.h>


CommandHandler cmdHdl;

CommandAccelStepper cmdStp(AccelStepper::DRIVER, 9, 10);
CommandAccelStepper cmdStp2(AccelStepper::DRIVER, 50, 51);


void setup()
{   
  Serial.begin(9600);
   
  cmdStp.init();
  cmdStp2.init();

  // Setup callbacks for CommandHandler commands
  cmdHdl.addRelay("M1", cmdStp.wrapper_handleCommand, &cmdStp);
  cmdHdl.addRelay("M2", cmdStp2.wrapper_handleCommand, &cmdStp2);
  
  cmdHdl.setDefaultHandler(unrecognized);      // Handler for command that isn't matched  (says "What?")
}

void loop()
{  
  cmdHdl.processSerial(Serial);
  
  cmdStp.update();
  cmdStp2.update();
}

// This gets set as the default handler, and gets called when no other command matches.
void unrecognized(const char *command) {
  Serial.println("What?");
}

