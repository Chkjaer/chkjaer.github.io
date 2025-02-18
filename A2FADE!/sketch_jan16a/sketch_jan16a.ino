const int R = 9;
const int G = 10;
const int B = 11;
int buttonState = 0;

void setup() {
pinMode(9, OUTPUT); // RED
}

void loop() {
  for(int i = 0; i < 150; i++){ 
      analogWrite(R, i);
    }
    for(int i = 150; i >= 0; i--){
      analogWrite(R, i);
    }
  }

