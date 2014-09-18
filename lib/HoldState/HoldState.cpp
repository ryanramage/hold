#include "HoldState.h"
#include "HardwareIF.h"
#include "LCDMessages.h"
#include <stdlib.h>
#include "../BigNumber/number.h"

#define MODULUS "3594340021"

#define EEPROM_BASE_ADDRESS 0
#define PRIVATE_KEY_SET  3

#define STATE_NO_PRIVATE_KEY 10
#define STATE_PROCESS_PRIVATE_KEY 11
#define STATE_PRIVATE_KEY_ERROR 12
#define STATE_WAITING 13
#define STATE_DISPLAY_PUBLIC_KEY 14
#define STATE_PROCESS_MSG 15
#define STATE_MSG_ERROR 16
#define STATE_DISPLAY_DECRYPTED_CONTENT 17
#define STATE_POWER_OFF 18;

#define BETWEEN_TONES_TIMEOUT_SEC = 1
#define POWER_OFF_TIMEOUT_SEC = 20
#define SHOW_ERROR_SEC = 5
#define SHOW_LONG_MSG_SEC = 20


HoldState::HoldState() {
  _state = 0;
}

int HoldState::getState() {
  return _state;
}

void HoldState::init(HardwareIF* hardware){
  _hardware = hardware;
  char check_byte = this->_hardware->EEPROM_read(EEPROM_BASE_ADDRESS);
  if (check_byte != PRIVATE_KEY_SET) _no_private_key();
  else _waiting();
}


void HoldState::_waiting(){
  _state = STATE_WAITING;
  _hardware->LCD_msg(MSG_WAITING);
  // _hardware->wait_for_codepacket_or_button_or_timeout(
  //   _on_encrypted_msg_error,             // on error
  //   _on_encrypted_msg,                   // on codepacket
  //   _show_public_key,                    // on button
  //   _power_off, POWER_OFF_TIMEOUT_SEC    // on timeout
  // );
}

void HoldState::_no_private_key(){
  _state = STATE_NO_PRIVATE_KEY;
  _hardware->LCD_msg(MSG_NO_PRIVATE_KEY);
  // _hardware->wait_for_codepacket_or_button_or_timeout(
  //   _on_private_key_error,               // on error
  //   _on_private_key,                     // on codepacket
  //   _power_off,                          // on button
  //   _power_off, POWER_OFF_TIMEOUT_SEC);  // in timeout
}

void HoldState::_power_off(){
  _state = STATE_POWER_OFF;
  _hardware->LCD_msg(MSG_POWER_OFF);
  // _hardware->power_off();
}

void HoldState::_on_encrypted_msg_error(char* error){
  _state = STATE_MSG_ERROR;
  _hardware->LCD_msg(MSG_DECRYPTION_ERROR);
  // _hardware->button_or_timeout(_waiting, SHOW_ERROR_SEC);
}

void HoldState::_on_encrypted_msg(char* msg) {
  _state = STATE_PROCESS_MSG;
  _hardware->LCD_msg(MSG_PROCESSING);

  bc_num temp;
  temp = (bc_num) malloc (sizeof(bc_struct));

  // bc_num ct = bc_str2num(temp, msg, 0);
  // bc_num msg = bc_raisemod(ct, get_private_key(_hardware), "3594340021")
  // _hardware->LCD_show_msg(bc_num);
  // _hardware->button_or_timeout(_waiting, SHOW_LONG_MSG_SEC);
}


void HoldState::_on_private_key_error(char* error){
  _state = STATE_PRIVATE_KEY_ERROR;
  _hardware->LCD_msg(MSG_PRIVATE_KEY_ERROR);
  // _hardware->button_or_timeout(_no_private_key, SHOW_ERROR_SEC);
}

void HoldState::_on_private_key(unsigned short n_len, char* private_key) {
  // set_private_key(_hardware, n_len, private_key);

}


void HoldState::_show_public_key(){
  _state = STATE_DISPLAY_PUBLIC_KEY;
  // _hardware->LCD_display_public_key(bc_num key);
  // _hardware->button_or_timeout(_waiting, SHOW_LONG_MSG_SEC);
}

void set_private_key(HardwareIF* hardware, unsigned short n_len, char* n_ptr) {
  int MAX_SIZE = hardware->EEPROM_max_size();
  int cur_eeprom_address = 0; // make sure we are at the base
  hardware->EEPROM_write(cur_eeprom_address++, PRIVATE_KEY_SET);

  // we should check hardware max size
  if ((n_len + 3) > MAX_SIZE) return;

  // write the n_len int as two bytes
  unsigned char * p_int = (unsigned char *)&n_len;
  hardware->EEPROM_write(cur_eeprom_address++, p_int[0]);
  hardware->EEPROM_write(cur_eeprom_address++, p_int[1]);

  int current_key_char;
  for (current_key_char = 0; current_key_char < n_len; current_key_char++){
    hardware->EEPROM_write(cur_eeprom_address++, n_ptr[current_key_char]);
  }

}


bc_num get_private_key(HardwareIF* hardware) {

  int cur_eeprom_address = 1; // start at the bytes for the n_len

  bc_num temp;

  temp = (bc_num) malloc (sizeof(bc_struct));
  temp->n_sign = PLUS;
  temp->n_refs = 1;
  temp->n_scale = 0;

  // read the n_len int as two bytes

  unsigned char *p_int = (unsigned char *)&temp->n_len;
  p_int[0] = hardware->EEPROM_read(cur_eeprom_address++);
  p_int[1] = hardware->EEPROM_read(cur_eeprom_address++);

  temp->n_ptr = (char *) malloc (temp->n_len);
  int current_key_char;

  for (current_key_char = 0; current_key_char < temp->n_len; current_key_char++){
    temp->n_ptr[current_key_char] = hardware->EEPROM_read(cur_eeprom_address++);
  }
  temp->n_value = temp->n_ptr;

  return temp;
}




