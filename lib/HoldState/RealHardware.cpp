#include <Arduino.h>
// http://forum.arduino.cc/index.php/topic,49250.0.html
#include <../../../../libraries/EEPROM/EEPROM.h>
//#include <../../../../libraries/LiquidCrystal/LiquidCrystal.h>
#include "./RealHardware.h"
#include "./LCDMessages.h"

// power control pins
#define BUTTON_PIN   2 // interrupt pin. Maps to interrupt 0 on arduino.
#define SHUTDOWN_PIN 6

#define UNCONNECTED_PIN A5

#define HT9170B_PIN_DV 3  // interrupt pin. Maps to interrupt 1 on arduino.
// use analog pins to free up some pins
#define HT9170B_PIN_D3 A3 // high bit
#define HT9170B_PIN_D2 A2
#define HT9170B_PIN_D1 A1
#define HT9170B_PIN_D0 A0 // low bit

#define HT9170B_PIN_OE 7 // writing low will disable DMTF output

// LCD pins
#define LCD_D4_PIN 8
#define LCD_D5_PIN 9
#define LCD_D6_PIN 10
#define LCD_D7_PIN 11
#define LCD_E_PIN  12
#define LCD_RS_PIN 13

#define PACKET_TIMEOUT_MS 3000
#define MAX_PACKET_SIZE  300

const char* LCD_WAITING_MSG = "Waiting...";
const char* LCD_NO_PRIVATE_KEY = "No Private Key";
const char* LCD_POWERING_OFF = "Powering Off";
const char* LCD_DECRYPTION_ERROR = "Decryption Error";
const char* LCD_PROCESSING = "Processing";
const char* LCD_PRIVATE_KEY_ERROR = "Private Key Error";


//LiquidCrystal lcd(LCD_RS_PIN, LCD_E_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

volatile int button_pressed = 0;
volatile int tone_received = 0;
volatile byte last_tone;

void on_button(){
  // read the button. might not have to because we got called on rise
  button_pressed = 1;
}

void on_tone(){
  tone_received = 1;
  last_tone = PORTC; // http://www.arduino.cc/en/Reference/PortManipulation
}

RealHardware::RealHardware() {


  pinMode(SHUTDOWN_PIN, OUTPUT);
  digitalWrite(SHUTDOWN_PIN, HIGH); // keep it latched

  pinMode(BUTTON_PIN, INPUT);

  pinMode(HT9170B_PIN_DV, INPUT);
  pinMode(HT9170B_PIN_OE, OUTPUT);
  pinMode(HT9170B_PIN_D0, INPUT);
  pinMode(HT9170B_PIN_D1, INPUT);
  pinMode(HT9170B_PIN_D2, INPUT);
  pinMode(HT9170B_PIN_D3, INPUT);

  attachInterrupt(0, on_button, RISING);
  attachInterrupt(1, on_tone, RISING);
  //Serial.begin(9600);
  //lcd.begin(16, 2);
}

char RealHardware::EEPROM_read(int address) const {
  return EEPROM.read(address);
}

void RealHardware::EEPROM_write(int address, char val) {
  EEPROM.write(address, val);
}

int  RealHardware::EEPROM_max_size() const {
  //assume uno/ATmega328 chips
  return 1024;
}


void print_everywhere(const char* msg) {
  Serial.println(msg);
  //lcd.print(msg);
}

void RealHardware::LCD_msg(unsigned char msg_num) {
  if (msg_num == MSG_WAITING)            print_everywhere(LCD_WAITING_MSG);
  if (msg_num == MSG_NO_PRIVATE_KEY)     print_everywhere(LCD_NO_PRIVATE_KEY);
  if (msg_num == MSG_POWER_OFF)          print_everywhere(LCD_POWERING_OFF);
  if (msg_num == MSG_DECRYPTION_ERROR)   print_everywhere(LCD_DECRYPTION_ERROR);
  if (msg_num == MSG_PROCESSING)         print_everywhere(LCD_PROCESSING);
  if (msg_num == MSG_PRIVATE_KEY_ERROR ) print_everywhere(LCD_PRIVATE_KEY_ERROR);
}

void RealHardware::LCD_display_public_key(BigNumber* modulus) {

}

void RealHardware::LCD_display_big_num(BigNumber* modulus){

}

void RealHardware::LCD_display_roll(char* rolls, BigNumber* signature){

}


void RealHardware::power_off(){
  // this unlatches the latch circuit
  digitalWrite(SHUTDOWN_PIN, LOW);
}

void RealHardware::button_or_timeout(HoldState* holdstate, int timeout) {
  unsigned int timeout_ends = millis() + timeout;

  do {
    delay(200);
    if (button_pressed == 1) holdstate->_on_button();
  } while(millis() < timeout_ends);
  holdstate->_on_timeout();
}

char what_char(byte tone){
  byte valid_pins = B00001111 | tone;
  switch (valid_pins){
    case B00000001: return '1';
    case B00000010: return '2';
    case B00000011: return '3';
    case B00000100: return '4';
    case B00000101: return '5';
    case B00000110: return '6';
    case B00000111: return '7';
    case B00001000: return '8';
    case B00001001: return '9';
    case B00001010: return '0';
    case B00001011: return '*';
    case B00001100: return '#';
    case B00001101: return 'a';
    case B00001110: return 'b';
    case B00001111: return 'c';
    case B00000000: return 'd';
  }
  return 'E';
}


bool is_star(byte last_tone) {
  return (what_char(last_tone) == '*') ;
}



void RealHardware::wait_for_packet_or_button_or_timeout(HoldState* holdstate, int timeout) {
  unsigned int timeout_ends = millis() + timeout;
  int last_packet = 0;
  char packet[MAX_PACKET_SIZE] = {};


  // wait for first tone, button, or timeout
  do {
    delay(20);
    if (button_pressed == 1) return holdstate->_on_button();
  } while(millis() < timeout_ends || tone_received == 1);
  if (tone_received == 0) return holdstate->_on_timeout();

  // do tone processing
  // outer do is to have a timeout, or * as end of packet
  do {
    // we have just got a tone
    packet[last_packet++] = what_char(last_tone);
    timeout_ends = millis() + PACKET_TIMEOUT_MS; // MAX seconds between TONES
    // clear dsa

    // inner do is super short so we catch the result of the interrupt
    do {
      delay(20);
    } while(millis() < timeout_ends || tone_received == 1);

  } while(millis() < timeout_ends || is_star(last_tone) );

  if (millis() < timeout_ends) return holdstate->_on_error();

  packet[last_packet++] = what_char(last_tone);

  //holdstate->_on_packet(*packet);
}

int RealHardware::random_seed(){
  return analogRead(UNCONNECTED_PIN);
}


