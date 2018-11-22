//my bluetooth adress A086:C6:8F4506

int RSSI_TRESHOLD = 190;

#include <ESP_SoftwareSerial.h>
#include <TimeLib.h>
#include <EEPROM.h>
#include <MyWebserver.h>


//software serial for communicating with the bluetooth module
SoftwareSerial BTserial(13, 15); // RX | TX

//the array of adresses that can be configured and the server that allows it
String adresses[5];
MyWebserver myServer;

int button = 14; //D5
int pin34 = 12; //D6
int relay = 16; //D0

void setup(){
  Serial.begin(9600);
  EEPROM.begin(512);

  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);
  
  pinMode(button, INPUT);

  setupBT();
  
  if(buttonScan(5, 1)) letConfigure();
  else retrieveConfig();

}

void loop(){
  
  
  bool targetNear = false;
  if(digitalRead(button) == HIGH){
    targetNear = targetScan(5);
  }
  

  //the compiler produced segfault errors when the Serial.print(ln) statement was inside the curly braces 
  //following the if statement with button-input above, don't know why
  if(targetNear){
    Serial.println("target near");
    Serial.println("opening");
    digitalWrite(relay, HIGH);
    delay(1000);
    digitalWrite(relay, LOW);
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

void setupBT(){
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

void letConfigure(){
   //let the user register devices per webserver and then quit the AP if one is running
  Serial.println("\nconfiguration mode started\n");
  
  setupConfigurableness();
  while(!handleConfigurableness()) yield(); //let background processes do their thing
  myServer.quitAP();

  //save configuration data to let it survive power cycles
  saveConfig();
  
  Serial.println("\nconfiguration mode terminated\n");
}

void setupConfigurableness(){

  myServer.startAP(); 
  myServer.begin();
}

bool handleConfigurableness(){
  //returns true if adress was changed in this call of the function

  myServer.handleClient();

  //check if adresses were changed
  String adresses_cp[5] = adresses;
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

void saveConfig(){
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

void retrieveConfig(){
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

bool buttonScan(int scanDuration, int pressDuration = 0){
  //returns true if the button is pressed during the next %duration seconds for a minimum of pressDuration seconds,
  //blocks program flow during this time
  
  int startTime = now();
  int curTime = startTime;

  while(curTime < startTime + scanDuration){
    if (digitalRead(button) == HIGH) {
      if (pressDuration == 0) return true;
      else {
        bool longEnough = isButtonPressedFor(pressDuration);
        if (longEnough) return true;
      }
    }
    yield(); //let background processes do their things (the esp crashes otherwise)
    curTime = now();
  } 
  
  return false;
}

bool isButtonPressedFor(int pressDuration){
  //returns true if button is pressed for pressDuration seconds,
  //blocks program flow during this time

  const int tolerance = 5; //this many consecutive times the button state can be low until the function returns false
  int ctr = 0; //interruption counter, if this counter surpasses the tolerance, the function returns false
  int startTime = now();
  int curTime = startTime;
  
  while(curTime < startTime + pressDuration){
    if (digitalRead(button) == LOW) ctr += 1;
    else ctr = 0;
    if (ctr > tolerance) return false;
    yield();
    curTime = now();
  }
  return true;
}


bool targetScan(int duration){
  /*blocks all other processes for its duration and listens for inq-data on BTserial, 
  if the device with the target adress has a high enough rssi --> return true, 
  if time runs out before this happens, return false*/

  BTserial.println("AT+INQ"); //should start inquiry mode, it is often active anyway, because it does not deactivate despite sending the INQC command
  
  int startTime = now();
  Serial.print("Scan initialized; uptime=");
  Serial.print(startTime);
  Serial.print("; duration=");
  Serial.print(duration);
  Serial.println();

  int curTime = startTime;
  
  while(curTime < startTime + duration){

    String IncomingString = "";
    bool StringReady = false;
  
    while (BTserial.available()){
      IncomingString=BTserial.readString();
      StringReady = true;
    }

    
    if(StringReady){
      //Serial.println(IncomingString);
    
      
      if(IncomingString.indexOf("+INQ") != -1){
        int rssi = getNearestRssiValue(IncomingString);
        Serial.print("rssi: ");
        Serial.println(rssi);
        if (rssi > RSSI_TRESHOLD) {
          BTserial.println("AT+INQC"); //should stop inquiry mode, works only sometimes
          return true;
        }
      }
    }

    yield(); //let background processes do their things (the esp crashes otherwise)
    
    curTime = now();
  }
  
  Serial.println("time ended");
  BTserial.println("AT+INQC");
  return false;
}

int getNearestRssiValue(String inqLines){
  //parses multi-line blocks of inq data and returns the biggest, non-zero rssi value of a device of which the adress is in adresses
  
  const int MIN = -9999;
  int result = MIN;

  int pos = 0;
  int npos = inqLines.indexOf("+INQ", pos+1);
        
  while(pos != -1){
    int endPos = npos;
    if(endPos == -1) endPos = inqLines.length();
    String curLine = inqLines.substring(pos, npos);
    //Serial.print(curLine);
    int rssi = getRssi(curLine);

    if(rssi != 0 and rssi > result) result = rssi;
    
    pos = npos;
    npos = inqLines.indexOf("+INQ", npos+1);
   }

  if(result == MIN) result = 0;
  return result;
}

int getRssi(String line){
  //parses line of inq data and returns 0 when adresses are not in adresses (the array), else the rssi
  //the rssi is between 0-255
  //an example of an line is: +INQ:[4 alpha-numeric chars]:[2 alpha-numeric chars]:[6 alpha-numeric chars],7A020C,FFC3
  
  int comma_pos = line.indexOf(",", 0);
  String cur_adress = line.substring(line.indexOf(":")+1, comma_pos);

  //check if cur_adress is contained in adresses
  bool match = false;
  for (int i = 0; i<sizeof(adresses)/sizeof(String); i++){
    if (cur_adress == adresses[i]) {
      match = true;
      break;
    }
  }
  if (!match) return 0;

  comma_pos = line.indexOf(",", comma_pos+1);
  String hexStr = line.substring(comma_pos+3); //skipping "FF"
  return (int)strtol(&hexStr[0], NULL, 16);
}
