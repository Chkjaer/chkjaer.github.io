#include <SoftwareSerial.h>
#include <

SoftwareSerial espSerial(0, 1);  // Change back if needed

void setup() {
  Serial.begin(115200);
  espSerial.begin(115200);

  Serial.println("Connecting to Wi-Fi...");
  espSerial.println("AT+CWJAP=\"treehouse\",\"balls123\"");

  delay(5000);
  Serial.println("Checking Connection...");
  espSerial.println("AT+CIFSR"); // Get IP Address
}

void loop() {
  if (espSerial.available()) {
    Serial.write(espSerial.read());
  }
}