#include <SD.h>
#include <TMRpcm.h>
#include <CapacitiveSensor.h>
#include <Adafruit_MCP23X17.h>

/*
  Lookup the pinout for the MCP23017
  | MCP | Arduino |
  |-----|---------|
  | GND | GND |
  | VCC | 5V |
  | SCK | A5 |
  | SDA | A4 |
  | A0 | GND |
  | A1 | GND |
  | A2 | GND |
  | RESET | 5V |

  Then you can use the MCP's pins (GPA is range 0 to 7, GPB is range 8 to 15) 

  This is stored in a 2D array so that:
  poi[number][0] == status led
  poi[number][1] == last state of button and led, we'll treat this as a bool
  poi[number][2] == last capacitive reading
  poi[number][3] == filename

  We're dealing with the capacitive buttons differently

  poi_cap[number][0] == capacitive btn pin

  poi = point of interest
*/

Adafruit_MCP23X17 mcp;

// Adjust ROWS and the poi array if you connect more pois
int const COLS = 4;
int const ROWS = 9;
// the leds are connected via mcp, the buttons are regular digital pins
// poi and poi_cap share the same index
int poi[ROWS][COLS] = {{7, 0, 0, 1}, {6, 0, 0, 2}, {5, 0, 0, 3}, {4, 0, 0, 4}, {3, 0, 0, 5}, {2, 0, 0, 6}, {1, 0, 0, 7}, {0, 0, 0, 8}, {13, 0, 0, 9}};

// to be adjusted
#define touchThreshold 500
// CapacitiveSensor(base, measurement);
// between the base pin and the respective others is a > 1M Ohm resistor
CapacitiveSensor poi_cap[ROWS] = {
  CapacitiveSensor(3, 2),
  CapacitiveSensor(3, 4),
  CapacitiveSensor(3, 5),
  CapacitiveSensor(6, 7),
  CapacitiveSensor(6, 8),
  CapacitiveSensor(11, 9),
  CapacitiveSensor(11, 10),
  CapacitiveSensor(11, 12),
  CapacitiveSensor(11, 13)
  };

TMRpcm audio;
File root;

// General status LEDs
int ready_led = 14; //mcp
int error_led = 15; //mcp


void setup(void) {
  Serial.begin(9600);

  mcp.pinMode(ready_led, OUTPUT);
  mcp.pinMode(error, OUTPUT);

  Serial.print("waiting for mcp...");
  if (!mcp.begin_I2C()) {
    Serial.println("failed!");
    error();
    while(1);
  }
  Serial.println("OK!");
 
  Serial.print("Initializing SD card...");
  if (!SD.begin()) {
    Serial.println("failed!");
    error();
    while(true);  // stay here.
  }
  Serial.println("OK!");
 
  audio.speakerPin = 9;  // set speaker output to pin 9 
  audio.setVolume(5);    //   0 to 7. Set volume level
  audio.quality(1);      //  Set 1 for 2x oversampling Set 0 for normal
  root = SD.open("/");      // open SD card main root

  for (int i = 0; i < ROWS; i++) {
    mcp.pinMode(poi[i][0], OUTPUT);
    mcp.digitalWrite(poi[i][0], LOW);
    poi[i][1] = 0;
    poi[i][2] = poi_cap[i].capacitiveSensor(30);
  }

  ready();
  Serial.println("Setup done!");
}
 
// main loop
void loop() {
  if (!audio.isPlaying()) {
    for (int i = 0; i < ROWS; i++) {
      //int reading = digitalRead(files[i][0]);
      int reading = poi_cap[i].capacitiveSensor(30);

      if (reading != 0 && reading > poi[i][2] + touchThreshold) {
        Serial.print("poi ");
        Serial.print(i + 1); // human index counting
        Serial.println(" pressed");
        poi[i][2] = reading;
        audio.stopPlayback();
        delay(100);
        char buffer[6];
        sprintf(buffer, "%03d.wav", poi[i][3]);
        Serial.print("Playing ");
        Serial.println(buffer);
        audio.play(buffer);
      }
      if (reading != 0 && reading < poi[i][2] - touchThreshold) { 
        Serial.print("poi ");
        Serial.print(i + 1); // human index counting
        Serial.println(" released");
        poi[i][2] = 1;
      }
    }
    delay(10);
  }
}


void error() {
  mcp.digitalWrite(ready_led, LOW);
  mcp.digitalWrite(error_led, HIGH);
}
void ready() {
  mcp.digitalWrite(ready_led, HIGH);
  mcp.digitalWrite(error_led, LOW);
}