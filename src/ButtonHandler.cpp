#include "./ButtonHandler.h"

ButtonHandler::ButtonHandler(int pin)
{
    _pin = pin;
    _buttonState = LOW;
    _lastButtonState = LOW;
    _lastDebounceTime = 0;
    _clickTime = 0;
    _clickCount = 0;
    _longPressDuration = 1000;
    _debounceDelay = 50;

    pinMode(_pin, INPUT);
}

void ButtonHandler::update()
{
    int reading = digitalRead(_pin);
    if (reading != _lastButtonState)
    {
        _lastDebounceTime = millis();
    }

    if ((millis() - _lastDebounceTime) > _debounceDelay)
    {
        if (reading != _buttonState)
        {
            _buttonState = reading;

            if (_buttonState == HIGH)
            {
                _clickCount++;
                _clickTime = millis();
            }
        }
    }
    if (_clickCount > 0)
    {
        if (_clickCount == 1 && (millis() - _clickTime) > _longPressDuration)
        {
            _clickCount = 0; // Reset click count
        }
        else if (_clickCount == 2)
        {
            _clickCount = 0; // Reset click count
        }
        else if (_clickCount == 3)
        {
            _clickCount = 0; // Reset click count
        }
    }

    _lastButtonState = reading;
}

bool ButtonHandler::isPressed()
{
    return _buttonState == HIGH;
}

bool ButtonHandler::isDoubleClick()
{
    return _clickCount == 2;
}

bool ButtonHandler::isTripleClick()
{
    return _clickCount == 3;
}

bool ButtonHandler::isLongPress()
{
    return (_clickCount == 1) && ((millis() - _clickTime) > _longPressDuration);
}
