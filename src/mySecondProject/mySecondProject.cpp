#include <Arduino.h>
#include <bcconfig.h>
#include <BigNumber.h>
#include <number.h>
#include <HoldState.h>
#include <BigNumber.h>
#include <LCDMessages.h>
#include "LiquidCrystal.h"
#include "RealHardware.h"



void setup() {

  BigNumber::begin();  // initialize library
  RealHardware hw;
  HoldState hold;



	//Serial.begin(115200);
  //hold.init(&hw);
  hold.init(&hw);


}

void loop() {



}

