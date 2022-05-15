/*** Includes ***/
// For Azure
#include <AzureIotHub.h>
#include <Esp32MQTTClient.h>
#include <WiFi.h>
#include "AzureIotHub.h"
#include "Esp32MQTTClient.h"
// For sensor and oled
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*** Defines ***/
#define INTERVAL 10000
#define DEVICE_ID "myESP32"
#define MESSAGE_MAX_LEN 256

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

#define BIN_SIZE 100     // Size of bin in cm   

/*** Global variables ***/
// Please input the SSID and password of WiFi
const char* ssid     = "huawei p smart";
const char* password = "2b3150c6";

/*String containing Hostname, Device Id & Device Key in the format:                         */
static const char* connectionString = "HostName=smart-bin-project.azure-devices.net;DeviceId=myESP32;SharedAccessKey=LbkztLp8nR0ZauWI5gFr4ICw1y/NesWYTAJwcOaBD1k=";

const char *messageData = "{\"deviceId\":\"%s\", \"messageId\":%d, \"Distance\":%f, \"Percentage\":%f}";

int messageCount = 1;
static bool hasWifi = false;
static bool messageSending = true;
static uint64_t send_interval_ms;

const int echoPin = 15;  // Echo Pin in the distance sensor
const int trigPin  = 2;  // Trig Pin in the distance sensor

static int n = 50;      // Samples

float distance = 0;
float percentage = 0;

String date = "No date selected";

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utilities
static void InitWifi()
{
  Serial.println("Connecting...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  hasWifi = true;
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

static void SendConfirmationCallback(IOTHUB_CLIENT_CONFIRMATION_RESULT result)
{
  if (result == IOTHUB_CLIENT_CONFIRMATION_OK)
  {
    Serial.println("Send Confirmation Callback finished.");
  }
}

static void MessageCallback(const char* payLoad, int size)
{
  Serial.println("Message callback:");
  Serial.println(payLoad);
  date = String(payLoad).substring(4, 16);
  Serial.println(date);
}

static void DeviceTwinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char *payLoad, int size)
{
  char *temp = (char *)malloc(size + 1);
  if (temp == NULL)
  {
    return;
  }
  memcpy(temp, payLoad, size);
  temp[size] = '\0';
  // Display Twin message.
  Serial.println(temp);
  free(temp);
}

static int  DeviceMethodCallback(const char *methodName, const unsigned char *payload, int size, unsigned char **response, int *response_size)
{
  LogInfo("Try to invoke method %s", methodName);
  const char *responseMessage = "\"Successfully invoke device method\"";
  int result = 200;

  if (strcmp(methodName, "start") == 0)
  {
    LogInfo("Start sending distance data");
    messageSending = true;
  }
  else if (strcmp(methodName, "stop") == 0)
  {
    LogInfo("Stop sending distance data");
    messageSending = false;
  }
  else
  {
    LogInfo("No method %s found", methodName);
    responseMessage = "\"No method found\"";
    result = 404;
  }

  *response_size = strlen(responseMessage) + 1;
  *response = (unsigned char *)strdup(responseMessage);

  return result;
}

float ping(int echoPin)
{
  float duration, cm;

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

float microsecondsToCentimeters(float microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29.0 / 2.0;
}

void drawProgressbar(int x,int y, int width,int height, int progress)
{
   progress = progress > 100 ? 100 : progress; // set the progress value to 100
   progress = progress < 0 ? 0 : progress; // start the counting to 0-100
   float bar = ((float)(width-1) / 100) * progress;
   display.drawRect(x, y, width, height, WHITE);
   display.fillRect(x+2, y+2, bar , height-4, WHITE); // initailize the graphics fillRect(int x, int y, int width, int height)
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Arduino sketch
void setup()
{
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  
  Serial.begin(115200);
  Serial.println("ESP32 Device");
  Serial.println("Initializing...");

  // Initialize the WiFi module
  Serial.println(" > WiFi");
  hasWifi = false;
  InitWifi();
  if (!hasWifi)
  {
    return;
  }
  randomSeed(analogRead(0));

  Serial.println(" > IoT Hub");
  Esp32MQTTClient_SetOption(OPTION_MINI_SOLUTION_NAME, "GetStarted");
  Esp32MQTTClient_Init((const uint8_t*)connectionString, true);

  Esp32MQTTClient_SetSendConfirmationCallback(SendConfirmationCallback);
  Esp32MQTTClient_SetMessageCallback(MessageCallback);
  Esp32MQTTClient_SetDeviceTwinCallback(DeviceTwinCallback);
  Esp32MQTTClient_SetDeviceMethodCallback(DeviceMethodCallback);

  send_interval_ms = millis();

  // Init OLED
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
  // distance sensor data, average of n samples
  distance = 0;
  for (int i = 0; i < n; i++)   // average of n samples
  {
    distance += ping(echoPin);        // Az analóg 0 csatornán mért érték (bit)
    delay(10);
  }

  distance = distance / (float)n;       // Az analóg 0 csatornán mért érték (bit)
  Serial.println(distance);             // print measured value
  
  percentage = 100.0-distance/BIN_SIZE*100.0;
  if (percentage > 100) percentage = 100.0; 
  if (percentage < 0) percentage = 0.0;
  Serial.println(percentage);           // print measured value
  
  // String displayString = "Distance: " + String(distance) + " cm";
  String displayString2 = String(percentage) + "%";
  //Serial.println(displayString);
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  //display.println(displayString);
  display.println("Next: "+date);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(30, 25);
  display.println(displayString2);
  drawProgressbar(0,45,120,15,percentage);
  display.display();
  
  if (hasWifi)
  {
    if (messageSending && 
        (int)(millis() - send_interval_ms) >= INTERVAL)
    {
      // Send distance data
      char messagePayload[MESSAGE_MAX_LEN];
      //float temperature = (float)random(0,50);
      //float humidity = (float)random(0, 1000)/10;
      snprintf(messagePayload,MESSAGE_MAX_LEN, messageData, DEVICE_ID, messageCount++, distance, percentage);
      Serial.println(messagePayload);
      EVENT_INSTANCE* message = Esp32MQTTClient_Event_Generate(messagePayload, MESSAGE);
      Esp32MQTTClient_Event_AddProp(message, "temperatureAlert", "true");
      Esp32MQTTClient_SendEventInstance(message);
      
      send_interval_ms = millis();
    }
    else
    {
      Esp32MQTTClient_Check();
    }
  }
  delay(1000);
}
