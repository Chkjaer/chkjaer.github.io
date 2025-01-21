const int R = 9;
const int G = 10;
const int B = 11;
int buttonState = 0;

void setup() {
pinMode(9, OUTPUT); // RED
pinMode(10, OUTPUT); // GREEN
pinMode(11, OUTPUT); // BLUE
pinMode(3, INPUT); // BUTTON
}

void loop() {
  buttonState = digitalRead(3);
  while(buttonState == HIGH) {
  for(int i = 0; i < 256; i++){ 
      analogWrite(R, i);
      analogWrite(B, .5 * i);
      delay(5);
    }
    for(int i = 255; i >= 0; i--){
      analogWrite(R, i);
      analogWrite(B, .5* i);
      delay(5);
    }
    buttonState = digitalRead(3);
    delay(10);
  }
}

