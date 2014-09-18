#include <stdio.h>
#include "MockArduino.h"
#include "LCDMessages.h"

MockArduino::MockArduino() {
  for (int i = 0; i < 1024; i++){
    _eeprom[i] = 0;
  }
}
char MockArduino::EEPROM_read(int address) const {
  return _eeprom[address];
}

void MockArduino::EEPROM_write(int address, char val) {
  _eeprom[address] = val;
}

int  MockArduino::EEPROM_max_size() const {
  return 512;
}

void MockArduino::simulateMessage(const char* msg) {

}

void MockArduino::LCD_msg(unsigned char msg_num) {
  if (msg_num == MSG_WAITING) printf("Waiting\n");
  if (msg_num == MSG_NO_PRIVATE_KEY) printf("Please Load Private Key\n");
}

