#include "usersideConfiguration.h"

void Configurator::letConfigure(String (&adresses)[5]){
   //let the user register devices per webserver and then quit the AP if one is running
  Serial.println("\nconfiguration mode started\n");
  
  setupConfigurableness();
  while(!handleConfigurableness(adresses)) yield(); //let background processes do their thing
  myServer.quitAP();

  //save configuration data to let it survive power cycles
  configurationSaver.save(adresses);
  
  Serial.println("\nconfiguration mode terminated\n");
}

void Configurator::setupConfigurableness(){

  myServer.startAP(); 
  myServer.begin();
}

bool Configurator::handleConfigurableness(String (&adresses)[5]){
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
