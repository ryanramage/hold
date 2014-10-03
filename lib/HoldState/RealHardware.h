
#ifndef _READ_HARDWARE_
#define _READ_HARDWARE_


#include "./HardwareIF.h"
#include "./HoldState.h"
#include "../BigNumber/BigNumber.h"

class RealHardware : public HardwareIF {

  public:

    RealHardware();
    char EEPROM_read(int address) const;
    void EEPROM_write(int address, char val) ;
    int  EEPROM_max_size() const;
    void simulateMessage(const char* msg);
    void LCD_msg(unsigned char msg_num);
    void LCD_display_public_key(BigNumber* modulus);
    void LCD_display_roll(char* rolls, BigNumber* signature);
    void LCD_display_big_num(BigNumber* signature);
    virtual void button_or_timeout(HoldState* holdstate, int timeout);
    virtual void wait_for_packet_or_button_or_timeout(HoldState* holdstate, int timeout);
    virtual int random_seed();
    virtual void power_off();

  private:
    char _eeprom[1024];


};

#endif