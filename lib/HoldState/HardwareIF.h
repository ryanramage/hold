#ifndef _HARDWARE_IF_
#define _HARDWARE_IF_

#include "../BigNumber/BigNumber.h"

class HoldState;

class HardwareIF {

  public:
    HardwareIF();
    virtual int  EEPROM_max_size() const;
    virtual char EEPROM_read(int address) const;
    virtual void EEPROM_write(int address, char val);
    virtual void LCD_text(char* text);
    virtual void LCD_msg(unsigned char msg_num);
    virtual void LCD_display_public_key(BigNumber* modulus);
    virtual void LCD_display_roll(char* rolls, BigNumber* signature);
    virtual void LCD_display_big_num(BigNumber* signature);
    virtual void power_off();
    virtual void wait(unsigned int milliseconds);
    virtual void wait_for_packet_or_button_or_timeout(HoldState* holdstate, int timeout);
    virtual void button_or_timeout(HoldState* holdstate, int timeout);
    virtual int random_seed();

};

#endif
