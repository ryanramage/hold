#include <stdio.h>
#include "../lib/HoldState/HoldState.h"
#include "lib/MockArduino/MockArduino.h"

MockArduino hw;
HoldState state;
int s;

int main(){
  hw = MockArduino();
  state = HoldState();
  state.init(&hw);
  s = state.getState();
  //printf("%d\n", s);
  hw.simulateMessage("#2#3232*"); // sign 3232

}
