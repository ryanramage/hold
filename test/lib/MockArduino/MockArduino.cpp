#include <stdio.h>
#include <stdlib.h>
#include <csignal>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <vector>
#include "MockArduino.h"
#include "../../../lib/HoldState/LCDMessages.h"
#include <time.h>

#define MICRO_WAIT 1

HoldState* _handler;
volatile bool pressed = false;

void signalHandler( int signum ){
  pressed = true;
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

int MockArduino::random_seed() {
  return time(NULL);
}

void MockArduino::simulateMessage(const char* msg) {
  printf("Time to simulate\n");
}

void MockArduino::LCD_msg(unsigned char msg_num) {
  if (msg_num == MSG_WAITING) printf("Waiting\n");
  if (msg_num == MSG_NO_PRIVATE_KEY) printf("Please Load Private Key\n");
  if (msg_num == MSG_POWER_OFF) printf("Sleeping...\n");
  if (msg_num == MSG_DECRYPTION_ERROR) printf("Error Decrypting\n");
  if (msg_num == MSG_PROCESSING) printf("Processing...\n");
  if (msg_num == MSG_PRIVATE_KEY_ERROR ) printf("Private Key Problem\n");
}

void MockArduino::LCD_display_public_key(BigNumber* modulus){
  printf("public key\n");
  printf("%s\n", modulus->toString());
}

void MockArduino::LCD_display_roll(char* rolls, BigNumber* signature){
  printf("%s %s\n", rolls, signature->toString());
}
void MockArduino::LCD_display_big_num(BigNumber* signature) {
  printf("%s\n", signature->toString());
}


void MockArduino::power_off(){
  printf("HARDWARE OFF\n");
  exit(0);
}

void MockArduino::button_or_timeout(HoldState* holdstate, int timeout) {
  _handler = holdstate;
  int waited_time = 0;
  do {
    sleep(MICRO_WAIT);
    if (pressed) {
      pressed = false;
      return holdstate->_on_button();
    }
    waited_time += MICRO_WAIT;
  } while(waited_time < timeout);
  return holdstate->_on_timeout();

}

bool is_valid_packet(std::vector<char> v){
  int packet = v.size();
  if (packet < 3) return false;
  if ((char)v[0] != '#') throw 22;
  if ((char)v[packet-1] != '*') throw 22;
  return true;
}

bool valid_char(char key) {
  if (key == 'a') return true;
  if (key == 'b') return true;
  if (key == 'c') return true;
  if (key == 'd') return true;
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

     res = select(1, &readfs, NULL, NULL, &tout);
     if (pressed) {
      pressed = false;
      return holdstate->_on_button();
     }

     current_time += 1; // add 1 milli
     if (current_time > fire_timeout) return _handler->_on_timeout();

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

