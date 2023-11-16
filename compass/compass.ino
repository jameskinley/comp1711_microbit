#include <Adafruit_Microbit.h>
#include <LSM303AGR_MAG_Sensor.h>
#include <LSM303AGR_ACC_Sensor.h>
#define PI  3.14159;

LSM303AGR_MAG_Sensor Mag(&Wire1);
Adafruit_Microbit_Matrix microbit;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("loaded compass");

  Wire1.begin();

  Mag.begin();
  Mag.Enable();

  microbit.begin();

  pinMode(13, INPUT);
}

float getHeading(int x, int y, int z)
{
  float heading = (atan2(y, x) * 180) / PI;

  heading += 90;

  if(heading < 0)
  {
    heading += 360;
  }

  return heading;
}

void displayDirection(float heading)
{
  if(heading < 45 || heading > 315)
  {
    microbit.print("N");
  }
  else if(heading < 135)
  {
    microbit.print("E");
  }
  else if(heading < 205)
  {
    microbit.print("S");
  }
  else
  {
    microbit.print("W");
  }
}

void loop() {
  int32_t magnetometer[3];
  Mag.GetAxes(magnetometer);

  char coords[4];
  //sprintf(coords, "X: %d Y: %d Z: %d", magnetometer[0], magnetometer[1], magnetometer[2]);

  //Serial.println(coords);

  float heading = getHeading(magnetometer[0], magnetometer[1], magnetometer[2]);

  Serial.println(heading);
  displayDirection(heading);

  delay(1000);
}

