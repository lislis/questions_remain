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

  if (!audio.isPlaying()) {
    String command = "";
    int commandArg = 0;
    if (stringComplete) {
      //Serial.println(inputString);
      int index = inputString.indexOf("-");
      if (index != -1) {
        command = inputString.substring(0, index);
        commandArg = inputString.substring(index + 1).toInt() -1; // for human index counting

        Serial.print(command);
        Serial.println(commandArg);
      }
      inputString = "";
      stringComplete = false;
    }

    //  if (command == "press") {
    //     audio.stopPlayback();
    //     delay(50);
    //     mcp.digitalWrite(poi[commandArg][0], HIGH);
    //     char buffer[6];
    //     sprintf(buffer, "%03d.wav", poi[commandArg][3]);
    //     Serial.print("Playing ");
    //     Serial.println(buffer);
    //     audio.play(buffer);
    //   }

    //   if (command == "release") {
    //     mcp.digitalWrite(poi[commandArg][0], LOW);
    //     audio.stopPlayback(); 
    //   }
  }
}
