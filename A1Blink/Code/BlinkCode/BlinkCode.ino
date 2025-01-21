Christian

const int R = 9; // stores integer value of pin for red LED as "R"
const int G = 10; // stores integer value of pin for green LED as "G"
const int B = 11; // stores integer value of pin for blue LED as "B"
int buttonState = 0; // initializes global variable to be used to store the button's state

void setup() {
pinMode(R, OUTPUT); // initializes digital pin 9, assigned "R", as an output for the red LED
pinMode(G, OUTPUT); // initializes digital pin 10, assigned "G", as an output for the green LED
pinMode(B, OUTPUT); // initializes digital pin 11, assigned "B", as an output for the blue LED
pinMode(3, INPUT); // initializes digital pin 3 as an input to recieve a signal from a button
}

void loop() {
  digitalWrite(G, HIGH); // sets the non-button-pressed behavior to be a static green light
  buttonState = digitalRead(3); // reads for input from the button; will be HIGH when pressed and LOW when not pressed.
  if (buttonState == HIGH) { // if button is pressed, will fade out green.
  for(int i = 255; i >= 0; i--){ // Fades green LED all the way out
      analogWrite(G, i); // Sets green LED's brightness to current step in for loop
      delay(1); // controls speed of fade out
    }
  }
  while(buttonState == HIGH) { // While the button is pressed, a pink light will fade on and off in place of the static green light
  digitalWrite(G, LOW); // While the button is pressed, this ensures the green LED will stay off
  for(int i = 0; i < 256; i++) { // Fades in a pink light to full brightness
      analogWrite(R, i); // Sets red LED's brightness to current step in for loop
      analogWrite(B, .5 * i); // Sets the blue LED's brightness to half the brightness of the red LED
      delay(3); // This delay controls the speed of the fade in
    }
  for(int i = 255; i >= 0; i--){ // Fades pink light all the way out
      analogWrite(R, i); // Sets red LED's brightness to current step in for loop
      analogWrite(B, .5 * i); // // Sets the blue LED's brightness to half the brightness of the Red LED
      delay(3); // This delay controls the speed of the fade out
  }
    buttonState = digitalRead(3); // Re-checks if button is still pressed
    if(buttonState == LOW){ // If the button is no longer pressed, will fade in the green LED
      for(int i = 0; i < 256; i++){  // Fades in green LED to full brightness
        analogWrite(G, i); // sets Green LED's brightness to current step in for loop
        delay(1); // controls speed of fade in
      }
    }
    delay(5); // creates small delay between each loop
  }
}
