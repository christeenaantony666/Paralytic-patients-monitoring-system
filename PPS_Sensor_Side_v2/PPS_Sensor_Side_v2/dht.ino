#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// Uncomment the type of sensor in use:
#define DHTTYPE DHT11  // DHT 11

DHT_Unified dht1(dht1_pin, DHTTYPE);
DHT_Unified dht2(dht2_pin, DHTTYPE);

uint32_t delayMS1;
uint32_t delayMS2;

void dht1_setup() {
  dht1.begin();
  // Print temperature sensor details.
  sensor_t sensor;
  dht1.temperature().getSensor(&sensor);
  dht1.humidity().getSensor(&sensor);
  delayMS1 = sensor.min_delay / 1000;
}

void dht2_setup() {
  dht2.begin();
  sensor_t sensor;
  dht2.temperature().getSensor(&sensor);
  dht2.humidity().getSensor(&sensor);
  delayMS2 = sensor.min_delay / 1000;
}


void read_dht1() {
  // Delay between measurements.
  delay(delayMS1);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht1.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature1!"));
  } else {
    temperature1 = event.temperature;
  }
  // Get humidity event and print its value.
  dht1.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity1!"));
  } else {
    humidity1 = event.relative_humidity;
  }
}

void read_dht2() {
  // Delay between measurements.
  delay(delayMS2);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht2.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature2!"));
  } else {
    temperature2 = event.temperature;
  }
  // Get humidity event and print its value.
  dht2.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity2!"));
  } else {
    humidity2 = event.relative_humidity;
  }
}
