#ifndef BTmodule_h
#define BTmodule_h

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TimeLib.h>

const int default_rssi_threshold = 190;

class BTmodule{

    int rxPin;
    int txPin;
    int pin34Pin;

    int rssi_threshold = default_rssi_threshold;

    SoftwareSerial* BTserial;

    public:

    BTmodule(int rx, int tx, int pin34, int rssi_threshold = default_rssi_threshold);

    bool targetScan(int duration, String adresses[5]);

    int getNearestRssiValue(String inqLines, String adresses[5]);

    int getRssi(String line, String adresses[5]);


};

#endif