#ifndef _HARDWARE_IF_
#define _HARDWARE_IF_


class HardwareIF {

  public:
    HardwareIF();
    virtual char EEPROM_read(int address) const;
    virtual void EEPROM_write(int address, char val);
    virtual int EEPROM_max_size() const;
    virtual void LCD_msg(unsigned char msg_num);
};

#endif
