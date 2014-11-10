
#include "HoldState.h"
#include "HardwareIF.h"
#include "LCDMessages.h"
#include <stdlib.h>
#include <stdio.h>

#define MODULUS "3594340021"

#define EEPROM_BASE_ADDRESS 0
#define PRIVATE_KEY_SET  3

#define STATE_NO_PRIVATE_KEY 10
#define STATE_PROCESS_PRIVATE_KEY 11
#define STATE_PRIVATE_KEY_ERROR 12
#define STATE_WAITING 13
#define STATE_DISPLAY_PUBLIC_KEY 14
#define STATE_SHOW_ROLLS 141
#define STATE_PROCESS_MSG 15
#define STATE_MSG_ERROR 16
#define STATE_DISPLAY_DECRYPTED_CONTENT 17
#define STATE_POWER_OFF 18

#define BETWEEN_TONES_TIMEOUT_SEC 1
#define POWER_OFF_TIMEOUT_SEC 20
#define SHOW_ERROR_SEC 2
#define SHOW_LONG_MSG_SEC 20

#define PACKET_PRIVATE_KEY 0
#define PACKET_ENCRYPTED 1
#define PACKET_TO_SIGN 2
#define PACKET_ROLL_D10 3
#define PACKET_CLEAR_PK 4

char* load_mod(HardwareIF* _hardware) {
  int cur_eeprom_address = 1; // start at the modulus length
  unsigned short  modulus_length = _hardware->EEPROM_read(cur_eeprom_address++);

  unsigned short current_char;
  char* modulus = (char*) malloc((modulus_length + 1) * sizeof(char));

  for (current_char = 0; current_char < modulus_length; current_char++){
    modulus[current_char] = _hardware->EEPROM_read(cur_eeprom_address++);
  }
  modulus[current_char] = '\0';
  return modulus;
}

char* load_pk(HardwareIF* _hardware) {
  int cur_eeprom_address = 1; // start at the modulus length
  unsigned short  modulus_length = _hardware->EEPROM_read(cur_eeprom_address++);
  unsigned short  private_key_length = _hardware->EEPROM_read(cur_eeprom_address++);
  unsigned short current_char;
  cur_eeprom_address += modulus_length;

  char* private_key = (char*) malloc((private_key_length + 1) * sizeof(char));
  for (current_char = 0; current_char < private_key_length; current_char++){
    private_key[current_char] = _hardware->EEPROM_read(cur_eeprom_address++);
  }

  private_key[current_char] = '\0';
  return private_key;
}



HoldState::HoldState(HardwareIF* hardware) {
  _hardware = hardware;
  _button_pressed = false;
  _timout_occured = false;
  _error_happened = false;
  _packet = NULL;
  srand(hardware->random_seed());
  char check_byte = _hardware->EEPROM_read(EEPROM_BASE_ADDRESS);
  if (check_byte != PRIVATE_KEY_SET) {
    _state = STATE_NO_PRIVATE_KEY;
    return;
  }

  BigNumber mod = BigNumber(load_mod(_hardware));
  _modulus = &mod;

  BigNumber pk = BigNumber(load_pk(_hardware));
  _private_key = &pk;

  _state = STATE_WAITING;
  return;
}

int HoldState::getState() {
  return _state;
}


void HoldState::run(){
  switch(_state) {
    case STATE_NO_PRIVATE_KEY: {

      if (_button_pressed || _timout_occured){
        _button_pressed = false; //clear flag
        _timout_occured = false; // clear flag
        _state = STATE_POWER_OFF;
        return;
      }

      if (_packet != NULL){
        return _process_packet();
      }

      return _no_private_key();
    }
    case STATE_WAITING: {

      if (_button_pressed){
        _button_pressed = false; // clear flag
        _state = STATE_DISPLAY_PUBLIC_KEY;
        return;
      }
      if (_timout_occured) {
        _timout_occured = false; // clear flag
        _state = STATE_POWER_OFF;
        return;
      }
      if (_packet != NULL){
        return _process_packet();
      }

      return _waiting();
    }
    case STATE_DISPLAY_PUBLIC_KEY: {
      if (_button_pressed || _timout_occured){
        _button_pressed = false; // clear flag
        _timout_occured = false;
        _state = STATE_WAITING;
        return;
      }
      return _show_public_key();
    }
    case STATE_POWER_OFF: return _power_off();
    case STATE_MSG_ERROR: return _on_encrypted_msg_error();
  }
}

void HoldState::_waiting(){
  _hardware->LCD_msg(MSG_WAITING);
  _hardware->wait_for_packet_or_button_or_timeout(this, POWER_OFF_TIMEOUT_SEC);
}

void HoldState::_no_private_key(){
  _hardware->LCD_msg(MSG_NO_PRIVATE_KEY);
  _hardware->wait_for_packet_or_button_or_timeout(this, POWER_OFF_TIMEOUT_SEC);
}

void HoldState::_power_off(){
  _hardware->LCD_msg(MSG_POWER_OFF);
  _hardware->power_off();
}

void HoldState::_on_encrypted_msg_error(){
  _hardware->LCD_msg(MSG_DECRYPTION_ERROR);
  _hardware->button_or_timeout(this, SHOW_ERROR_SEC);
}


/* Returns an integer in the range [0, n).
 *
 * Uses rand(), and so is affected-by/affects the same seed.
 */
