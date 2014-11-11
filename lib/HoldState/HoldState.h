#ifndef _HOLD_STATE_
#define _HOLD_STATE_


#include "../BigNumber/BigNumber.h"
#include "HardwareIF.h"

class HoldState {

  public:

    HoldState(HardwareIF* hardware);

    void setPrivateKey(BigNumber key);
    BigNumber getPrivateKey();
    int getState();

    // called by the external loop
    void run();

    // called backs from the hardware
    void _on_packet(char *packet);
    void _on_button();
    void _on_timeout();
    void _on_error();

  private:

    //  VARIABLES
    int _state;
    HardwareIF* _hardware;
    BigNumber* _private_key;
    BigNumber* _modulus;

    bool _button_pressed;
    bool _timout_occured;
    bool _error_happened;
    bool _interupt_set;
    char* _packet;
    //BigNumber* _public_exponent;

    // state changes...
    void _waiting();
    void _no_private_key();
    void _power_off();

    void set_private_key(unsigned short modulus_length, char* modulus, unsigned short private_key_length, char* private_key);
    void clear_private_key();
    void _process_packet();
    void _process_pk_message(char* message);
    void _process_roll_message(char* message);
    void _process_sign_message(char* message);
    void _process_encrypted_message(char* message);
    void _show_public_key();
    void _long_message(unsigned char msg_num);
};

#endif
