const unsigned char buttonPin = 62;
unsigned char delayMs = 500;
int positions[] = {0b10000001, 0b01000010, 0b00100100, 0b00011000};
unsigned char positionsLen = 4;

void setup() {
  DDRL = 0b11111111; // setting all L pins as output
  PORTL = 0; // setting all L pins LOW
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  if(digitalRead(buttonPin) == LOW) {
    for(int i = 0; i < positionsLen; i++){
      PORTL = positions[i];
      delay(delayMs);
    }
    PORTL = 0;
  }
}

