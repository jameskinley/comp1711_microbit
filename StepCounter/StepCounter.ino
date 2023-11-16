#include <Adafruit_Microbit.h>
#include <LSM303AGR_ACC_Sensor.h>
#include <math.h>

#define DEV_I2C Wire1
#define THRESHOLD 0.72
#define MIN_SECS 0.03
#define MAX_MILLISECONDS 1000

const int buttonA = 5;     // the number of the pushbutton pin
const int buttonB = 11;     // the number of the pushbutton pin

int steps = 0;

const uint8_t
  cross_bmp[] =
  { B10001,
    B01010,
    B00100,
    B01010,
    B10001, };

Adafruit_Microbit_Matrix microbit;

// Components.
LSM303AGR_ACC_Sensor Acc(&DEV_I2C);

typedef struct Point {
  int X, Y, Z;
} Point;

Point getPos()
{
  int32_t acc_temp[3];
  Acc.GetAxes(acc_temp);

  Point pos;
  pos.X = acc_temp[0];
  pos.Y = acc_temp[1];
  pos.Z = acc_temp[2];

  return pos;
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

  pinMode(27, OUTPUT);
  pinMode(buttonA, INPUT);
  pinMode(buttonB, INPUT);

  Serial.println("Booting microbit...");
}

float getNormalisedMagnitude(Point point)
{
  return sqrt(pow(point.X, 2) + pow(point.Y, 2) + pow(point.Z, 2)) / 2000.0;
}

float applyLowPassFilter(float mag1, float mag2, float mag3, float mag4)
{
  return (mag1 + mag2 + mag3 + mag4) / 4.0;
}

float samplePosition(int sampleRate)
{
  float rawMagnitude = getNormalisedMagnitude(getPos());
  delay(1000 / sampleRate);
  return rawMagnitude;
}

float sigmoid(float sample)
{
  return 1.0 / (float)(1 + exp(sample * -1));
}

float getSample(int sampleRate)
{
  float mag1 = samplePosition(sampleRate);
  float mag2 = samplePosition(sampleRate);
  float mag3 = samplePosition(sampleRate);
  float mag4 = samplePosition(sampleRate);

  return sigmoid(applyLowPassFilter(mag1, mag2, mag3, mag4));
}

float getElapsedSeconds(int sampleLength, int sampleRate)
{
  return (float)sampleLength / ((float)sampleRate / 4.0);
}

int processSamples(const int samplesLength, const int sampleRate)
{
  int numSamples = (sampleRate / 4) * samplesLength;
  float sampleSet[numSamples];
  short foundStep = 0;
  int stepsFound = 0;
  int tempDistance = 0;

  for(int i = 0; i < numSamples; i++)
  {
    sampleSet[i] = getSample(sampleRate);
  }

  for(int i = 0; i < numSamples; i++)
  {
    if(foundStep == 0 && sampleSet[i] > THRESHOLD)
    {
      foundStep = 1;
      Serial.print(sampleSet[i]);
      Serial.println(", STEP!");
      tempDistance++;
    }
    else if(foundStep != 0 && sampleSet[i] < THRESHOLD)
    {
      float tempMilliseconds = getElapsedSeconds(tempDistance, sampleRate);
      if(tempMilliseconds > MIN_SECS)
      {
        Serial.println("END STEP");
        stepsFound++;
      }
      else
      {
        Serial.println("FALSE POSITIVE");
      }

      foundStep = 0;
      tempDistance = 0;
    }
    else if(foundStep != 0)
    {
      tempDistance++;
    }
  }

  return stepsFound;
}

void loop() {
  if (! digitalRead(buttonA)) {
    Serial.println("Displaying stepCount on matrix.");
    microbit.print(steps);
    delay(1000);
    microbit.clear();
  }

  if(!digitalRead(buttonB))
  {
    Serial.println("Resetting step count.");
    microbit.show(cross_bmp);
    delay(1000);
    microbit.clear();
    steps = 0;
  }

  steps += processSamples(2, 160);
}
