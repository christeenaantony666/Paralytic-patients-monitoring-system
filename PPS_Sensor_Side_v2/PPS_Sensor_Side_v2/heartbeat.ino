#include <Wire.h>
#include "MAX30105.h"

#include "heartRate.h"

const byte RATE_SIZE = 4;  //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE];     //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0;  //Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

void heartbeat_setup() {
  // Initialize sensor
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST))  //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1)
      ;
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup();                     //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A);  //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0);   //Turn off Green LED
}

void getHbSensorData() {
  heartbeat_setup();
  int hbloop = 0;
  long irValue = particleSensor.getIR();
  if (irValue < 50000) {
    Serial.println(" No finger?");
    hbSensorCount = 0;
  } else {
    beatAvg = 0;
    int hb_valid = 0;
    int maxbeat = 0;
    while (hbloop < 1000 && (beatAvg < 70 || beatAvg > 80)) {
      //Serial.println("Inside Hb loop");
      hbloop++;
      irValue = particleSensor.getIR();
      if (checkForBeat(irValue) == true) {
        //We sensed a beat!
        long delta = millis() - lastBeat;
        lastBeat = millis();

        beatsPerMinute = 60 / (delta / 1000.0);

        if (beatsPerMinute < 255 && beatsPerMinute > 20) {
          rates[rateSpot++] = (byte)beatsPerMinute;  //Store this reading in the array
          rateSpot %= RATE_SIZE;                     //Wrap variable

          //Take average of readings
          beatAvg = 0;
          for (byte x = 0; x < RATE_SIZE; x++)
            beatAvg += rates[x];
          beatAvg /= RATE_SIZE;
        }
      }
      Serial.print("IR=");
      Serial.print(irValue);
      Serial.print(", BPM=");
      Serial.print(beatsPerMinute);
      Serial.print(", Avg BPM=");
      Serial.print(beatAvg);
      Serial.println();
      if (beatAvg > maxbeat) {
        maxbeat = beatAvg;
      }

      if (irValue < 50000) {
        Serial.println(" No finger?");
        return;
      }
      //delay(10);
    }
    if (beatAvg > 60 && beatAvg < 90) {
      hbSensorCount = beatAvg;
      Serial.print("hbSensorCount=");
      Serial.print(hbSensorCount);
      Serial.println();
      delay(1000);
    } else {
      hbSensorCount = maxbeat;
      Serial.print("hbSensorCount=");
      Serial.print(hbSensorCount);
      Serial.println();
      delay(1000);
    }
  }
}
