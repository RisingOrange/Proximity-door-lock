

#ifndef MyWebserver_h
#define MyWebserver_h

#include <Arduino.h>

#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"


class MyWebserver{
  
  public:

  MyWebserver();
  
  bool connectToWifi(const char* ssid, const char* pass);

  bool begin();
  
  void handleClient();

  void updateArray(String (&arrRef) [5]);

  String local_ip;

  private:
  String theArray[5]; //this array is the thing that can be configured throught the webserver
  
  ESP8266WebServer* server;
  String site_template;
  String site;

  void handleRootPath();
  void prepareSite();
  
  void handleSubmitPath();

};

#endif