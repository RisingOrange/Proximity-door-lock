//my bluetooth adress A086:C6:8F4506

#include <Arduino.h>
#include "button.h"
#include "BTmodule.h"
#include "usersideConfiguration.h"

int buttonPin = 14; //D5
int rxPin = 13;
int txPin = 15;
int pin34Pin = 12; //D6
int relayPin = 16; //D0

//the array of adresses and the configurator object for it
String adresses[5];
Configurator configurator;

BTmodule btModule(rxPin, txPin, pin34Pin);

Button button(buttonPin);


void setup(){

  Serial.begin(9600);

  //to be sure that pin 34 is low the first two seconds, this is needed to make the bluetooth module enter
  //the right mode
  pinMode(pin34Pin, OUTPUT);
  digitalWrite(pin34Pin, LOW);
  delay(2000);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  
  if(button.scan(5, 1)) configurator.letConfigure(adresses);
  else configurator.retrieve(adresses);

}

void loop(){
  
  
  bool targetNear = false;
  if(button.isHigh()){
    targetNear = btModule.targetScan(5, adresses);
  }
  

  //the compiler produced segfault errors when the Serial.print(ln) statement was inside the curly braces 
  //following the if statement with button-input above, don't know why
  if(targetNear){
    Serial.println("target near");
    Serial.println("opening");
    digitalWrite(relayPin, HIGH);
    delay(1000);
    digitalWrite(relayPin, LOW);
    Serial.println();
  }
 
}


