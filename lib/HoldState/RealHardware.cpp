#include <Arduino.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <../LiquidCrystal/LiquidCrystal.h>
#include "./RealHardware.h"
#include "./LCDMessages.h"
// http://forum.arduino.cc/index.php/topic,49250.0.html


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




// from http://www.arduino.cc/en/Reference/PROGMEM
// and  http://www.nongnu.org/avr-libc/user-manual/pgmspace.html
prog_char const string_0[] PROGMEM =  "Waiting...";
prog_char const string_1[] PROGMEM =  "No Private Key";
prog_char const string_2[] PROGMEM =  "Powering Off";
prog_char const string_3[] PROGMEM =  "Decryption Err";
prog_char const string_4[] PROGMEM =  "Processing";
prog_char const string_5[] PROGMEM =  "Private Key Err";
prog_char const string_6[] PROGMEM =  "Receiving...";

PGM_P const messages[] PROGMEM  =
{
  string_0,
  string_1,
  string_2,
  string_3,
  string_4,
  string_5,
  string_6
};


LiquidCrystal lcd(LCD_RS_PIN, LCD_E_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

volatile bool button_pressed = false;
volatile bool tone_received = false;

volatile uint8_t PIN_D0_VAL = 0x0;
volatile uint8_t PIN_D1_VAL = 0x0;
volatile uint8_t PIN_D2_VAL = 0x0;
volatile uint8_t PIN_D3_VAL = 0x0;

void on_button(){
  // read the button. might not have to because we got called on rise
  button_pressed = true;
}

void on_tone(){
  tone_received = true;
  //last_tone = PORTC; // http://www.arduino.cc/en/Reference/PortManipulation
  PIN_D0_VAL = digitalRead(HT9170B_PIN_D0);
  PIN_D1_VAL = digitalRead(HT9170B_PIN_D1);
  PIN_D2_VAL = digitalRead(HT9170B_PIN_D2);
  PIN_D3_VAL = digitalRead(HT9170B_PIN_D3);

}

RealHardware::RealHardware() {


  pinMode(SHUTDOWN_PIN, OUTPUT);
  digitalWrite(SHUTDOWN_PIN, HIGH); // keep it latched

  //pinMode(BUTTON_PIN, INPUT);

  pinMode(HT9170B_PIN_DV, INPUT);
  pinMode(HT9170B_PIN_OE, OUTPUT);
  pinMode(HT9170B_PIN_D0, INPUT);
  pinMode(HT9170B_PIN_D1, INPUT);
  pinMode(HT9170B_PIN_D2, INPUT);
  pinMode(HT9170B_PIN_D3, INPUT);

  //attachInterrupt(0, on_button, RISING);
  attachInterrupt(1, on_tone, RISING);
  //digitalWrite(HT9170B_PIN_OE, LOW);
  lcd.begin(16, 2);
  digitalWrite(HT9170B_PIN_OE, HIGH);
}

char RealHardware::EEPROM_read(int address) const {
  return eeprom_read_byte((unsigned char *) address);
  //return EEPROM.read(address);
}

void RealHardware::EEPROM_write(int address, char val) {
  //EEPROM.write(address, val);
  eeprom_write_byte((unsigned char *) address, val);
}

int  RealHardware::EEPROM_max_size() const {
  //assume uno/ATmega328 chips
  return 1024;
}


void print_everywhere(const char* msg) {
  lcd.clear();
  lcd.print(msg);
}

void RealHardware::LCD_text(char* text) {
  lcd.clear();
  lcd.print(text);
}

void RealHardware::LCD_msg(unsigned char msg_num) {
  if (msg_num > 7) return; // guard
  char buffer[16];
  strcpy_P(buffer, (PGM_P)pgm_read_word(&(messages[msg_num])));
  lcd.clear();
  lcd.print(buffer);
}

void print_receiving(int digit){
  if (digit == 0) {
    char buffer[16];
    strcpy_P(buffer, (PGM_P)pgm_read_word(&(messages[6])));
    lcd.clear();
    lcd.print(buffer);
  }
  else {
    lcd.setCursor(0, 1);
    lcd.print(digit);
  }
}

void RealHardware::LCD_display_public_key(BigNumber* modulus) {
  char* pk = modulus->toString();
  print_everywhere(pk);
}

void RealHardware::LCD_display_big_num(BigNumber* modulus){

  char* pk = modulus->toString();
  print_everywhere(pk);
}

void RealHardware::LCD_display_roll(char* rolls, BigNumber* signature){
  char* pk = signature->toString();
  lcd.clear();
  lcd.print(rolls);
  lcd.setCursor(0, 1);
  lcd.print(pk);
}


void RealHardware::power_off(){
  // this unlatches the latch circuit
  digitalWrite(SHUTDOWN_PIN, LOW);
}

void RealHardware::button_or_timeout(HoldState* holdstate, int timeout) {
  unsigned int timeout_ends = millis() + (timeout * 1000);

  do {
    delay(200);
    //if (button_pressed == 1) holdstate->_on_button();
  } while(millis() < timeout_ends);
  holdstate->_on_timeout();
}

char what_char(){
  if (PIN_D3_VAL == LOW  && PIN_D2_VAL == LOW  && PIN_D1_VAL == LOW  && PIN_D0_VAL == HIGH) return '1';
  if (PIN_D3_VAL == LOW  && PIN_D2_VAL == LOW  && PIN_D1_VAL == HIGH && PIN_D0_VAL == LOW ) return '2';
  if (PIN_D3_VAL == LOW  && PIN_D2_VAL == LOW  && PIN_D1_VAL == HIGH && PIN_D0_VAL == HIGH) return '3';
  if (PIN_D3_VAL == LOW  && PIN_D2_VAL == HIGH && PIN_D1_VAL == LOW  && PIN_D0_VAL == LOW ) return '4';
  if (PIN_D3_VAL == LOW  && PIN_D2_VAL == HIGH && PIN_D1_VAL == LOW  && PIN_D0_VAL == HIGH) return '5';
  if (PIN_D3_VAL == LOW  && PIN_D2_VAL == HIGH && PIN_D1_VAL == HIGH && PIN_D0_VAL == LOW ) return '6';
  if (PIN_D3_VAL == LOW  && PIN_D2_VAL == HIGH && PIN_D1_VAL == HIGH && PIN_D0_VAL == HIGH) return '7';
  if (PIN_D3_VAL == HIGH && PIN_D2_VAL == LOW  && PIN_D1_VAL == LOW  && PIN_D0_VAL == LOW ) return '8';
  if (PIN_D3_VAL == HIGH && PIN_D2_VAL == LOW  && PIN_D1_VAL == LOW  && PIN_D0_VAL == HIGH) return '9';
  if (PIN_D3_VAL == HIGH && PIN_D2_VAL == LOW  && PIN_D1_VAL == HIGH && PIN_D0_VAL == LOW ) return '0';
  if (PIN_D3_VAL == HIGH && PIN_D2_VAL == LOW  && PIN_D1_VAL == HIGH && PIN_D0_VAL == HIGH) return '*';
  if (PIN_D3_VAL == HIGH && PIN_D2_VAL == HIGH && PIN_D1_VAL == LOW  && PIN_D0_VAL == LOW ) return '#';
  if (PIN_D3_VAL == HIGH && PIN_D2_VAL == HIGH && PIN_D1_VAL == LOW  && PIN_D0_VAL == HIGH) return 'a';
  if (PIN_D3_VAL == HIGH && PIN_D2_VAL == HIGH && PIN_D1_VAL == HIGH && PIN_D0_VAL == LOW ) return 'b';
  if (PIN_D3_VAL == HIGH && PIN_D2_VAL == HIGH && PIN_D1_VAL == HIGH && PIN_D0_VAL == HIGH) return 'c';
  if (PIN_D3_VAL == LOW  && PIN_D2_VAL == LOW  && PIN_D1_VAL == LOW  && PIN_D0_VAL == LOW ) return 'd';
  return 'e';
}


bool is_star(char last_char) {
  return (last_char == '*');
}



void RealHardware::wait_for_packet_or_button_or_timeout(HoldState* holdstate, int timeout) {
  unsigned int timeout_ends = millis() + (timeout * 1000);
  int last_packet = 0;
  char packet[MAX_PACKET_SIZE] = {};

  // // wait for first tone, button, or timeout
  do {
    delay(20);
    if (button_pressed == true) return holdstate->_on_button();
  } while(millis() < timeout_ends && tone_received == false);
  if (tone_received == false) return holdstate->_on_timeout();

  print_receiving(0);
  // // outer do is to have a timeout, or * as end of packet
  bool timed_out = false;
  char tone;
  do {
    // we have just got a tone
    tone = what_char();

    packet[last_packet++] = tone;
    unsigned int inner_timeout_ends = millis() + PACKET_TIMEOUT_MS; // MAX seconds between TONES
    tone_received = false;
    print_receiving(last_packet);
    // clear dsa

    // inner do is super short so we catch the result of the interrupt
     do {
       delay(20);
     } while(millis() < inner_timeout_ends && tone_received == 0  && tone != '*');

     if (millis() >= inner_timeout_ends  && tone != '*') timed_out = true;

  } while(!timed_out && tone != '*');

  if (timed_out) {
    return holdstate->_on_error();
  }
  //packet[last_packet++] = '/0';
  tone_received = false;
  lcd.clear();
  lcd.print(packet);
  holdstate->_on_packet(packet);
}

int RealHardware::random_seed(){
  return analogRead(UNCONNECTED_PIN);
}


