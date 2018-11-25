//my bluetooth adress A086:C6:8F4506

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TimeLib.h>
#include <MyWebserver.h>
#include "rssi.h"
#include "button.h"
#include "configurationSaver.h"

int buttonPin = 14; //D5
int pin34Pin = 12; //D6
int relayPin = 16; //D0

//software serial for communicating with the bluetooth moduleeee
SoftwareSerial BTserial(13, 15); // RX | TX

//the array of adresses that can be configured and the server that allows it
String adresses[5];
MyWebserver myServer;

ConfigurationSaver configurationSaver;

Button button(buttonPin);

void setupBT(){
  pinMode(pin34Pin, OUTPUT);

  //waiting 2 seconds before powering pin34 of the HC05 prevents it from entering the permanent AT mode
  //during which inquiring does not work
  digitalWrite(pin34Pin, LOW);
  delay(2000);
  digitalWrite(pin34Pin, HIGH);
  
  Serial.begin(9600);  
  Serial.println("Remember to select Both NL & CR in the serial monitor");

  BTserial.begin(9600);  

  BTserial.println("AT+CLASS=0");
  BTserial.println("AT+INQM=1,9,3");
}

void setupConfigurableness(){

  myServer.startAP(); 
  myServer.begin();
}

bool handleConfigurableness(){
  //returns true if adress was changed in this call of the function

  myServer.handleClient();

  //check if adresses were changed
  int size = sizeof(adresses)/sizeof(String);
  String adresses_cp[size];
  for(int i = 0; i<size; i++){
    adresses_cp[i] = adresses[i];
  }

  myServer.updateArray(adresses);

  
  bool adressesWereChanged = false;
  for(int i = 0; i<sizeof(adresses)/sizeof(String); i++){
    if (adresses[i] != adresses_cp[i]){
      adressesWereChanged = true;
      break;
    }
  }

  //return true and print the adresses if they were changed
  if (adressesWereChanged){
    Serial.println("updated adresses: ");
    for(int i=0; i<sizeof(adresses)/sizeof(String); i++) Serial.println(adresses[i]);
    Serial.println();
    Serial.println();
    return true;
  }
  
  return false;
}

void letConfigure(){
   //let the user register devices per webserver and then quit the AP if one is running
  Serial.println("\nconfiguration mode started\n");
  
  setupConfigurableness();
  while(!handleConfigurableness()) yield(); //let background processes do their thing
  myServer.quitAP();

  //save configuration data to let it survive power cycles
  configurationSaver.save(adresses);
  
  Serial.println("\nconfiguration mode terminated\n");
}

void setup(){
  Serial.begin(9600);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  

  setupBT();
  
  if(button.scan(5, 1)) letConfigure();
  else configurationSaver.retrieve(adresses);

}

void loop(){
  
  
  bool targetNear = false;
  if(button.isHigh()){
    targetNear = targetScan(5, BTserial, adresses);
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
  
  /*  
  //code for communicating with the HC05 per AT-commands
  while (BTserial.available()){
   Serial.write(BTserial.read());
  }

  // Keep reading from Arduino Serial Monitor and send
  if (Serial.available()) {
    BTserial.write(Serial.read());
  }
  */
 
}


