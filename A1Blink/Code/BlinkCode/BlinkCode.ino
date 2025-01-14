/*
A1 Blink! HCDE 439
Christian Kjaer

Attribution: Code is based on Blink! Example from Arduino. The documentation for Blink example is included here:
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Blink
*/

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(10, OUTPUT); // initialize digital pin 10 (Red LED) as an output.
  pinMode(9, OUTPUT); // initialize digital pin 9 (Green LED) as an output.
  pinMode(8, OUTPUT); //  initialize digital pin 8 (Blue LED) as an output.
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(10, HIGH);  // turn the Red LED on (HIGH is the voltage level)
    delay(500);                    // wait for .5 seconds
  digitalWrite(10, LOW);   // turn the Red LED off by making the voltage LOW
    delay(50);                       // wait for .05 seconds
  digitalWrite(9, HIGH); // turn the Green LED on (HIGH is the voltage level)
    delay(500);                      // wait for .5 seconds
  digitalWrite(9, LOW);    // turn the Green LED off by making the voltage LOW
    delay(50);                       // wait for .05 seconds
  digitalWrite(10, HIGH);  // turn the Red LED on (HIGH is the voltage level)
    delay(500);                      // wait for .5 seconds
  digitalWrite(10, LOW);   // turn the Red LED off by making the voltage LOW
    delay(50);                       // wait for .05 seconds
  digitalWrite(8, HIGH);   // turn the Blue  LED on (HIGH is the voltage level)
    delay(500);                      // wait for .5 seconds
  digitalWrite(8, LOW);    // turn the Blue LED off by making the voltage LOW
    delay(50);                       // wait for .05 seconds
  digitalWrite(9, HIGH); // turn the Green LED on (HIGH is the voltage level)
    delay(500);                      // wait for .5 seconds
  digitalWrite(9, LOW);   // turn the Green LED off by making the voltage LOW
    delay(50);                         // wait for .05 seconds
  digitalWrite(8, HIGH);  // turn the Blue LED on (HIGH is the voltage level)
    delay(500);                        // wait for .5 seconds
  digitalWrite(8, LOW);   // turn the Blue LED off by making the voltage LOW
    delay(50);                         // wait for .05 seconds
}
