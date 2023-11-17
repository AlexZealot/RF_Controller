#include "Button.h"
#include <Arduino.h>

Button::Button(int _pin){
  PIN = _pin;
  downLeft = UPD_INTERVAL;
  pinMode(PIN, INPUT_PULLUP);
  state = false;
  lastTick = millis();
  whenPressed = 0;
}

void Button::update(unsigned long T){
  lastState = state;
  int tmp = !digitalRead(PIN);
  if (tmp) {
    if (!state) {
      downLeft -= (T - lastTick);
      if (downLeft <= 0)
        state = true;
        whenPressed = T;
    }
  } else {
    state = false;
    downLeft = UPD_INTERVAL;
  }
  lastTick = T;
}

bool  Button::isDown(){
  return state;
}

bool  Button::isPressed(){
  return (state != lastState) && state;
}

bool Button::isReleased(){
  return (state != lastState) && !state;
}

unsigned long Button::holdInterval(unsigned long T){
  if (!state) return 0;
  return T - whenPressed;
}
