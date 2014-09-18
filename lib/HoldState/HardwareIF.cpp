#include "HardwareIF.h"

HardwareIF::HardwareIF() {}

char HardwareIF::EEPROM_read(int address) const {
  return 0;
}

void HardwareIF::EEPROM_write(int address, char val)  {}
int HardwareIF::EEPROM_max_size() const {
  return 0;
}

void HardwareIF::LCD_msg(unsigned char msg_num)  {

}
