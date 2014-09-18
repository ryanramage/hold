
#ifndef _MOCK_ARDUINO_
#define _MOCK_ARDUINO_


#include "HardwareIF.h"

class MockArduino : public HardwareIF {

  public:

    MockArduino();
    char EEPROM_read(int address) const;
    void EEPROM_write(int address, char val) ;
    int EEPROM_max_size() const;
    void simulateMessage(const char* msg);
    void LCD_msg(unsigned char msg_num);

  private:
    char _eeprom[1024];


};

#endif
