#include <SoftwareSerial.h>
#include <Wire.h>
#include "MAX30105.h"

const int flex1_pin = A0;
const int flex2_pin = A1;
const int flex3_pin = A2;

const int force1_pin = A3;
const int force2_pin = A4;

const int dht1_pin = 2;
const int dht2_pin = 3;

const int buzzer_pin = 4;

int flex1_value;
int flex2_value;
int flex3_value;

int force1_value;
int force2_value;

int hum1_value;
int hum2_value;

int flex1_value_threshold = 200;
int flex2_value_threshold = 200;
int flex3_value_threshold = 200;

int force1_value_threshold = 500;
int force2_value_threshold = 500;

int hum1_value_threshold = 80;
int hum2_value_threshold = 80;

int temp1_value_threshold = 40;
int temp2_value_threshold = 40;

String inputString = "";      // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

MAX30105 particleSensor;

#define espSerial Serial1

#define debug_enable 1

int start_checkup = 0;
int countTrueCommand;
int countTimeCommand;

boolean found = false;
float tempSensorVal = 0;
int hbSensorCount = 0;

String string1 = "COMM:";
String string2 = "TEMP:";
String string3 = "HBCT:";
String string4 = "WARN:";
String string5 = "SORE:";

long int previous_time = 0;
long int current_time = 0;
long int diff_time = 0;
unsigned long duration;

int fingerplaced = false;
int firstflag = 1;

int temperature1;
int temperature2;

int humidity1;
int humidity2;


int command_id = 0;
int warn_flag = 0;
int bed_sore_flag = 0;

int bed_sore2_count = 0;
int bed_sore1_count = 0;

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);

  pinMode(buzzer_pin, OUTPUT);
  digitalWrite(buzzer_pin, HIGH);

  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD))  //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1)
      ;
  }
  // reserve 200 bytes for the inputString:
  inputString.reserve(100);

  dht1_setup();
  dht2_setup();
}

