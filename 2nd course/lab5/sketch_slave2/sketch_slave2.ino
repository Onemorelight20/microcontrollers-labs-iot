#define ZERO 0x11
#define ONE 0x12
#define TWO  0x13
#define FOUR  0x14
#define DOT  0x15
#define _ 0x10

// Slave2 address = 123
const byte SLAVE_ADDRESS = 0x7B;
const byte MESSAGE_LENGTH = 12;
byte message[MESSAGE_LENGTH] = {
  TWO, ONE, DOT,         // 2 1 .
  ZERO, TWO, DOT,        // 0 2 .
  TWO, ZERO, ZERO, FOUR,   // 2 0 0 2
  _, _
};

void setWriteModeRS485() {
  PORTD |= 1 << PD2;
  delay(1);
}

// переривання по завершенню передачі
ISR(USART_TX_vect)
{
  PORTD &= ~(1 << PD2); 
}

void writeDataToMaster() {
  unsigned short checkSumCRC = crc16_dect_x(message, MESSAGE_LENGTH - 2);
  byte firstByteOfCheckSum = (checkSumCRC >> 8) & 0xFF;
  byte secondByteOfCheckSum = checkSumCRC & 0xFF;
  message[MESSAGE_LENGTH - 2] = firstByteOfCheckSum;
  message[MESSAGE_LENGTH - 1] = secondByteOfCheckSum;

  // 1) без спотворень 
  // 2) 7й розряд найстаршого байту даних Slave1 
  // +3) 0й розряд наймолодшого байту даних Slave2 
  // +4) 6й та 7й розряд 4го байту даних Slave2 
  // 5) 1, 2, 4 розряди 9го байту даних Slave1
  for(int k = 0; k < 5; k++) {
    for (int i = 0; i < MESSAGE_LENGTH; i++) {
      byte byteToSend = message[i];
      if (k == 2 && i == 0) {
        byteToSend ^= (1 << 0);
      } else if (k == 3 && i == 3) {
        byteToSend ^= ((1 << 5) | (1 << 6)) ;
      }
      Serial.write(byteToSend);
    }
  }
}

void setup() {
  delay(1000);

  DDRD = 0b00000111;
  PORTD = 0b11111000;

  Serial.begin(14400, SERIAL_8N1);
  // задає біт UCSZ02, який описаний тут: "Біти UCSZn2, поєднані з бітом UCSZn1: 0 в UCSRnC, встановлюють
  // кількість бітів даних (Символ SiZe) у кадрі. використання приймача та передавача ". Це в основному
  // дозволяє вам вибрати 9-бітовий серійний номер (8 біт є більш поширеним)
  
  UCSR0B |= (1 << UCSZ02) | (1 << TXCIE0);
  UCSR0A |= (1 << MPCM0);

  delay(1);
}

void loop() {
  if (Serial.available()) {
    byte inByte = Serial.read();
    if (SLAVE_ADDRESS == inByte) {
      UCSR0A &= ~(1 << MPCM0);
      setWriteModeRS485();
      writeDataToMaster();
      delay(200);
    } 
  }
}

uint16_t crc16_dect_x(const uint8_t *data, size_t len) {
    uint16_t crc = 0x0000;
    uint16_t poly = 0x589;
    
    for (size_t i = 0; i < len; i++) {
        crc ^= ((uint16_t)data[i] << 8);
        
        for (int j = 0; j < 8; j++) {
            if ((crc & 0x8000) != 0) {
                crc = (crc << 1) ^ poly;
            } else {
                crc <<= 1;
            }
        }
    }
    
    return crc;
}
