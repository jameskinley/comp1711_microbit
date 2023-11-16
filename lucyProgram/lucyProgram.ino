#include <Adafruit_Microbit.h>

Adafruit_Microbit_Matrix microbit;

const uint8_t
  cross_bmp[] =
  { B01110,
    B11111,
    B11111,
    B01110,
    B00100, };

void setup() {
  // put your setup code here, to run once:
  microbit.begin();
  microbit.show(cross_bmp);
}

void loop() {
  // put your main code here, to run repeatedly:

}
