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
  poi[number][0] == status led pin mcp
  poi[number][1] == state of led
  poi[number][2] == last capacitive reading
  poi[number][3] == filename

  We're sending the filename over serial to another arduino

  poi = point of interest
*/

Adafruit_MCP23X17 mcp;

// Adjust ROWS and the poi array if you connect more pois
int const COLS = 4;
int const ROWS = 9;
// the leds are connected via mcp, the buttons are regular digital pins
// poi and poi_cap share the same index
int poi[ROWS][COLS] = {{7, 0, 0, 1}, {6, 0, 0, 2}, {5, 0, 0, 3}, {4, 0, 0, 4}, {3, 0, 0, 5}, {2, 0, 0, 6}, {1, 0, 0, 7}, {0, 0, 0, 8}, {8, 0, 0, 9}};

// to be adjusted
#define touchThreshold 500
// CapacitiveSensor(base, measurement);
// between the base pin and the respective others is a > 1M Ohm resistor
CapacitiveSensor poi_cap[ROWS] = {
  CapacitiveSensor(3, 2),
  CapacitiveSensor(3, 4),
  CapacitiveSensor(3, 5),
  CapacitiveSensor(7, 6),
  CapacitiveSensor(7, 8),
  CapacitiveSensor(7, 9),
  CapacitiveSensor(11, 10),
  CapacitiveSensor(11, 12),
  CapacitiveSensor(11, 13)
  };

// General status LEDs
int ready_led = 9; //mcp
int error_led = 10; //mcp


void setup(void) {
  Serial.begin(9600);

  mcp.pinMode(ready_led, OUTPUT);
  mcp.pinMode(error_led, OUTPUT);

  //Serial.print("waiting for mcp...");
  if (!mcp.begin_I2C()) {
    //Serial.println("failed!");
    error();
    while(1);
  }
  //Serial.println("OK!");

  // for (int i = 0; i < 16; i++) {
  //   mcp.pinMode(i, OUTPUT);
  //   mcp.digitalWrite(i, HIGH);
  // }

  for (int i = 0; i < ROWS; i++) {
    mcp.pinMode(poi[i][0], OUTPUT);
    mcp.digitalWrite(poi[i][0], HIGH);
    delay(200);
    mcp.digitalWrite(poi[i][0], LOW);
    poi[i][1] = 0;
    poi[i][2] = poi_cap[i].capacitiveSensor(30);
  }

  // mcp.digitalWrite(9, HIGH);
  // mcp.digitalWrite(10, HIGH);

  ready();
  //Serial.println("Setup done 1!");
}
 
// main loop
void loop() {
  for (int i = 0; i < ROWS; i++) {
    int reading = poi_cap[i].capacitiveSensor(30);
    //Serial.println(reading);

    if (reading != 0 && reading > poi[i][2] + touchThreshold && poi[i][1] == 0) {
      // Serial.print("poi ");
      // Serial.print(i + 1); // human index counting
      // Serial.println(" pressed");
      Serial.print("play-");
      Serial.println(poi[i][3]);
      mcp.digitalWrite(poi[i][0], HIGH);
      poi[i][1] = 1;
    }
    if (reading != 0 && reading < poi[i][2] - touchThreshold && poi[i][1] == 1) { 
      // Serial.print("poi ");
      // Serial.print(i + 1); // human index counting
      // Serial.println(" released");
      Serial.println("released");
      mcp.digitalWrite(poi[i][0], LOW);
      poi[i][1] = 0;
    }
    poi[i][2] = reading;
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