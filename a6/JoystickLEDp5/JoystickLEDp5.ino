// Christian Kjaer
// HCDE 439
// A6 Talking to the Web! 

int x = A0;    
int y = A1;
int LED = 9;
int blinkSpeed = 500; // Default blink speed, representing the time between blinks in ms
int ledBrightness = 128; // Default brightness out of 255
long lastBlinkTime = 0; // Stores the last time the LED state changed
bool ledState = false; // Tracks LED on/off state

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
    String input = Serial.readStringUntil('\n'); // Reads full input line
    input.trim(); // Removes spaces and newlines

    if (input.startsWith("[") && input.endsWith("]")) { // Verifies validity of JSON input
      input = input.substring(1, input.length() - 1); // Removes brackets from recieved JSON 
      int commaIndex = input.indexOf(","); // Finds comma position, delineating the blink speed and brightness values
      blinkSpeed = input.substring(0, commaIndex).toInt(); // First value = LED blink speed
      ledBrightness = input.substring(commaIndex + 1).toInt(); // Second value = LED brightness
    }
  }
    // Handles LED blinking using millis() so that blink speed doesn't affect sample speed;
    // Previous iterations blinked the LED with a variable delay, resulting in the blink speed also controling joystick reading rate
  long currentTime = millis(); // Gets current time in milliseconds
  if (currentTime - lastBlinkTime >= blinkSpeed) { // Checks that the time between blinks is atleast the recieved blink Speed sent from the webpage
    lastBlinkTime = currentTime; // Updates last blink time
    ledState = !ledState; // Toggles LED state
  }
  analogWrite(LED, ledState ? ledBrightness : 0); // Writes the recieved LED brightness value to the LED if the LED state is true, itherwise it turns off the LED
  delay(100); // delays the sampling to every 10th of a second
}
