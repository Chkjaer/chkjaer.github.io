#include <Servo.h>
#include <CapacitiveSensor.h>

int currentAng = 0; // initializes current Angle representing the Servos current position
int sensorMin = 5000; // initalizes minimum sensor reading, set to an arbitrary high number which will be reduced down to the lowest detected reading from the sensor during calibration
int sensorMax = 0; // initalizes maximum sensor reading, which will be increased up to the highest detected reading from the sensor during calibration
int activationThreshold = 0; // initializes the threshold for sensor values that activate the servo or not, representing the average of the sensor max and sensor min post calibration
CapacitiveSensor   cs_4_2 = CapacitiveSensor(4,2);    	// 1 megohm resistor between pins 4 & 2, pin 2 is sensor pin, add wire, metal thing

Servo myServo;  // create a servo object

void setup() {
  myServo.attach(10); // attaches the servo on pin 10 to the servo object
  cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF); 	// turn off autocalibrate on channel 1 - just as an example *FROM SLIDES, NOT SURE WHAT THIS DOES*
  Serial.begin(9600); // Starts serialization with baud rate of 9600
  

  while (millis() < 5000) { // calibrates photoresistor sensor max and min for the first 5 seconds
    long start = millis(); // starts milliseconds timer to be used for performance debugging
    long sensorValue =  cs_4_2.capacitiveSensor(30); // takes a reading
    // Serial.print(millis() - start);    	// check on performance in milliseconds, for debugging
    // Serial.print("\t");             // tab character for debug window spacing
    // Serial.println(sensorValue);         // outputs the sensor reading for debugging
    if (sensorValue > sensorMax) { // if sensor reading is higher than current max reading
      sensorMax = sensorValue; // set max reading to current reading
    }
    if (sensorValue < sensorMin) { // if sensor reading is lower than current max reading
      sensorMin = sensorValue; // set min reading to current reading
    }
  delay(10); // adds small delay between sensor readings 
  Serial.println("Calibrating... TOUCH CAPACITIVE SENSOR RAPIDLY"); // Lets user know device is still calibrating, and that they should touch it repeatedly to help it calibrate high and low readings.
  }
  Serial.print("Sensor Max: "); // tells user the calibrated max sensor reading.
  Serial.print(sensorMax); 
  Serial.print("\t");
  Serial.print("Sensor Min: "); // tells user the calibrated min sensor reading.
  Serial.println(sensorMin); 
  activationThreshold = (sensorMax + sensorMin) / 2; // sets the reading at which the servo will activate at, derived from the average of the sensor max and sensor min post calibration
  Serial.print("Activation Threshold: "); // tells the user the activationThreshold
  Serial.println(activationThreshold);
}


void loop() {
	long start = millis(); // starts milliseconds timer to be used for performance debugging
	long sensorValue =  cs_4_2.capacitiveSensor(30); // takes a capacitive touch sensor reading 

	// Serial.print(millis() - start);    	// check on performance in milliseconds, for debugging
	// Serial.print("\t");                	// tab character for debug window spacing
	// Serial.println(sensorValue);         // outputs the sensor reading for debugging

  while (sensorValue > activationThreshold && currentAng < 180) { // while the sensor reading is above the activation threshold and hasn't reached the servos max angle, it will increment the servos angle being written to it
    currentAng++;
    myServo.write(currentAng); // 
  }
  
  while (sensorValue < activationThreshold && currentAng > 0) { // while the sensor reading is below the activation threshold and hasn't reached the servos min angle, it will deincrement the servos angle being written to it
    currentAng--;
    myServo.write(currentAng);
  }
  delay(10);                         	// arbitrary delay to limit data to serial port
}
