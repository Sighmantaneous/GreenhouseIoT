
#include <WiFi.h>
#include "secrets.h"// where SSID and password are stored 
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros
#include "DFRobot_AHT20.h"
#include <ESP32Servo.h>
#include "rgb_lcd.h"

DFRobot_AHT20 aht20; //creating instance of aht20 sensor as ah20
Servo servo1; // creating instance of servo motor as servo1

const int fanEnable = 17;
const int pumpEnable = 16;
const int servoPin = 14;
const int LED = 4;

const int MoistureSensor = 34;
const int MoistureDryThreshold = 1500;
const int MoistureWetThreshold = 3000;
const int MoistureMaximum = 4095;

int MoistureValue = 0;
int AnalogMoisture = 0;



char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

const int WindowOpen = 180;
const int WindowClosed = 0;
float celsiusTemp;
float humidity;
bool Window= false;
int WindowTime;

void setup() {
  Serial.begin(115200);  //Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  servo1.attach(servoPin);

   uint8_t status;
  while((status = aht20.begin()) != 0){
    Serial.print("AHT20 sensor initialization failed. error status : ");
    Serial.println(status);
    delay(1000);
}
pinMode(LED, OUTPUT);
pinMode(fanEnable, OUTPUT);
pinMode(pumpEnable, OUTPUT);
pinMode(servoPin, OUTPUT);
pinMode(MoistureSensor, INPUT);
}

void loop() {

  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }

  
    tempGet(celsiusTemp, humidity);
    delay(5000);

    MoistureValue = readMoisture(MoistureSensor);
    delay(5000);

    digitalWrite(fanEnable, HIGH);
    delay(5000);
    digitalWrite(fanEnable, LOW);
    
    
    
    
    ThingSpeak.setField(1, celsiusTemp);
    ThingSpeak.setField(2, humidity);
    ThingSpeak.setField(3, MoistureValue);

      tempCheck(celsiusTemp, Window);
  

  // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
  // pieces of information in a channel.  Here, we write to field 1.
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  
  }
  // change the value
  
  delay(20000); // Wait 20 seconds to update the channel again
}

//function to calculate the temp and humidity 
float tempGet(float celsiusTemp, float humidity){
  Serial.println("Entered tempGet Funtion");
  if(aht20.startMeasurementReady(true)){
    Serial.println("temperature(-40~85 C): ");
    Serial.print(aht20.getTemperature_C());
    // Get temp in Celsius (℃), range -40-80℃
    celsiusTemp = aht20.getTemperature_C();
    // Get relative humidity (%RH), range 0-100℃
    humidity = aht20.getHumidity_RH();
    return celsiusTemp, humidity;
  
  }
}
//function to calculate moisture in 0 -100 range
int readMoisture(int MoistureSensor){
    AnalogMoisture = analogRead(MoistureSensor);
    return ( 100 - ( (AnalogMoisture/4095) * 100 ) );

}
//function to see if window should be open based on temp
float tempCheck(float celsiusTemp, bool Window){
  Serial.println("Entered Function tempCheck");
  if(celsiusTemp > 21){    
        servo1.write(WindowOpen);
        Serial.println(WindowOpen);
        Window = true;
     delay(1000);
    }
    if(celsiusTemp > 22){
          for(int i = 0; i < 10 ; i++){
            digitalWrite(LED, HIGH);
            delay(1000);
            digitalWrite(LED, LOW);
            delay(1000);      
          }
    }
    else if(celsiusTemp < 20 || WindowTime >1200 )
    {
          
      servo1.write(WindowClosed);
      Serial.println("Window is Closed");
      Window = false;
      WindowTime = 0;
      delay(100);
    }
}
