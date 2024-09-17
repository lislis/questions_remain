// ESP32 code

// this is only dealing with capacitive sensing and lighting up LEDs
// for SD card and audio see the Arduino sketch

// Serial comms with Arduino
// sending
// press-<index>
// release-<index>
// listening
// on-<index>
// off-<index>
// where <index> is the caps array index but in human readable form (+1)
#define RXp2 16
#define TXp2 17

#define touchThreshold 12
// caps[index][0] == capcitive pin
// caps[index][1] == led pin
// caps[index][2] == last led state
// caps[index][3] == last touch reading
int const ROWS = 8;
int const COLS = 4;
int caps[ROWS][COLS] = { {T0, 5, 0, 1}, {T3, 19, 0, 1}, {T4, 21, 0, 1}, {T5, 1, 0, 1}, {T6, 22, 0, 1}, {T7, 23, 0, 1}, {T8, 26, 0, 1}, {T9, 25, 0, 1} };

String inputString = "";      // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

void setup() {
  Serial.begin(115200);
  //Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);  

  Serial.println("ESP32 setting up");

  for (int i = 0; i < ROWS; i++) {
    pinMode(caps[i][1], OUTPUT);
    //digitalWrite(caps[i][1], HIGH);
    //delay(200);
    //digitalWrite(caps[i][1], LOW);
    //caps[i][2] = 0;
    //caps[i][3] = touchRead(caps[i][0]);
  }
  Serial.println("ESP32 done");

}
void loop() {
  //sensing
  for (int i = 0; i < ROWS; i++) {
    int reading = touchRead(caps[i][0]);

    if (reading != 0 && reading < caps[i][3] - touchThreshold) {
      Serial.print("cap ");
      Serial.print(i + 1); // human index counting
      Serial.println(" pressed");
      // Serial2.print("press-");
      // Serial2.println(i);
    }
    if (reading != 0 && reading > caps[i][3] + touchThreshold) { 
      Serial.print("cap ");
      Serial.print(i + 1); // human index counting
      Serial.println(" released");
      // Serial2.print("release-");
      // Serial2.println(i);
    }
    caps[i][3] = reading;
  }

  String command = "";
  int commandArg = 0;

  while (Serial.available()) {
    String s = Serial.readStringUntil('\n');
    Serial.println(inputString);

    int index = inputString.indexOf("-");
    if (index != -1) {
      command = inputString.substring(0, index);
      commandArg = inputString.substring(index + 1).toInt() -1; // for human index counting
    }
  }
  if (command == "on") {
    digitalWrite(caps[commandArg][1], HIGH);
    caps[commandArg][2] = 1;
  }
  if (command == "off") {
    digitalWrite(caps[commandArg][1], LOW);
    caps[commandArg][2] = 0;
  }
}
