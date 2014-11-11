#include "HardwareIF.h"

HardwareIF::HardwareIF() {}
int HardwareIF::EEPROM_max_size() const {
  return 0;
}
char HardwareIF::EEPROM_read(int address) const {
  return 0;
}
void HardwareIF::EEPROM_write(int address, char val)  {}
void HardwareIF::LCD_text(char* text) {}
void HardwareIF::LCD_msg(unsigned char msg_num)  {}
void HardwareIF::LCD_display_public_key(BigNumber* modulus)  {}
void HardwareIF::LCD_display_roll(char* rolls, BigNumber* signature){}
void HardwareIF::LCD_display_big_num(BigNumber* signature){}
void HardwareIF::power_off(){}
void HardwareIF::wait(unsigned int milliseconds){}
void HardwareIF::button_or_timeout(HoldState* holdstate, int timeout) {}
void HardwareIF::wait_for_packet_or_button_or_timeout(HoldState* holdstate, int timeout) {}
int  HardwareIF::random_seed(){
  return 0;
}