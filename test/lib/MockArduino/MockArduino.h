
#ifndef _MOCK_ARDUINO_
#define _MOCK_ARDUINO_


#include "../../../lib/HoldState/HardwareIF.h"
#include "../../../lib/HoldState/HoldState.h"
#include "../../../lib/BigNumber/BigNumber.h"

class MockArduino : public HardwareIF {

  public:

    MockArduino();
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
