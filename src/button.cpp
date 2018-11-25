

#include "button.h"

bool Button::isPressedFor(int pressDuration){
  //returns true if button is pressed for pressDuration seconds,
  //blocks program flow during this time

  const int tolerance = 5; //this many consecutive times the button state can be low until the function returns false
  int ctr = 0; //interruption counter, if this counter surpasses the tolerance, the function returns false
  int startTime = now();
  int curTime = startTime;
  
  while(curTime < startTime + pressDuration){
    if (digitalRead(buttonPin) == LOW) ctr += 1;
    else ctr = 0;
    if (ctr > tolerance) return false;
    yield();
    curTime = now();
  }
  return true;
}

bool Button::scan(int scanDuration, int pressDuration){
  //returns true if the button is pressed during the next %duration seconds for a minimum of pressDuration seconds,
  //blocks program flow during this time
  
  int startTime = now();
  int curTime = startTime;

  while(curTime < startTime + scanDuration){
    if (digitalRead(buttonPin) == HIGH) {
      if (pressDuration == 0) return true;
      else {
        bool longEnough = isPressedFor(pressDuration);
        if (longEnough) return true;
      }
    }
    yield(); //let background processes do their things (the esp crashes otherwise)
    curTime = now();
  } 
  
  return false;
}