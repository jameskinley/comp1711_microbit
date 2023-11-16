#include <Adafruit_Microbit.h>
#include <LSM303AGR_ACC_Sensor.h>
#include <math.h>
#include "C:\Users\james\Documents\CS_Uni\comp1711_microbit\libraries\MicrobitV2-HHS\src\Tone.h"

#define DEV_I2C Wire1  //Define which I2C bus is used. Wire1 for the Microbit V2. Wire is used for the Microbit V1

#define NOTE_CS5 554
#define NOTE_E5 659


Adafruit_Microbit_Matrix microbit;

const int buttonA = 5;   // the number of the pushbutton pin
const int buttonB = 11;  // the number of the pushbutton pin

typedef struct Point {
  int X, Y, Z;
} Point;

Point origin;

int alarm[] = {
  NOTE_E5, 8, NOTE_CS5
};

short armed = 1;

// Components.
LSM303AGR_ACC_Sensor Acc(&DEV_I2C);

float getEucDistance(Point point1, Point point2) {
  return sqrt(pow(point1.X - point2.X, 2) + pow(point1.Y - point2.Y, 2) + pow(point1.Z - point2.Z, 2));
}

short detectMotion(Point currentPos) {
  float eDistance = getEucDistance(origin, currentPos);

  Serial.println(eDistance);

  if (eDistance > 2000) {
    return 1;
  }
  return 0;
}

void warning();

Point getPos() {
  Point currentPos;
  int32_t acc_temp[3];
  Acc.GetAxes(acc_temp);

  origin.X = acc_temp[0];
  origin.Y = acc_temp[1];
  origin.Z = acc_temp[2];
}

void setup() {
  // Initialize serial for output.
  Serial.begin(9600);

  // Initialize I2C bus.
  DEV_I2C.begin();

  // Initlialize components.
  Acc.begin();
  Acc.Enable();

  microbit.begin();

  int32_t acc_temp[3];
  Acc.GetAxes(acc_temp);

  origin.X = acc_temp[0];
  origin.Y = acc_temp[1];
  origin.Z = acc_temp[2];

  pinMode(27, OUTPUT);
  pinMode(buttonA, INPUT);
  pinMode(buttonB, INPUT);
}

void loop() {
  if (!digitalRead(buttonA) && armed == 0) {
    Serial.println("Alarm armed.");
    origin = getPos();
    armed = 1;
    delay(1000);
  } else if (!digitalRead(buttonB) && armed == 1) {
    Serial.println("Alarm disarmed.");
    armed = 0;
    delay(1000);
  } else {
    if (detectMotion(getPos()) == 1) {
      warning();
      delay(1000);
    }
  }
}

void warning() {
  short disarmButtonPressed = 0;

  while (disarmButtonPressed == 0) {
    if (!digitalRead(buttonB)) {
      Serial.println("\nAlarm disabled");
      disarmButtonPressed = 1;
      armed = 0;
    } else {
      //tone(27, alarm[0], alarm[1] * 0.9);
      microbit.fillScreen(1);

      delay(alarm[1] / 10);

      //tone((uint8_t)27, alarm[2], alarm[3] * 0.9);
      Serial.println("\nALARM!");
      //noTone(27);

      microbit.clear();

      delay(100);
    }
  }
}
