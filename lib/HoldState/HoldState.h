#ifndef _HOLD_STATE_
#define _HOLD_STATE_


#include "../BigNumber/BigNumber.h"
#include "HardwareIF.h"

class HoldState {

  public:

    HoldState();
    void init(HardwareIF* hardware);
    void setPrivateKey(BigNumber key);
    BigNumber getPrivateKey();
    int getState();

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
    BigNumber* _public_exponent;

    // state changes...
    void _waiting();
    void _no_private_key();
    void _power_off();
    void _on_encrypted_msg_error();
    void _on_encrypted_msg(char* msg);
    void _on_private_key_error();
    void _on_private_key(unsigned short modulus_length, char* modulus, unsigned short private_key_length, char* private_key);
    void _show_public_key();

    void set_private_key(unsigned short modulus_length, char* modulus, unsigned short private_key_length, char* private_key);
    void _show_rolls(char* rolls, BigNumber* signature);
    void _show_signature(BigNumber* signature);
    void _show_decrypted(BigNumber* num);
};

#endif
