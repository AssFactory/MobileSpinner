#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <SoftRcPulseOut.h> // default servo lib not work on digispark (attiny 85)

// hardware definition
#define LED_PIN 0
#define SERVO_Pin 2

// constants
#define LED_RING_CHANGE_DELAY_MS 1000
#define LED_RING_COLOR_STEP 800

#define SERVO_STEP_CHANGE_DELAY_MS 30
#define SERVO_DIR_CHANGE_DELAY_MS 1500

// declare objects and variables
Adafruit_NeoPixel ledRing = Adafruit_NeoPixel(12, LED_PIN, NEO_GRB + NEO_KHZ800);
unsigned long ledRingColorChanged;
uint16_t lastPixel;
uint16_t lastHue;

SoftRcPulseOut servo;
unsigned long servoPosChanged;
bool servoDirRight;

unsigned long aktTime;

void setup()
{
  // init variables
  ledRingColorChanged = 0;
  servoPosChanged = 0;
  lastPixel = 0;
  lastHue = 0;
  servoDirRight = true;
  aktTime = 0;

  // init neopixel ring
  ledRing.begin();
  uint32_t color = Adafruit_NeoPixel::ColorHSV(lastHue);
  color = Adafruit_NeoPixel::gamma32(color);
  ledRing.fill(color);
  ledRing.show();

  // init servo object
  servo.attach(SERVO_Pin);
  servo.setMinimumPulse(500);
  servo.setMaximumPulse(2300);
  servo.write(0);
  SoftRcPulseOut::refresh();
}

void setNextServoPos()
{
  // calcualte next servo pos
  uint8_t pos = servo.read();
  if (servoDirRight)
  {
    pos++;
  }
  else
  {
    pos--;
  }

  // change direction if servo at end position
  if (pos == 0 || pos == 180)
  {
    servoDirRight = !servoDirRight;
    servoPosChanged += SERVO_DIR_CHANGE_DELAY_MS;
  }

  // update position
  servo.write(pos);
}

void setNextRingColors()
{
  // calcualte new colors next pixel
  uint32_t color = Adafruit_NeoPixel::ColorHSV(lastHue);
  color = Adafruit_NeoPixel::gamma32(color);
  ledRing.setPixelColor(lastPixel, color);
  lastPixel++;

  // if end of ring is reached set next color step
  if (lastPixel == ledRing.numPixels())
  {
    lastHue += LED_RING_COLOR_STEP;
    lastPixel = 0;
  }

  // show changes
  ledRing.show();
}

void loop()
{
  // simple scheduler to allow multiple tasks
  aktTime = millis();

  // TasK 1: Update servo position
  if (aktTime > servoPosChanged)
  {
    servoPosChanged = aktTime + SERVO_STEP_CHANGE_DELAY_MS;
    setNextServoPos();
  }

  // Task 2: Generate 50 Hz signal for servo
  SoftRcPulseOut::refresh(); // scheduling is handled inside library

  // Task 3: Update ring colors
  if (aktTime > ledRingColorChanged)
  {
    ledRingColorChanged = aktTime + LED_RING_CHANGE_DELAY_MS;
    setNextRingColors();
  }
}