#include <Adafruit_Microbit.h>

Adafruit_Microbit_Matrix microbit;

const int buttonA = 5;
const int buttonB = 11;

const uint8_t pattern1_bmp[] =
{
  B11111,
  B00000,
  B11111,
  B00000,
  B00000
};

int counter = 0;

char hexMap(int digit)
{
  char temp[2];

  sprintf(temp, "%X");

  //return temp[0];
  return 'X';
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("microbit has started");
  microbit.begin();

  pinMode(buttonA, INPUT);  
  pinMode(buttonB, INPUT);  

  microbit.print(counter);
}

void loop() {

  char temp;

  if(!digitalRead(buttonA) && counter < 9)
  {
    counter++;
    temp = hexMap(temp);
    microbit.println(temp);
    Serial.println(temp);
  }

  if(!digitalRead(buttonB) && counter > 0)
  {
    counter--;
    temp = hexMap(temp);
    microbit.println(temp);
    Serial.println(temp);
  }

  delay(200);
}