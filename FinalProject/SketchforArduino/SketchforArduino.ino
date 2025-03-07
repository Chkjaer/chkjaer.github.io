#include <Wire.h>
#include <RTClib.h>
#include "SevSeg.h"

RTC_DS1307 rtc;
SevSeg sevseg; // Create SevSeg object
int lastDisplayedTime = -1; // Store last displayed time to avoid unnecessary updates

void setup() {
    Serial.begin(115200);
    Wire.begin();

    // Initialize RTC
    if (!rtc.begin()) {
        Serial.println("⛔ Couldn't find RTC module!");
        while (1);
    }
    if (!rtc.isrunning()) {
        Serial.println("RTC is NOT running, setting time...");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Set time on first run
    }

    // SevSeg Setup
    byte numDigits = 4;
    byte digitPins[] = {2, 3, 4, 5};  // Digit control pins
    byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13}; // Segments A-G, DP
    bool resistorsOnSegments = false; // Change if resistors are on segment pins
    sevseg.begin(COMMON_CATHODE, numDigits, digitPins, segmentPins, resistorsOnSegments);
    sevseg.setBrightness(90);
}

void loop() {
    DateTime now = rtc.now();
    int currentTime = (now.hour() * 100) + now.minute(); // Get time in HHMM format

    // ✅ **Update display ONLY if the time has changed**
    if (currentTime != lastDisplayedTime) {
        sevseg.setNumber(currentTime, 2); // `2` adds decimal at second position (HH.MM)
        lastDisplayedTime = currentTime;
        Serial.print("⏰ Updated Display: ");
        Serial.println(currentTime);
    }

    // ✅ **Always refresh display for smooth multiplexing**
    sevseg.refreshDisplay();  
}
