#ifndef ButtonHandler_h
#define ButtonHandler_h

#include "Arduino.h"

class ButtonHandler {
public:
  ButtonHandler(int pin);

  void update();
  bool isPressed();
  bool isDoubleClick();
  bool isTripleClick();
  bool isLongPress();

private:
  int _pin;
  int _buttonState;
  int _lastButtonState;
  unsigned long _lastDebounceTime;
  unsigned long _clickTime;
  int _clickCount;
  unsigned long _longPressDuration;
  unsigned long _debounceDelay;
};

#endif
