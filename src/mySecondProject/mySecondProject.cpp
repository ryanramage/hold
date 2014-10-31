#include <Arduino.h>
#include "HoldState.h"
#include "BigNumber.h"
#include "LiquidCrystal.h"
#include "RealHardware.h"

RealHardware hw;
HoldState hold;


void setup() {


	Serial.begin(115200);
  hold.init(&hw);
}

void loop() {



}

