int x = A0;    
int y = A1;
int LED = 9;
int blinkSpeed = 500; // Default blink speed (ms)
int ledBrightness = 128; // Default brightness (0-255)
unsigned long lastBlinkTime = 0; // Stores the last time the LED state changed
bool ledState = false; // Tracks LED ON/OFF state

void setup() {
  Serial.begin(9600);
  pinMode(x, INPUT);
  pinMode(y, INPUT);
  pinMode(LED, OUTPUT);
}

void loop() {
  int xVal = analogRead(x); // Read joystick X
  int yVal = analogRead(y); // Read joystick Y
  // Send data to p5.js in a readable format
  Serial.print("[");
  Serial.print(xVal);
  Serial.print(",");
  Serial.print(yVal);
  Serial.println("]");
  // Check if there's incoming serial data from p5.js
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n'); // Read full input line
    input.trim(); // Remove spaces and newlines

    // Validate JSON-like input (must start with "[" and end with "]")
    if (input.startsWith("[") && input.endsWith("]")) {
      input = input.substring(1, input.length() - 1); // Remove brackets

      int commaIndex = input.indexOf(","); // Find comma position
      if (commaIndex > 0) { // Ensure valid input
        blinkSpeed = input.substring(0, commaIndex).toInt(); // First value = speed
        ledBrightness = input.substring(commaIndex + 1).toInt(); // Second value = brightness
      }
    }
  }
    // LED blinking using millis() so that blink speed doesn't affect sample speed *FROM CHATGPT*
  unsigned long currentMillis = millis(); // Get current time
  if (currentMillis - lastBlinkTime >= blinkSpeed) { 
    lastBlinkTime = currentMillis; // Update last blink time
    ledState = !ledState; // Toggle LED state
  }
  analogWrite(LED, ledState ? ledBrightness : 0);
  delay(100);
}
