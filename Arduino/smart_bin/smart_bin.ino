/***********************************************************
  File name: smart_bin.ino
  Description: Arduino code for the smart bin IoT project
***********************************************************/

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using SPI:
#define OLED_MOSI  23
#define OLED_CLK   18
#define OLED_DC    16
#define OLED_CS    5
#define OLED_RESET 17
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
                         OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

const int echoPin = 15;  // Echo Pin in the distance sensor
const int trigPin  = 2;  // Trig Pin in the distance sensor

static int n = 50;      // Samples

long distance = 0;

void setup()
{

  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  Serial.begin(115200);             // serial for debug
  Serial.println("Program begun!");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  // Show the display buffer on the screen.
  display.display();

  delay(1000);
}

void loop()
{
  // int cm = ping(echoPin);

  // average of n samples
  distance = 0;
  for (int i = 0; i < n; i++)   // average of n samples
  {
    distance += ping(echoPin);        // Az analóg 0 csatornán mért érték (bit)
    delay(10);
  }

  distance = distance / n;       // Az analóg 0 csatornán mért érték (bit)
  Serial.println(distance);         // print measured value

  String displayString = "Tavolsag: " + String(distance) + " cm";
  Serial.println(displayString);
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  display.println(displayString);
  display.display();
  delay(1000);
}

int ping(int echoPin)
{
  long duration, cm;

  // Trigger pulse

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin, LOW);

  // Measure time on Echo
  duration = pulseIn(echoPin, HIGH);

  // convert the time into a distance
  cm = microsecondsToCentimeters(duration);
  return cm ;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}
