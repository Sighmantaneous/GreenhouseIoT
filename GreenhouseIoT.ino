
//
#include <WiFi.h>
#include "secrets.h"     // where SSID and password are stored
#include "ThingSpeak.h"  // always include thingspeak header file after other header files and custom macros
#include "DFRobot_AHT20.h"
#include <ESP32Servo.h>
#include "homepage.h"
#include <WebServer.h>
#include "esp_sleep.h"

DFRobot_AHT20 aht20;  //creating instance of aht20 sensor as ah20
Servo servo1;         // creating instance of servo motor as servo1

RTC_DATA_ATTR unsigned long lightTimer = 0;
const unsigned long daySeconds = 86400;
const unsigned long lightOnSeconds = 28800;

const int fanEnable = 17;
const int pumpEnable = 16;
const int servoPin = 14;
const int LED = 26;

const int MoistureSensor = 34;
const int MoistureDryThreshold = 500;
const int MoistureWetThreshold = 4095;
const int MoistureMaximum = 4095;

const int TimerTimeMS = 5000;
hw_timer_t *timer = NULL;

const uint64_t sleepUS = 10 * 1000000;

int MoistureValue = 0;
int AnalogMoisture = 0;

char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password
int keyIndex = 0;           // your network key Index number (needed only for WEP)

WiFiClient client;

WebServer server(80);

unsigned long myChannelNumber = SECRET_CH_ID;
const char *myWriteAPIKey = SECRET_WRITE_APIKEY;

const int WindowOpen = 180;
const int WindowClosed = 0;
float celsiusTemp;
float humidityValue;
bool Window = false;
int WindowTime;
int fanTime = 0;
int angle;

int sleepTester = 1;

bool shouldUpdate = false;

void ARDUINO_ISR_ATTR onTimer() {
  shouldUpdate = true;
}

void handleRoot() {
  server.send(200, "text/html", homepage);
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not Found");
}

float tempGet();
float humidGet();
int readMoisture();
float tempCheck(float);
void thingSpeak();


void setup() {
  Serial.begin(115200);  //Initialize serial
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for Leonardo native USB port only
  }

  WiFi.mode(WIFI_STA);

  if (lightTimer == 0) {
    lightTimer = esp_timer_get_time() / 1000000 ; //get in seconds
  }

  Serial.println("Woke from sleep");



  server.on("/", HTTP_GET, handleRoot);

  server.on("/getValues", HTTP_GET, []() {
    String json = "{\"temperature\": " + String(celsiusTemp) + ", \"humidity\": " + String(humidityValue) + ", \"moisture\": " + String(MoistureValue) + "}";

    server.send(200, "application/json", json);
  });

  server.onNotFound(handleNotFound);

  server.begin();


  ThingSpeak.begin(client);  // Initialize ThingSpeak

  servo1.attach(servoPin);

  uint8_t status;
  while ((status = aht20.begin()) != 0) {
    Serial.print("AHT20 sensor initialization failed. error status : ");
    Serial.println(status);
    delay(1000);
  }
  pinMode(LED, OUTPUT);
  pinMode(fanEnable, OUTPUT);
  pinMode(pumpEnable, OUTPUT);
  pinMode(MoistureSensor, INPUT);

  timer = timerBegin(1000000);  //Timer Freq 10Mhz
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, TimerTimeMS * 1000, true, 0);
}

void loop() {

  unsigned long timeNow = esp_timer_get_time() / 1000000;
  unsigned long inTimeRange = (timeNow - lightTimer) % daySeconds ;
  // Connect or reconnect to WiFi

  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
    Serial.println(WiFi.localIP());
  }



  server.handleClient();
  delay(1100);
  /*
  digitalWrite(fanEnable, HIGH);
  delay(5000);
  digitalWrite(fanEnable, LOW);
*/
  digitalWrite(pumpEnable,HIGH);

  if(inTimeRange < lightOnSeconds){

    digitalWrite(LED,HIGH);
  } 
  else{
    digitalWrite(LED,LOW);
  }


  celsiusTemp = tempGet();
  humidityValue = humidGet();
  MoistureValue = readMoisture();

  if (shouldUpdate == true) {

    shouldUpdate = false;
    Serial.println(celsiusTemp);
    Serial.println(MoistureValue);
    Serial.println(humidityValue);
    tempCheck(celsiusTemp);
    thingSpeak();
  }
}

//function to calculate the temp and humidity
float tempGet() {
  if (aht20.startMeasurementReady(true)) {
    celsiusTemp = aht20.getTemperature_C();
    return celsiusTemp;
  }
}

float humidGet() {
  if (aht20.startMeasurementReady(true)) {
    humidityValue = aht20.getHumidity_RH();
    return humidityValue;
  }
}

//function to calculate moisture in 0 -100 range
int readMoisture() {

  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(MoistureSensor);
    delay(50);
  }
  AnalogMoisture = sum / 10;
  Serial.println(AnalogMoisture);
  int percentMoisture = 100 * (AnalogMoisture - 1500) / (MoistureWetThreshold - MoistureDryThreshold);

  // Ensure the value stays within 0-100% range
  if (percentMoisture < 0) {
    percentMoisture = 0;
  } else if (percentMoisture > 100) {
    percentMoisture = 100;
  }

  // Debugging Prints
  Serial.print("Raw Moisture Sensor Value: ");
  Serial.println(AnalogMoisture);
  Serial.print("Calculated Moisture Percentage: ");
  Serial.println(percentMoisture);

  return percentMoisture;
}
//function to see if window should be open based on temp
float tempCheck(float celsiusTemp) {
  if (celsiusTemp > 21) {

    Window = true;
    servo1.attach(servoPin);

    if (Window && angle < 180) {
      for (int i = 0; i <= 180; i += 5) {
        servo1.write(i);
        angle = i;
        delay(50);
      }
    }
    fanTime++;
    digitalWrite(fanEnable, HIGH);
  }

  else if (celsiusTemp < 20 || fanTime > 15) {
    if (Window) {
      for (int i = 180; i >= 0; i -= 5) {
        servo1.write(i);
        delay(50);
        angle = i;
      }
      Window = false;
      servo1.detach();
    }
    digitalWrite(fanEnable, LOW);

    fanTime = 0;
    delay(100);
  }
  return 0;
}

void thingSpeak() {

  ThingSpeak.setField(1, celsiusTemp);
  ThingSpeak.setField(2, humidityValue);
  ThingSpeak.setField(3, MoistureValue);
  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
  // pieces of information in a channel.  Here, we write to field 1.

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  Serial.println("ThingSpeak Response Code: ");
  Serial.println(x);


  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  delay(1000);
}
