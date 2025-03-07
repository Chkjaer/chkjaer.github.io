#include <Wire.h>
#include <RTClib.h>
#include "SevSeg.h"

#define PIR_PIN A0          // PIR Motion Sensor Pin
#define SEED_TX_PIN 1      // TX Pin for Seed Controller activation

RTC_DS1307 rtc;
SevSeg sevseg; // Create SevSeg object

bool systemOn = false;      // Track if displays are ON
unsigned long motionStartTime = 0; // Store last motion detection time

void setup() {
    Serial.begin(115200);
    Wire.begin();

    // Setup RTC
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC module!");
        while (1);
    }
    if (!rtc.isrunning()) {
        Serial.println("RTC is NOT running, setting time...");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set RTC time
    }

    // Setup SevSeg (7-Segment Display)
    byte numDigits = 4;
    byte digitPins[] = {2, 3, 4, 5};  
    byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13}; 
    bool resistorsOnSegments = false;
    sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins, resistorsOnSegments);
    sevseg.setBrightness(90);

    // Setup PIR Sensor with Pull-Down Resistor
    pinMode(PIR_PIN, INPUT);
    digitalWrite(PIR_PIN, LOW); // Ensure pin starts LOW

    // Setup TX Pin for Seed Controller
    pinMode(SEED_TX_PIN, OUTPUT);
    digitalWrite(SEED_TX_PIN, LOW);  // Ensure it's LOW at startup

    Serial.println("System Initialized.");
}

void loop() {
    DateTime now = rtc.now();
    int currentTime = (now.hour() * 100) + now.minute(); // Format: HHMM

    static int lastMotionState = LOW;
    int motionDetected = digitalRead(PIR_PIN);

    //  **Only detect motion when system is OFF**
    if (!systemOn && motionDetected == HIGH && lastMotionState == LOW) {
        Serial.println("Motion Detected! Turning ON displays...");
        motionStartTime = millis();
        systemOn = true;

        digitalWrite(SEED_TX_PIN, HIGH);  // Signal Seed controller to turn on
        lastMotionState = HIGH;  // Prevent multiple triggers
    }

    //  **Turn off displays if 20 seconds have passed & no new motion**
    if (systemOn && millis() - motionStartTime > 7000) {
        Serial.println("No motion for 5 sec. Turning OFF displays...");
        systemOn = false;
        digitalWrite(SEED_TX_PIN, LOW);  // Signal Seed controller to turn off
        lastMotionState = LOW; // Reset motion state
    }

    //  **Update display ONLY if the time has changed & system is ON**
    if (systemOn) {
        sevseg.setNumber(currentTime, 2);  // `2` adds decimal at second position (HH.MM)
        sevseg.refreshDisplay();
    } else {
      sevseg.blank();
      sevseg.refreshDisplay();
    }
}