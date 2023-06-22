#include <string.h>
#include "SHT2x.h"

// Slave1 address = 222
const byte SLAVE_ADDRESS = 0xDE;

// 1 byte array length
// 4 bytes of temperature (float = 32 bits = 4 bytes)
// 4 bytes of humidity (float = 32 bits = 4 bytes)
// 2 bytes checksum values
// - ---- ---- --
const byte MESSAGE_LENGTH = 11;
byte message[MESSAGE_LENGTH];

SHT25 sht;

void setWriteModeRS485() {
  PORTD |= 1 << PD2;
  delay(1);
}

// переривання при завершенні передачі
ISR(USART_TX_vect) { 
  PORTD &= ~(1 << PD2); // встановлюю режим прийому
}

int writeDataToMaster() {
  message[0] = MESSAGE_LENGTH;
  sht.read();
  float_to_byte_array(sht.getTemperature(), message, 1);
  float_to_byte_array(sht.getHumidity(), message, 5);

  unsigned short checkSumCRC = crc16_dect_x(message, MESSAGE_LENGTH - 2);
  byte firstByteOfCheckSum = (checkSumCRC >> 8) & 0xFF;
  byte secondByteOfCheckSum = checkSumCRC & 0xFF;
  message[MESSAGE_LENGTH - 2] = firstByteOfCheckSum;
  message[MESSAGE_LENGTH - 1] = secondByteOfCheckSum;


// 1) без спотворень 
// +2) 7й розряд найстаршого байту даних Slave1 
// 3) 0й розряд наймолодшого байту даних Slave2 
// 4) 6й та 7й розряд 4го байту даних Slave2 
// +5) 1, 2, 4 розряди 9го байту даних Slave1
  for(int k = 0; k < 5; k++){
    for (int i = 0; i < MESSAGE_LENGTH; i++) {
        byte byteToSend = message[i];
        if (k == 1 && i == MESSAGE_LENGTH - 3) {
          byteToSend ^= (1 << 6);
        } else if (k == 4 && i == 8) {
          byteToSend ^= ((1 << 0) | (1 << 1) | (1 << 3));
        }
        Serial.write(byteToSend);
    }
  }
}

void setup() {
  delay(500);
  // En_slave - на вихід + низький рівень
  DDRD = 0b00000111;
  PORTD = 0b11111000;

  // initialize UART0
  Serial.begin(14400, SERIAL_8N1);
  UCSR0B |= (1 << UCSZ02) | (1 << TXCIE0);
  UCSR0A |= (1 << MPCM0); // мультипроцесорний режим
  
  sht.begin();

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

/*
The crc16_dect_x function takes a pointer to an array of bytes 
and the length of the array, and returns the CRC-16/DECT-X value 
as a uint16_t. The polynomial used is 0x589, the initial value is 
0x0000, and the XOR output value is 0x0000. The RefIn and RefOut 
parameters are both set to false, which means that the input and 
output data are not reflected.
*/
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

void float_to_byte_array(float num, byte *byte_array, byte array_start_position) {
    memcpy(byte_array + array_start_position, &num, sizeof(float));
}
