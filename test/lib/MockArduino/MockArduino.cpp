#include <stdio.h>
#include <stdlib.h>
#include <csignal>
#include <unistd.h>
#include "MockArduino.h"
#include "LCDMessages.h"

HoldState* _handler;
volatile int pending = 0;

void signalHandler( int signum ){
  pending = 0;
  _handler->_on_button();
}


MockArduino::MockArduino() {
  for (int i = 0; i < 1024; i++){
    _eeprom[i] = 0;
  }
  signal(SIGINT, signalHandler);
}

char MockArduino::EEPROM_read(int address) const {
  return _eeprom[address];
}

void MockArduino::EEPROM_write(int address, char val) {
  _eeprom[address] = val;
}

int  MockArduino::EEPROM_max_size() const {
  return 512;
}

void MockArduino::simulateMessage(const char* msg) {

}

void MockArduino::LCD_msg(unsigned char msg_num) {
  if (msg_num == MSG_WAITING) printf("Waiting\n");
  if (msg_num == MSG_NO_PRIVATE_KEY) printf("Please Load Private Key\n");
  if (msg_num == MSG_POWER_OFF) printf("Sleeping...\n");
  if (msg_num == MSG_DECRYPTION_ERROR) printf("Error Decrypting\n");
  if (msg_num == MSG_PROCESSING) printf("Processing...\n");
  if (msg_num == MSG_PRIVATE_KEY_ERROR ) printf("Private Key Problem\n");
}


void MockArduino::power_off(){
  printf("HARDWARE OFF\n");
  exit(0);
}

void MockArduino::button_or_timeout(HoldState* holdstate, int timeout) {
  _handler = holdstate;
  pending = 1;
  sleep(timeout);
  if (pending == 1) holdstate->_on_timeout();
}

void MockArduino::wait_for_packet_or_button_or_timeout(HoldState* holdstate, int timeout) {
  _handler = holdstate;
  pending = 1;
  sleep(timeout );
  if (pending == 1) holdstate->_on_timeout();
}

