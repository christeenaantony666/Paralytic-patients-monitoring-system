#include <Wire.h>
#include "MAX30105.h"  

void temperature_setup() {
  //The LEDs are very low power and won't affect the temp reading much but
  //you may want to turn off the LEDs to avoid any local heating
  particleSensor.setup(0);  //Configure sensor. Turn off LEDs
  //particleSensor.setup(); //Configure sensor. Use 25mA for LED drive

  particleSensor.enableDIETEMPRDY();  //Enable the temp ready interrupt. This is required.
}

void getTempSensorData() {
  temperature_setup();
  delay(500);

  float temperatureC = particleSensor.readTemperature();
  tempSensorVal = temperatureC;
  Serial.print("temperatureC=");
  Serial.print(temperatureC, 4);

  float temperatureF = particleSensor.readTemperatureF();  //Because I am a bad global citizen
  Serial.print(" temperatureF=");
  Serial.print(temperatureF, 4);
  Serial.println();
}
