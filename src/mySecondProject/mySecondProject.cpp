#include <Arduino.h>
#include "HoldState.h"
#include "BigNumber.h"
#include "RealHardware.h"
#include <../../../../libraries/LiquidCrystal/LiquidCrystal.h>

RealHardware hw;
HoldState hold;


void setup() {


	Serial.begin(115200);
  hold.init(&hw);
}

void loop() {



}

