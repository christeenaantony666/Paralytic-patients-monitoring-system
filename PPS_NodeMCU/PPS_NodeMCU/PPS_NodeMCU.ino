/* Fill-in information from Blynk Device Info here */
#define BLYNK_TEMPLATE_ID "TMPL3Bi4qwuhc"
#define BLYNK_TEMPLATE_NAME "PARALYSIS PATIENT SYSTEM"
#define BLYNK_AUTH_TOKEN "TLGDPRZWQcxE70lJDRIi0phfz8S5JbBh"

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(D5, D4);  // RX, TX
// Your WiFi credentials.
// Set password to "" for open networks.
//char ssid[] = "OnePlus Nord";
//char pass[] = "nandukuttan";

char ssid[] = "TP-Link_F46C";
char pass[] = "*8547122440@";

BlynkTimer timer;

String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete

String string1 = "COMM:";
String string2 = "TEMP:";
String string3 = "HBCT:";
String string4 = "WARN:";
String string5 = "SORE:";

String command_list[5] = { "", "Food & Water", "Fresh up", "Emergency", "" };


int data1 = 0;
int data2 = 0;
int data3 = 0;
int data4 = 0;
int data5 = 0;

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent() {
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  if (data1 > 0) {
    Blynk.virtualWrite(V0, command_list[data1]);  //Command
  }
  Blynk.virtualWrite(V1, data2);  // Temperature
  Blynk.virtualWrite(V2, data3);  // HeartRate
  if (data4 == 1) {
    Blynk.virtualWrite(V3, "High temperature detected");  //Alert
  } else if (data4 == 2) {
    Blynk.virtualWrite(V3, "Abnormal Heart Rate detected");  //Alert
  } else {
    Blynk.virtualWrite(V3, "Normal");  //Alert
  }
  if (data5 == 1) {
    Blynk.virtualWrite(V4, "Chances of bedsore is there");  //Bedsore
  } else {
    Blynk.virtualWrite(V4, "-");  //Alert
  }
}

void setup() {
  // Debug console
  Serial.begin(9600);
  mySerial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  // You can also specify server:
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, "blynk.cloud", 80);
  //Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass, IPAddress(192,168,1,100), 8080);

  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
}

void loop() {
  Blynk.run();
  timer.run();  // Initiates BlynkTimer
  receiveAndParseData();
}

void mySerialEvent() {
  while (mySerial.available()) {
    char inChar = (char)mySerial.read();
    //Serial.print(inChar);
    if (inChar == '\n' || inChar == '\r') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }
}

void receiveAndParseData() {
  mySerialEvent();
  if (stringComplete == true) {
    Serial.println(inputString);
    if (inputString.startsWith(string1)) {
      inputString.remove(0, 5);
      //Serial.println(inputString);
      data1 = inputString.toInt();
    } else if (inputString.startsWith(string2)) {
      inputString.remove(0, 5);
      //Serial.println(inputString);
      data2 = inputString.toInt();
    } else if (inputString.startsWith(string3)) {
      inputString.remove(0, 5);
      //Serial.println(inputString);
      data3 = inputString.toInt();
    } else if (inputString.startsWith(string4)) {
      inputString.remove(0, 5);
      //Serial.println(inputString);
      data4 = inputString.toInt();
    } else if (inputString.startsWith(string5)) {
      inputString.remove(0, 5);
      //Serial.println(inputString);
      data5 = inputString.toInt();
    }
    inputString = "";
    stringComplete = false;
  }
}
