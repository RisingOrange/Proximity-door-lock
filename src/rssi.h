#ifndef rssi_h
#define rssi_h

#include <Arduino.h>
#include <TimeLib.h>
#include <SoftwareSerial.h>

#define RSSI_TRESHOLD 190;


bool targetScan(int duration, SoftwareSerial& serialObj, String adresses[5]);

int getNearestRssiValue(String inqLines, SoftwareSerial& BTserial, String adresses[5]);

int getRssi(String line, SoftwareSerial& BTserial, String adresses[5]);


#endif