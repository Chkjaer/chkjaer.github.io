
#include <CapacitiveSensor.h>

int currentBrightness = 0;
int targetBrightness = 0;
int sensorMin = 5000; // arbitrary high number
int sensorMax = 0;
CapacitiveSensor   cs_4_2 = CapacitiveSensor(4,2);    	// 1 megohm resistor between pins 4 & 2, pin 2 is sensor pin, add wire, metal thing

void setup() {
  pinMode(9, OUTPUT); // attaches the servo on pin 9 to the servo object
  cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF); 	// turn off autocalibrate on channel 1 - just as an example
  Serial.begin(9600);

 while (millis() < 5000) { // calibrates photoresistor sensor max and min for the first 5 seconds
  long start = millis();
 	long total1 =  cs_4_2.capacitiveSensor(30);
  
 	// Serial.print(millis() - start);    	// check on performance in milliseconds
	// Serial.print("\t");             
          int sensorValue = total1; // checks the voltage measurement between photo resistor and second resistor.
          if (sensorValue > sensorMax) { // if sensor reading is higher than current max reading
            sensorMax = sensorValue; // set max reading to current reading
          }
          if (sensorValue < sensorMin) {
            sensorMin = sensorValue;
          }
          delay(10); // adds some time before sensor readings 
          Serial.println("Calibrating... TOUCH RAPIDLY"); // Lets user know device is still calibrating
        }
  Serial.print("Sensor Max: "); // tells user the calibrated max sensor reading.
  Serial.print(sensorMax);
  Serial.print("\t");
  Serial.print("Sensor Min: "); // tells user the calibrated max sensor reading.
  Serial.println(sensorMin);
}


void loop() {
	long start = millis();
	long total1 =  cs_4_2.capacitiveSensor(30);
	// Serial.print(millis() - start);    	// check on performance in milliseconds
	// Serial.print("\t");                	// tab character for debug window spacing 
  targetBrightness = map(total1, sensorMin, sensorMax, 0, 255);
  targetBrightness = constrain(targetBrightness, 0, 255);
  Serial.println(targetBrightness);   
  if (targetBrightness < 25) {
    targetBrightness = 0;
  } else if (targetBrightness > 190) {
    targetBrightness = 255;
  }
  while (currentBrightness < targetBrightness) {
      currentBrightness++;
      analogWrite(9, currentBrightness);
      delay(3);
  }
  while (currentBrightness > targetBrightness) {
      currentBrightness--;
      analogWrite(9, currentBrightness);
      delay(3);
  }
  analogWrite(9, currentBrightness);
  delay(20); // arbitrary delay to limit data to serial port
}                      	
