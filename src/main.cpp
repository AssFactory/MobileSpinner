#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <SoftRcPulseOut.h> // default servo lib not work on digispark (attiny 85)

// hardware definition
#define LED_PIN 0
#define SERVO_Pin 3

// wait times
#define RING_REFRESH_INTERVALL 10
#define SERVO_REFRESH_INTERVALL 15
#define SERVO_DIR_CHANGE_DELAY 1000

// declare objects and variables
Adafruit_NeoPixel ring = Adafruit_NeoPixel(12, LED_PIN, NEO_GRB + NEO_KHZ800);
SoftRcPulseOut servo;
unsigned long servroRefreshed;
unsigned long ringRefreshed;
unsigned long aktTime;
bool servoDirRight;
uint16_t lastHue;

void setup() 
{
  // init variables
  servroRefreshed = 0;
  ringRefreshed = 0;
  aktTime = 0;
  servoDirRight = true;
  lastHue = 0;

  // init neopixel ring 
  ring.begin();
  ring.rainbow(lastHue);
  ring.show(); 

  // init servo object
  servo.attach(SERVO_Pin);
  servo.setMinimumPulse(500);
  servo.setMaximumPulse(2300);
  servo.write(0);
  servo.refresh();
}

void loop() 
{
  aktTime = millis();

  if(aktTime > servroRefreshed)
  {
    servroRefreshed = aktTime + SERVO_REFRESH_INTERVALL;
    uint8_t pos = servo.read();
    if(servoDirRight)
    {
      pos++;
    }
    else
    {
      pos--;
    }

    if(pos == 0 || pos == 180)
    {
      servoDirRight = !servoDirRight;
      servroRefreshed += SERVO_DIR_CHANGE_DELAY;
    }

    servo.write(pos);
    servo.refresh();
  }

  if(aktTime > ringRefreshed)
  {
    ringRefreshed = aktTime + RING_REFRESH_INTERVALL;
    lastHue += (65535 / ring.numPixels());
    ring.rainbow(lastHue);
    ring.show();
  }
}