#include <Arduino.h>
#include <DHT.h>

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

unsigned char table[] =
{0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90};


void Display(int id, unsigned char num);

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));

  dht.begin();

  pinMode(STCP, OUTPUT);
  pinMode(SHCP, OUTPUT);
  pinMode(DS, OUTPUT);
  for (int j = 0; j < NUM_OF_DIGITS; j++) pinMode(dPins[j], OUTPUT);

}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));


  for (int i = 0; i < NUM_OF_DIGITS; i++) {
    for (int j = 0; j < 10; j++) {
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