int randint(int n) {
  if ((n - 1) == RAND_MAX) {
    return rand();
  } else {
    // Chop off all of the values that would cause skew...
    long end = RAND_MAX / n; // truncate skew
    //assert (end > 0L);
    end *= n;

    // ... and ignore results from rand() that fall above that limit.
    // (Worst case the loop condition should succeed 50% of the time,
    // so we can expect to bail out of this loop pretty quickly.)
    int r;
    while ((r = rand()) >= end);

    return r % n;
  }
}



void HoldState::_process_packet(){
  _hardware->LCD_msg(MSG_PROCESSING);
  int mode = _packet[1] - '0';
  char* message_ = &_packet[3];
  bool fixed = false;
  int i = 0;
  while (!fixed && i++ < 50) {
    if (message_[i] == '*') {
      message_[i] = '\0';
      fixed = true;
    }
  }
  if (!fixed) {
    _state = STATE_MSG_ERROR;
    return;
  }

  if (mode == PACKET_PRIVATE_KEY) return _process_pk_message(message_);
  if (mode == PACKET_ROLL_D10)    return _process_roll_message(message_);
  if (mode == PACKET_TO_SIGN)     return _process_sign_message(message_);
  if (mode == PACKET_ENCRYPTED)   return _process_encrypted_message(message_);
}


void HoldState::_process_pk_message(char* message){

  char* modulus = &message[0];
  char* private_key;

  unsigned short length=0;
  unsigned short modulus_length = 0;
  unsigned short private_key_length = 0;
  bool found_pk = false;

  do {
    // lengths
    length++;
    if (!found_pk) modulus_length++;
    else private_key_length++;

    if (modulus[ length ] == 'c') {
      modulus[ length ] = '\0';
      private_key = &modulus[++length];
      found_pk = true;
    }

  } while(modulus[ length ] != '*' && length < 5000);

  // replace the * with a null
  modulus[length] = '\0';

  set_private_key(modulus_length, modulus, private_key_length, private_key);

  BigNumber pk  = BigNumber(private_key);
  BigNumber mod = BigNumber(modulus);
  this->_private_key = &pk;
  this->_modulus = &mod;
  _state = STATE_DISPLAY_PUBLIC_KEY;
}

void HoldState::_process_roll_message(char* message) {

  int num = atoi(&message[0]);
  if (num < 0 || num > 40) return _on_error();

  char* rolls = (char *) malloc (num + 1);
  int i = 0;
  for (; i < num; i++){
    int r = randint(10);
    sprintf(&rolls[i],"%d",r);
  }
  rolls[i] = '\0';

  BigNumber rolls_bn = BigNumber(rolls);
  BigNumber sig = rolls_bn.powMod(*this->_private_key, *this->_modulus);

  _hardware->LCD_display_roll(rolls, &sig);
  _state = STATE_SHOW_ROLLS;

}

void HoldState::_process_sign_message(char* message){

  BigNumber to_sign = BigNumber(message);
  BigNumber sig = to_sign.powMod(*this->_private_key, *this->_modulus);
  return this->_show_signature(&sig);

}

void HoldState::_process_encrypted_message(char* message){

  BigNumber to_decrypt = BigNumber(message);
  BigNumber pt = to_decrypt.powMod(*this->_private_key, *this->_modulus);
  return this->_show_decrypted(&pt);
}



void HoldState::_on_packet(char *packet) {
  _packet = packet;
}
void HoldState::_on_button() {
  _button_pressed = true;
}
void HoldState::_on_timeout() {
  _timout_occured = true;
}
void HoldState::_on_error(){
  _error_happened = true;
}


void HoldState::_on_private_key_error(){
  _hardware->LCD_msg(MSG_PRIVATE_KEY_ERROR);
  _hardware->button_or_timeout(this, SHOW_ERROR_SEC);
}



void HoldState::_show_public_key(){
  _hardware->LCD_display_public_key(this->_modulus);
  return _hardware->button_or_timeout(this, SHOW_LONG_MSG_SEC);
}


void HoldState::_show_signature(BigNumber* signature){
  _hardware->LCD_display_big_num(signature);
  return _hardware->button_or_timeout(this, SHOW_LONG_MSG_SEC);
}

void HoldState::_show_decrypted(BigNumber* num) {
  _hardware->LCD_display_big_num(num);
  return _hardware->button_or_timeout(this, SHOW_LONG_MSG_SEC);
}


void HoldState::set_private_key(unsigned short modulus_length, char* modulus, unsigned short private_key_length, char* private_key) {
  int MAX_SIZE = this->_hardware->EEPROM_max_size();
  int cur_eeprom_address = 0; // make sure we are at the base
  this->_hardware->EEPROM_write(cur_eeprom_address++, PRIVATE_KEY_SET);

  // we should check _hardware max size
  if ((modulus_length + private_key_length + 3) > MAX_SIZE) return;

  // write the modulus_length first, then pk length
  this->_hardware->EEPROM_write(cur_eeprom_address++, modulus_length);
  this->_hardware->EEPROM_write(cur_eeprom_address++, private_key_length);

  int current_char;
  for (current_char = 0; current_char < modulus_length; current_char++){
    this->_hardware->EEPROM_write(cur_eeprom_address++, modulus[current_char]);
  }

  for (current_char = 0; current_char < private_key_length; current_char++){
    this->_hardware->EEPROM_write(cur_eeprom_address++, private_key[current_char]);
  }


}

void HoldState::clear_private_key() {
  int cur_eeprom_address = 0; // make sure we are at the base
  this->_hardware->EEPROM_write(cur_eeprom_address, 0);
}





