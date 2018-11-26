#ifndef usersideConfiguration_h
#define usersideConfiguration_h

#include <Arduino.h>
#include <MyWebserver.h>
#include <configurationSaver.h>

class Configurator{

    MyWebserver myServer;
    ConfigurationSaver configurationSaver;

    public:

    void letConfigure(String (&adresses)[5]);

    void save();
    void retrieve(String (&adresses)[5]) {configurationSaver.retrieve(adresses); }
    

    private:
        
    void setupConfigurableness();

    bool handleConfigurableness(String (&adresses)[5]);

};


#endif