void loop() {

  flex1_value = analogRead(flex1_pin);
  flex2_value = analogRead(flex2_pin);
  flex3_value = analogRead(flex3_pin);

  force1_value = analogRead(force1_pin);
  force2_value = analogRead(force2_pin);

  String force_str = "Force:" + String(force1_value) + "," + String(force2_value);
  Serial.println(force_str);
  
  read_dht1();
  delay(100);
  read_dht2();
  delay(100);

  debug_print("flex1_value: ");
  debug_print(String(flex1_value));
  debug_print("\t");
  debug_print("flex2_value: ");
  debug_print(String(flex2_value));
  debug_print("\t");
  debug_print("flex3_value: ");
  debug_print(String(flex3_value));
  debug_print("\t");
  debug_print("force1_value: ");
  debug_print(String(force1_value));
  debug_print("\t");
  debug_print("force2_value: ");
  debug_print(String(force2_value));
  debug_print("\t");
  debug_print("humidity1: ");
  debug_print(String(humidity1));
  debug_print("\t");
  debug_print("humidity2: ");
  debug_print(String(humidity2));
  debug_print("\t");
  debug_print("temperature1: ");
  debug_print(String(temperature1));
  debug_print("\t");
  debug_print("temperature2: ");
  debug_print(String(temperature2));
  debug_println("\t");

  //index finger up
  if ((flex1_value < flex1_value_threshold) && (flex2_value > flex2_value_threshold) && (flex3_value > flex3_value_threshold)) {
    debug_println("Food & Water Command");
    command_id = 1;
  }
  //index & middle finger up
  else if ((flex1_value > flex1_value_threshold) && (flex2_value < flex2_value_threshold) && (flex3_value > flex3_value_threshold)) {
    debug_println("Fresh up Command");
    command_id = 2;
  }
  //all fingers close
  else if ((flex1_value > flex1_value_threshold) && (flex2_value > flex2_value_threshold) && (flex3_value < flex3_value_threshold)) {
    debug_println("Emergency Command3");
    command_id = 3;
    digitalWrite(buzzer_pin, LOW);
    delay(1000);
    digitalWrite(buzzer_pin, HIGH);
    delay(1000);
    digitalWrite(buzzer_pin, LOW);
    delay(1000);
    digitalWrite(buzzer_pin, HIGH);
  } else {
    debug_println("No Command");
    command_id = 0;
  }

  if (force1_value > force1_value_threshold) {
    bed_sore1_count++;
    bed_sore2_count = 0;
  } else if (force2_value > force2_value_threshold) {
    bed_sore2_count++;
    bed_sore1_count = 0;
  } else if ((force1_value < force1_value_threshold) && (force2_value < force2_value_threshold)) {
    bed_sore1_count = 0;
    bed_sore2_count = 0;
  }
  if ((bed_sore1_count > 5) || (bed_sore2_count > 5) || (humidity1 > hum1_value_threshold) || (humidity2 > hum1_value_threshold) || (temperature1 > temp1_value_threshold) || (temperature2 > temp2_value_threshold)) {
    bed_sore_flag = 1;
  } else {
    bed_sore_flag = 0;
  }
  debug_print("bed_sore1_count: ");
  debug_print(String(bed_sore1_count));
  debug_print("\t");
  debug_print("bed_sore2_count: ");
  debug_println(String(bed_sore2_count));

  check_fingerplaced();
  if (fingerplaced == true) {
    debug_print(F("Reading Temp.."));
    getTempSensorData();
    delay(500);
    debug_print(F("Reading HeartBeat.."));
    getHbSensorData();
    delay(500);
    if (tempSensorVal > 38 || hbSensorCount < 60 || hbSensorCount > 80) {
      warn_flag = 1;
      digitalWrite(buzzer_pin, LOW);
      delay(1000);
      digitalWrite(buzzer_pin, HIGH);
      delay(1000);
      digitalWrite(buzzer_pin, LOW);
      delay(1000);
      digitalWrite(buzzer_pin, HIGH);
    } else {
      warn_flag = 0;
      digitalWrite(buzzer_pin, HIGH);
    }
    send_sensor_value();
  } else {
    send_sensor_value();
    delay(100);
  }
}

void check_fingerplaced() {
  particleSensor.setup();                     //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A);  //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0);   //Turn off Green LED
  delay(100);
  long fingerValue = particleSensor.getIR();
  if (fingerValue < 50000) {
    debug_println(" No finger?");
    fingerplaced = false;
    //digitalWrite(buzzer, LOW);
    if (fingerplaced == false && firstflag == true) {
      //digitalWrite(buzzer, HIGH);
      debug_println(F("Place the finger"));
      delay(2000);
      firstflag = false;
    }
  } else {
    if (fingerplaced == false) {
      debug_println(F("Finger Placed"));
      delay(2000);
      //debug_println(" No finger placed");
    }
    fingerplaced = true;
    firstflag = true;
  }
}

void send_sensor_value() {
  if (command_id > 0) {
    espSerial.print(string1);  //COMM
    espSerial.println(command_id);
    delay(100);
  }

  espSerial.print(string2);  //TEMP
  espSerial.println(tempSensorVal);
  delay(100);

  espSerial.print(string3);  //HBCT
  espSerial.println(hbSensorCount);
  delay(100);

  espSerial.print(string4);  //WARN
  espSerial.println(warn_flag);
  delay(100);

  espSerial.print(string5);  //SORE
  espSerial.println(bed_sore_flag);
  delay(100);
}

void debug_print(String str) {
#ifdef debug_enable
  Serial.print(str);
#endif
}

void debug_println(String str) {
#ifdef debug_enable
  Serial.println(str);
#endif
}


void espSerialEvent() {
  while (espSerial.available()) {
    // get the new byte:
    char inChar = (char)espSerial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

