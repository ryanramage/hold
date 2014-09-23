#include <stdio.h>
#include <stdlib.h>
#include <csignal>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <vector>
#include "MockArduino.h"
#include "../../../lib/HoldState/LCDMessages.h"

HoldState* _handler;
volatile int pending = 0;

void signalHandler( int signum ){
  if (!pending) return;
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

bool is_valid_packet(std::vector<char> v){
  int packet = v.size();
  if (packet < 3) return false;
  if ((char)v[0] != '#') throw 22;
  if ((char)v[packet-1] != '*') throw 22;
  return true;
}

bool valid_char(char key) {
  if (key == 'A') return true;
  if (key == 'B') return true;
  if (key == 'C') return true;
  if (key == 'D') return true;
  if (key == '1') return true;
  if (key == '2') return true;
  if (key == '3') return true;
  if (key == '4') return true;
  if (key == '5') return true;
  if (key == '6') return true;
  if (key == '7') return true;
  if (key == '8') return true;
  if (key == '9') return true;
  if (key == '0') return true;
  if (key == '#') return true;
  if (key == '*') return true;
  return false;
}

void MockArduino::wait_for_packet_or_button_or_timeout(HoldState* holdstate, int timeout) {
  _handler = holdstate;

  std::vector<char> buffer;
  fd_set readfs;
  struct timeval tout;
  int res, key;
  int fire_timeout = timeout * 1000;
  int current_time = 0;
  while (1) {
     FD_SET(0, &readfs);  /* 0 is STDIN */
     tout.tv_usec = 1000;  /* 1 milliseconds */
     tout.tv_sec  = 0;  /* seconds */
     pending = 1;
     res = select(1, &readfs, NULL, NULL, &tout);
     pending = 0;
     current_time += 1; // add 1 milli
     if (current_time > fire_timeout) return _handler->_on_button();

     if (res) {
        /* key pressed */
        key = getchar();
        buffer.push_back(key);
        while (valid_char(key)) {
          key = getchar();
          buffer.push_back(key);
        }
        // remove the last one, as its a line feed
        buffer.pop_back();
        try {
         if (is_valid_packet(buffer)) return _handler->_on_packet(reinterpret_cast<char*> (&buffer[0]));
        } catch(...) {
          _handler->_on_error();
        }
     }

  }

}

