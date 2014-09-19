#ifndef _HOLD_STATE_
#define _HOLD_STATE_

#include "HardwareIF.h"
#include "../BigNumber/number.h"

class HoldState {

  public:

    HoldState();
    void init(HardwareIF* hardware);
    void setPrivateKey(unsigned short key_length, char* n_ptr);
    bc_num getPrivateKey();
    int getState();

    // state changes...prob should not be public...
    void _waiting();
    void _no_private_key();
    void _power_off();
    void _on_encrypted_msg_error(char* error);
    void _on_encrypted_msg(char* msg);
    void _on_private_key_error(char* error);
    void _on_private_key(unsigned short n_len, char* private_key);
    void _show_public_key();

  private:

    //  VARIABLES
    int _state;
    HardwareIF* _hardware;
    bc_num _private_key;
    bc_num _modulus;




};

#endif
