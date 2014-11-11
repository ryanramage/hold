#include <Arduino.h>
#include <bcconfig.h>
#include <number.h>
#include <HoldState.h>
#include <BigNumber.h>
#include <LCDMessages.h>
#include "LiquidCrystal.h"
#include "RealHardware.h"


HoldState* hold;
RealHardware* hw;

void setup() {

  BigNumber::begin();  // initialize library
  hw = new RealHardware();
  hold = new HoldState(hw);


}

void loop() {
  //hw->LCD_text("Loop start");
  //delay(4000);
//  hw->wait(4000);
  hold->run();
  //hw->LCD_text("Loop end");
  //delay(4000);
  // hw->wait(4000);
}

