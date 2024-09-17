#include <SD.h>
#include <TMRpcm.h>
#include <SPI.h>


TMRpcm audio;
File root;

void setup(void) {
  Serial.begin(9600);
  delay(100);
 
  Serial.print("Initializing SD card...");
  if (!SD.begin()) {
    Serial.println("failed!");
    //error();
    while(true);  // stay here.
  }
  Serial.println("OK!");
 
  audio.speakerPin = 9;  // set speaker output to pin 9 
  audio.setVolume(5);    //   0 to 7. Set volume level
  audio.quality(1);      //  Set 1 for 2x oversampling Set 0 for normal
  root = SD.open("/");      // open SD card main root

  Serial.println("Setup done!");
}
 
// main loop
String inputString = "";
bool stringComplete = false;
String command = "";
int commandArg = 0;

void loop() {
  if (Serial.available()) {
    while(Serial.available()) {
      char inChar = (char)Serial.read();
      inputString += inChar;
      if (inChar == '\n') {
        stringComplete = true;
      }
    }
  }

  command = "";
  commandArg = 0;
  // if (!audio.isPlaying()) {
    
    if (stringComplete) {
      //Serial.println(inputString);
      int index = inputString.indexOf("-");
      if (index != -1) {
        command = inputString.substring(0, index);
        commandArg = inputString.substring(index + 1).toInt(); // for human index counting

        // Serial.print(command);
        // Serial.println(commandArg);
      }
      inputString = "";
      stringComplete = false;
    }
  // }

  if (command == "press") {
    audio.stopPlayback();
    delay(5);
    char buffer[6];
    sprintf(buffer, "%03d.wav", commandArg);
    Serial.print("Playing ");
    Serial.println(buffer);
    audio.play(buffer);
  }

  // if (command == "release") {
  //   Serial.print("receiv released");
  //   Serial.println(commandArg);
  //   audio.stopPlayback(); 
  // }
}
