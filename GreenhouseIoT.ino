
//
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>
#include "homepage.h"
#include "secrets.h"     // where SSID and password are stored
#include "ThingSpeak.h"  
#include "DFRobot_AHT20.h"
#include "esp_sleep.h"

//Instance Creation
DFRobot_AHT20 aht20;  
Servo servo1;         
WiFiClient client;
WebServer server(80);

//variables for light
unsigned long lightTimer = 0;
const unsigned long daySeconds = 86400;
const unsigned long lightOnSeconds = 28800;

//GPIO pin variables
const int fanEnable = 17;
const int pumpEnable = 16;
const int servoPin = 14;
const int LED = 26;
const int MoistureSensor = 34;


//ADC moisture variables
const int MoistureDryThreshold = 500;
const int MoistureWetThreshold = 4095;
int AnalogMoisture = 0;

//timer variables
const int TimerTimeMS = 20000;
hw_timer_t *timer = NULL;

//WiFi variables
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int keyIndex = 0;
//Thingspeak variables
unsigned long myChannelNumber = SECRET_CH_ID;
const char *myWriteAPIKey = SECRET_WRITE_APIKEY;


//changing global variables
float celsiusTemp;
float humidityValue;
int MoistureValue ;
int angle;
bool Window = false;
bool shouldUpdate = false;

//Interrupt Timer
void ARDUINO_ISR_ATTR onTimer() {
  shouldUpdate = true;
}

//Function declarations
float tempGet();
float humidGet();
int readMoisture();
float tempCheck(float temp);
float soilCheck(float MoistureValue);
void thingSpeak();


void setup() {

  Serial.begin(115200);  //Initialize serial
  while (!Serial) {
    ; 
  }

  WiFi.mode(WIFI_STA);

//Webpage config
  server.on("/", HTTP_GET, handleRoot);

  server.on("/getValues", HTTP_GET, []() {
    String json = "{\"temperature\": " + String(celsiusTemp) + ", \"humidity\": " + String(humidityValue) + ", \"moisture\": " + String(MoistureValue) + "}";

    server.send(200, "application/json", json);
  });

  server.onNotFound(handleNotFound);
  server.begin();

  ThingSpeak.begin(client);  // Initialize ThingSpeak

  servo1.attach(servoPin);

  //Checking to see if sensor is working
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

  //Timer config
  timer = timerBegin(1000000);  //Timer Freq 10Mhz
  timerAttachInterrupt(timer, &onTimer);
  timerAlarm(timer, TimerTimeMS, true, 0);
}

void loop() {

  //local variables

  unsigned long timeNow = esp_timer_get_time() / 1000000;
  unsigned long inTimeRange = (timeNow - lightTimer) % daySeconds;

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

  celsiusTemp = tempGet();
  humidityValue = humidGet();
  MoistureValue = readMoisture();

  if (shouldUpdate == true) {
    shouldUpdate = false;
    Serial.println(celsiusTemp);
    Serial.println(MoistureValue);
    Serial.println(humidityValue);
    tempCheck(celsiusTemp);
    soilCheck(MoistureValue);
    thingSpeak();
  }

  if (inTimeRange < lightOnSeconds) {

    digitalWrite(LED, HIGH);
  } 
  else {
    digitalWrite(LED, LOW);
  }
}

//Functions

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
  for (int i = 0; i < 10; i++) { // Taking multiple samples to elimate spikes 
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
    
    digitalWrite(fanEnable, HIGH);
  }

  else if (celsiusTemp < 20 ) {
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

    delay(100);
  }
  return 0;
}
// checking to see if pump needs to be engaged
float soilCheck(float MoistureValue) {

  if (MoistureValue < 30) {

    digitalWrite(pumpEnable, HIGH);
    for (int i = 0; i < 20; i++) {
      delay(1000);
    }
    digitalWrite(pumpEnable, LOW);
  }
  return 0;
}
//function that sends data to thingspeak
void thingSpeak() {

  ThingSpeak.setField(1, celsiusTemp);
  ThingSpeak.setField(2, humidityValue);
  ThingSpeak.setField(3, MoistureValue);

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

void handleRoot() {
  server.send(200, "text/html", homepage);
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not Found");
}
