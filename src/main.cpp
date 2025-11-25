#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

#include "WifiCredentials.h"

// I2C pins for oled screen
#define SDA 21
#define SCL 22

// 7dig display pins
#define NUM_OF_DIGITS 4
uint8_t dPins[] = {15, 2, 4, 5};

// 74hc595 pins
#define SHCP 18
#define STCP 19
#define DS 23

// DHT
#define DHTPIN 13
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Weather Data
float in_temp = 0;
float out_temp = 0;
uint8_t in_humidity = 0;
uint8_t out_humidity = 0;

// Timer for weather API
unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

// Reading JSON from weather API
String jsonBuffer;

// 0 - 9 and . Symbols on 7Seg
unsigned char table[] =
{0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0x7f};



void Display(int id, unsigned char num);
String getWeatherJSON();
String httpGETRequest(const char* serverName);

void setup() {

  Serial.begin(115200);
  WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(SSID, Password);
    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());

  Serial.println(F("DHTxx test!"));

  dht.begin();

  pinMode(STCP, OUTPUT);
  pinMode(SHCP, OUTPUT);
  pinMode(DS, OUTPUT);
  for (int j = 0; j < NUM_OF_DIGITS; j++) pinMode(dPins[j], OUTPUT);

}

void loop() {

  // DHT
  // Wait a few seconds between measurements.
  delay(2000);

  in_humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  in_temp = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(in_humidity) || isnan(in_temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(in_humidity);
  Serial.print(F("°C "));
  Serial.print(in_temp);

  // Weather
  getWeatherJSON();

  // 7Dig

  for (int i = 0; i < NUM_OF_DIGITS; i++) {
    for (int j = 0; j < 11; j++) {
      Display(i, j);
      delay(500);
      Serial.println(j);
    }
    delay(500);
  }
}


void Display(int id, unsigned char num)
{
  digitalWrite(STCP, LOW);
  shiftOut(DS, SHCP, MSBFIRST, ~table[num]);
  digitalWrite(STCP, HIGH);
  for (int j = 0; j < NUM_OF_DIGITS; j++) digitalWrite(dPins[j], HIGH);
  digitalWrite(dPins[id], LOW);
}

String getWeatherJSON(){
  // Send an HTTP GET request
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        return "fail";
      }
    
      Serial.print("JSON object = ");
      Serial.println(myObject);
      Serial.print("Temperature: ");
      Serial.println(myObject["main"]["temp"]);
      Serial.print("Pressure: ");
      Serial.println(myObject["main"]["pressure"]);
      Serial.print("Humidity: ");
      Serial.println(myObject["main"]["humidity"]);
      Serial.print("Wind Speed: ");
      Serial.println(myObject["wind"]["speed"]);
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}