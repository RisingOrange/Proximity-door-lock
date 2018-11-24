//my bluetooth adress A086:C6:8F4506

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TimeLib.h>
#include <MyWebserver.h>
#include "rssi.h"

//software serial for communicating with the bluetooth module
SoftwareSerial BTserial(13, 15); // RX | TX

//the array of adresses that can be configured and the server that allows it
String adresses[5];
MyWebserver myServer;


int button = 14; //D5
int pin34 = 12; //D6
int relay = 16; //D0


void setupBT(){
  pinMode(button, INPUT);
  pinMode(pin34, OUTPUT);

  //waiting 2 seconds before powering pin34 of the HC05 prevents it from entering the permanent AT mode
  //during which inquiring does not work
  
  digitalWrite(pin34, LOW);
  delay(2000);
  digitalWrite(pin34, HIGH);
  
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


void handleConfigurableness(){
   
  myServer.handleClient();

  //print the elements of the adresses if it changed
  String adresses_cp[sizeof(adresses)/sizeof(String)];
  
  for(int i=0; i<sizeof(adresses)/sizeof(String); i++){
    adresses_cp[i] = adresses[i];
  }

  myServer.updateArray(adresses);

  //check if adresses was changed
  bool adressesWereChanged = false;
  for(int i = 0; i<sizeof(adresses)/sizeof(String); i++){
    if (adresses[i] != adresses_cp[i]){
      adressesWereChanged = true;
      break;
    }
  }

  //print the adresses if it they were changed
  if (adressesWereChanged){
    Serial.println("updated adresses: ");
    for(int i=0; i<sizeof(adresses)/sizeof(String); i++) Serial.println(adresses[i]);
    Serial.println();
    Serial.println();
  }
  
}


void setup(){
  Serial.begin(9600);

  setupConfigurableness();
  setupBT();

  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
  
}

void loop(){
  
  bool targetNear = false;
  if(digitalRead(button) == HIGH){
    targetNear = targetScan(5, BTserial, adresses);
  }
  

  //the compiler produced segfault errors when the Serial.print(ln) statement was inside the curly braces 
  //following the if statement with button-input above
  if(targetNear){
    Serial.println("target near");
    Serial.println("opening");
    digitalWrite(relay, HIGH);
    delay(1000);
    digitalWrite(relay, LOW);
    Serial.println();
  }
  
  /*  
  while (BTserial.available()){
   Serial.write(BTserial.read());
  }

  // Keep reading from Arduino Serial Monitor and send
  if (Serial.available()) {
    BTserial.write(Serial.read());
  }
  */
  
  handleConfigurableness();
}

