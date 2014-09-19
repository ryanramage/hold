#include "HardwareIF.h"

HardwareIF::HardwareIF() {}
int HardwareIF::EEPROM_max_size() const {
  return 0;
}
char HardwareIF::EEPROM_read(int address) const {
  return 0;
}
void HardwareIF::EEPROM_write(int address, char val)  {}
void HardwareIF::LCD_msg(unsigned char msg_num)  {}
void HardwareIF::power_off(){}
void HardwareIF::button_or_timeout(HoldState* holdstate, char callback, int timeout) {}
void HardwareIF::wait_for_packet_or_button_or_timeout(HoldState* holdstate, int timeout) {}
void HardwareIF::wait_for_private_key_or_button_or_timeout(HoldState* holdstate, int timeout){}
