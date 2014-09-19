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

  private:

    //  VARIABLES
    int _state;
    HardwareIF* _hardware;
    BigNumber* _private_key;
    BigNumber* _modulus;

    // state changes...
    void _waiting();
    void _no_private_key();
    void _power_off();
    void _on_encrypted_msg_error(char* error);
    void _on_encrypted_msg(char* msg);
    void _on_private_key_error(char* error);
    void _on_private_key(unsigned short n_len, char* private_key);
    void _show_public_key();


};

#endif
