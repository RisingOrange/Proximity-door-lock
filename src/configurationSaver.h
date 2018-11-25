#ifndef configurationSaver_h
#define configurationSaver_h

#include <Arduino.h>
#include <EEPROM.h>

class ConfigurationSaver {

    public: 
    
    ConfigurationSaver(){ EEPROM.begin(512); }
    
    void save(const String (&adresses) [5]){
        //has to work jointly with retrieveConfig()
        //this function writes the adresses to eeprom (emulated eeprom on nodemcu flash)
        //it starts at adress 0, the first bit indicates the number of strings saved
        //in front of each string is a bit that indicates the number of chars in the string

        int eeAdress = 0;

        int numAdresses = sizeof(adresses)/sizeof(String);
        EEPROM.write(eeAdress++, numAdresses);
        for(int i = 0; i<numAdresses; i++){
            String curAdress = adresses[i];
            int len = curAdress.length();
            
            EEPROM.write(eeAdress++, len);
            for(int j = 0; j<len; j++){
            char ch = curAdress[j];
            EEPROM.write(eeAdress+j, ch);
            }
            eeAdress += len;
        }
        EEPROM.commit();
    }

    void retrieve(String (&adresses) [5]){
        //has to work jointly with saveConfig()
        //see there
        
        int eeAdress = 0;

        int numAdresses = EEPROM.read(eeAdress++);
        for (int i = 0; i<numAdresses; i++){
            int len = EEPROM.read(eeAdress++);
            String temp = "";
            for(int j = 0; j<len; j++){
            char ch = char(EEPROM.read(eeAdress + j));
            temp += String(ch);
            }
            eeAdress += len;
            adresses[i] = temp;
        }
    }

};


#endif