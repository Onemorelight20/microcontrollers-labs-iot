const unsigned char button1Pin = 62;
const unsigned char button2Pin = 63;
unsigned char delayMs = 500;
int algo3positions[] = {0b10000001, 0b01000010, 0b00100100, 0b00011000};
unsigned char positionsLen = 4;
int receivedInt;

void setup() {
  Serial.begin(9600);

  DDRL = 0b11111111; // setting all L pins as output
  PORTL = 0; // setting all L pins LOW
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
}

void loop() {
  if (Serial.available()) {
    receivedInt = Serial.read();
    if (receivedInt == 3) {
      exec_algo3();
    }
    else if (receivedInt == 5){
      exec_algo5();
    }
  }

  if(digitalRead(button1Pin) == LOW) {
    Serial.write(3);
    delay(100);
  }
  if(digitalRead(button2Pin) == LOW) {
    Serial.write(5);
    delay(100);
  }
  
}

void exec_algo3() {
    for(int i = 0; i < positionsLen; i++){
      PORTL = algo3positions[i];
      delay(delayMs);
    }
    PORTL = 0;
}

void exec_algo5() {
    for(int i = 0; i < 8; i++){
      int res;
      
      if(i < 4) res = 1 << (i * 2);
      else res = (128 >> (i % 4) * 2);
      PORTL = res;
      delay(delayMs);
    }
    PORTL = 0;
}

