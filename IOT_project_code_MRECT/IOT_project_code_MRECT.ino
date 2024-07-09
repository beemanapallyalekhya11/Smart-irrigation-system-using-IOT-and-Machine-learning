/*
 * This code shows how to monitor moisture level of plant along
   with other envirnmental variables like humidity and temperature

   App project setup:
     Humidity and Temperature readings are taken from DHT11 sensor
     DHT11 pin is attached to D4 of Nodemcu
     Soil Moisture sensor is attached to A pin of Nodemcu
     Water pump is attached to D8 for watering the plants

*/

#include <ThingSpeak.h>  //To connect our thingspeak channel with the esp8266 through this code.
#include <ESP8266WiFi.h>  // To connect the esp with internet
#include <DHT.h>
#include <Wire.h>           
 #include <LiquidCrystal_I2C.h>    
 LiquidCrystal_I2C lcd(0x27,16,2);

//---------------------Enter wifi credentials -----------------//
const char* ssid     = "project";    //Name of your wifi network
const char* password = "project07";  // Wifi password

/* Change these values based on your calibration values */
int soilWet = 65;        // Define max value we consider soil 'wet'   
int soilDry = 35;        // Define min value we consider soil 'dry'

#define sensorPin A0     //connect the sensor to analog pin of esp8266
#define motorPin 13
float Humidity = 0.0 ;
float Temperature = 0.0 ;


//---------------------Channel Details--------------------//
unsigned long Channel_ID =  2506568; // Channel ID
const char *WriteAPIKey = "27XIHEIC2S6QZX01"; // Your write API Key
WiFiClient  client;

#define DHTPIN 2              //D4 pin
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
long delayStart = millis();   // start delay

void setup() {
   pinMode(motorPin, OUTPUT);
   digitalWrite(motorPin, HIGH);
   Serial.begin(9600); 
   Serial.println();
   Serial.print("Connecting to ");
   Serial.println(ssid);
   lcd.begin();      
   lcd.backlight();
   lcd.setCursor(0,0);
   lcd.print("Smart Irrigation" );
   lcd.setCursor(0,1);
   lcd.print("     System" );
   WiFi.begin(ssid, password);  // Connecting esp to wifi
   while (WiFi.status() != WL_CONNECTED)  //This loop will work untill esp connects to the wifi
   {   
   delay(500);
   Serial.print(".");
   }
   ThingSpeak.begin(client);       //The client here is esp8266 
  delay(1000);
  lcd.clear();
  }


void loop() {
  Humidity = dht.readHumidity();
  Temperature = dht.readTemperature();
  Serial.print("Humidity : ");
  Serial.println(Humidity);
  Serial.print("Temperature :");
  Serial.println(Temperature);
  delay(1000);
   //get the reading from the function below and print it
  int moisture = analogRead(sensorPin);    //Read the analog values
  moisture = map(moisture, 0, 1024, 100, 0);
  Serial.print("Soil moisture: ");  //Print the analog values
  Serial.println(moisture);
  if ((millis() - delayStart) >= 15000) {
  ThingSpeak.writeField(Channel_ID,1,Humidity, WriteAPIKey);
  ThingSpeak.writeField(Channel_ID,2,Temperature, WriteAPIKey);
  ThingSpeak.writeField(Channel_ID,3,moisture, WriteAPIKey);
  lcd.setCursor(0,0);  
  lcd.print("Humidity : ");
  lcd.print(Humidity);  
  lcd.setCursor(0,1);  
  lcd.print("Moisture : ");
  lcd.print(moisture);
  // Determine status of our soil using the values already defined for wet and dry soil
if (moisture > soilWet) 
{
  Serial.println("Status: Soil is too wet");
  digitalWrite(motorPin, LOW); // Turn off motor when soil is too wet
  ThingSpeak.writeField(Channel_ID, 4, false, WriteAPIKey); // Update ThingSpeak with motor off status
} 
else if (moisture <= soilWet && moisture > soilDry) 
{
  Serial.println("Status: Soil moisture is perfect");
  digitalWrite(motorPin,LOW); // Keep motor off when soil moisture is perfect
  ThingSpeak.writeField(Channel_ID, 4, false, WriteAPIKey); // Update ThingSpeak with motor off status
} 
else 
{
  Serial.println("Status: Soil is too dry - starting motor");
  digitalWrite(motorPin, HIGH); // Turn on motor when soil is too dry
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Watering...");
  delay(2000); // Wait for 5 seconds (adjust as needed)
  digitalWrite(motorPin, LOW); // Turn off motor after watering
  lcd.clear();
  ThingSpeak.writeField(Channel_ID, 4, true, WriteAPIKey); // Update ThingSpeak with motor on status
}

delay(500);                // Take a reading every  half a second for testing
Serial.println();          // Normally you should take reading perhaps once or twice a day

  }}
