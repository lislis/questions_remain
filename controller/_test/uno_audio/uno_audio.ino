#include <SPI.h>
#include <SD.h>
#include <TMRpcm.h>

#define SD_ChipSelectPin 10

/*
  we're receiving commandos over serial

  play-<number>
  --> play this track
  released
  --> stop playing

  poi = point of interest
*/

TMRpcm audio;
File root;

String inputString = "";
bool stringComplete = false;


void setup(void) {
  Serial.begin(9600);

  delay(500);
  pinMode(13, OUTPUT);
 
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_ChipSelectPin)) {
    Serial.println("failed!");
    return;
    while(true);  // stay here.
  } else {
    Serial.println("OK!");
  }
  
 
  audio.speakerPin = 9;  // set speaker output to pin 9 
  audio.setVolume(5);    //   0 to 7. Set volume level
  audio.quality(1);      //  Set 1 for 2x oversampling Set 0 for normal
  root = SD.open("/");      // open SD card main root

  Serial.println("Setup done!");
  digitalWrite(13, HIGH);
}
 
// main loop
void loop() {
  String command = "";
  int commandArg = 0;

  if (stringComplete) {
    //Serial.println(inputString);
    if (inputString == "released") {
      command = "released";
    } else {
      int index = inputString.indexOf("-");
      if (index != -1) {
        command = inputString.substring(0, index);
        commandArg = inputString.substring(index + 1).toInt();
      }
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

  if (!audio.isPlaying()) {
    if (command == "play") {
      audio.stopPlayback();
      delay(50);
      char buffer[6];
      sprintf(buffer, "%03d.wav", commandArg);
      Serial.print("Playing ");
      Serial.println(buffer);
      audio.play(buffer);
    }
    if (command == "released") {
      audio.stopPlayback(); 
      Serial.print("stopping");
      delay(50);
    }
  } else {
    Serial.println("playing");
  }
}

void serialEvent() {
  while(Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}
