#include <SD.h>
#include <TMRpcm.h>
#include <CapacitiveSensor.h>

const int FILE_LENGTH = 3;

// between the base pin and the respective others is a > 1M Ohm resistor
#define basePin 4
#define pin1 2
#define pin2 6
#define pin3 8
#define touchThreshold 500
// files[index][0] = pin
// files[index][1] = filename
// files[index][2] = last button state
int files[FILE_LENGTH][3] = {{pin1, 1, 0}, {pin2, 2, 0}, {pin3, 1, 0}};

TMRpcm audio;
File root;

CapacitiveSensor sensors[FILE_LENGTH] = {CapacitiveSensor(basePin, pin1), CapacitiveSensor(basePin, pin2), CapacitiveSensor(basePin, pin3)};

void setup(void) {
  Serial.begin(9600);
 
  Serial.print("Initializing SD card...");
  if (!SD.begin()) {
    Serial.println("failed!");
    while(true);  // stay here.
  }
  Serial.println("OK!");
 
  audio.speakerPin = 9;  // set speaker output to pin 9
 
  root = SD.open("/");      // open SD card main root
 
  audio.setVolume(5);    //   0 to 7. Set volume level
  audio.quality(1);      //  Set 1 for 2x oversampling Set 0 for normal

  Serial.println("Setup done!");
}
 
// main loop
void loop() {
  if (!audio.isPlaying()) {
    for (int i = 0; i < FILE_LENGTH; i++) {
      //int reading = digitalRead(files[i][0]);
      int reading = sensors[i].capacitiveSensor(30);

      if (reading != 0 && reading > files[i][2] + touchThreshold) {
        Serial.print("button ");
        Serial.print(i + 1); // human index counting
        Serial.println(" pressed");
        files[i][2] = reading;
        audio.stopPlayback();
        delay(100);
        char buffer[6];
        sprintf(buffer, "%03d.wav", files[i][1]);
        Serial.print("Playing ");
        Serial.println(buffer);
        audio.play(buffer);
      }
      if (reading != 0 && reading < files[i][2] - touchThreshold) { 
        Serial.print("button ");
        Serial.print(i + 1); // human index counting
        Serial.println(" released");
        files[i][2] = 1;
      }
    }
    delay(20);
  }
}