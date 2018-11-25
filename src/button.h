#ifndef button_h
#define button_h

#include <Arduino.h>
#include <TimeLib.h>

class Button{

    int buttonPin;

    public:

    Button(int pin) : buttonPin(pin) { pinMode(buttonPin, INPUT); }

    bool isHigh(){ return digitalRead(buttonPin); }

    bool scan(int scanDuration, int pressDuration = 0);

    bool isPressedFor(int pressDuration);

};



#endif

