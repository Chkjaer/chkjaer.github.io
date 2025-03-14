#include <Wire.h>    // For communication with RTC module
#include <RTClib.h>  // For RTC module
#include "SevSeg.h"  // For Seven Segment Display

#define PIR_PIN A0  // PIR Motion Sensor Pin

RTC_DS1307 rtc; // Creates RTC object
SevSeg sevseg; // Creates SevSeg object

bool systemOn = false; // initializes storage of system status, "OFF" when motion is not detected for 5 seconds
unsigned long motionStartTime = 0; // used to keep track of how long ago motion was last detected

void setup() {
  Serial.begin(115200);  // Intializes serial communication with Seed ESP32S3 
  Wire.begin(); // Starts communication with RTC


  if (!rtc.begin()) {   // Sets up RTC
    while (1);
  }

  if (!rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Sets RTC time
  }

  // Sets up SevSeg Display
  byte numDigits = 4;
  byte digitPins[] = {2, 3, 4, 5};  
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13}; 
  bool resistorsOnSegments = false;
  sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(90);

  // Setup PIR Sensor
  pinMode(PIR_PIN, INPUT);
}

void loop() {
  DateTime now = rtc.now(); // Stores the time at this instant
  int currentTime = (now.hour() * 100) + now.minute(); // Format: HHMM

  static int lastMotionState = LOW;  // Tracks previous state of PIR sensor
  int motionDetected = digitalRead(PIR_PIN); // Reads motion sensor ("HIGH" = motion detected, "LOW" = no motion)

  // If it detects motion and system wasn't already on, will both turn on the seven segment display, 
  // and send a "ON" over serial to Seed
  if (motionDetected == HIGH && lastMotionState == LOW) { 
    motionStartTime = millis();  // Resets timer
    if (!systemOn) {
      systemOn = true;
      Serial.println("ON");  // Sends "ON" to Seed
    }
  }

  // Resets timer if motion is detected again, to prevent blinking on and off while motion is happening
  if (motionDetected == HIGH) {
    motionStartTime = millis();
  }

  // Checks if 5 seconds have passed since last motion, then turns seven segment display off and prints "OFF" to seed over serial
  if (systemOn && millis() - motionStartTime > 5000) {
    systemOn = false;
    Serial.println("OFF");  // Sends "OFF" to Seed
  }

  lastMotionState = motionDetected;  // Updates last motion state     

  // Updates 7-segment display only if system is ON
  if (systemOn) {
    sevseg.setNumber(currentTime, 2);  // Shows time with decimal point in the middle as pseudo ":"
    sevseg.refreshDisplay();
  } else { 
    sevseg.blank(); // Turns off display
    sevseg.refreshDisplay();
  }
}
