

#include "BTmodule.h"

BTmodule::BTmodule(int rx, int tx, int pin34, int rssi_threshold) : rxPin(rx), txPin(rx), pin34Pin(pin34) {

  if ( (0 < rssi_threshold) && (rssi_threshold >= 255)) rssi_threshold = rssi_threshold;
        else Serial.println("BTmodule: entered rssi-treshold in constructor is not valid, using default instead");
        BTserial = new SoftwareSerial(rxPin, txPin);

  //activates the AT mode of the HC05 bt-module
  //ATTENTION: this only puts the module in the right mode, if pin34 was LOW at the beginning (for ~2 seconds) 
  digitalWrite(pin34Pin, HIGH);

  BTserial->begin(9600);  

  BTserial->println("AT+CLASS=0");
  BTserial->println("AT+INQM=1,9,3");

}


bool BTmodule::targetScan(int duration, String adresses[5]){
  /*blocks all other processes for its duration and listens for inq-data on BTserial, 
  if the device with the target adress has a high enough rssi --> return true, 
  if time runs out before this happens, return false*/

  BTserial->println("AT+INQ");
  
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
  
    while (BTserial->available()){
      IncomingString=BTserial->readString();
      StringReady = true;
    }

    
    if(StringReady){
      //Serial.println(IncomingString);
    
      
      if(IncomingString.indexOf("+INQ") != -1){
        int rssi = getNearestRssiValue(IncomingString, adresses);
        Serial.print("rssi: ");
        Serial.println(rssi);
        if (rssi > rssi_threshold) {
          BTserial->println("AT+INQC");
          return true;
        }
      }
    }

    yield(); //let background processes do their things (the esp crashes otherwise)
    
    curTime = now();
  }
  
  Serial.println("time ended");
  BTserial->println("AT+INQC");
  return false;
}

int BTmodule::getNearestRssiValue(String inqLines, String adresses[5]){
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
    int rssi = getRssi(curLine, adresses);

    if(rssi != 0 and rssi > result) result = rssi;
    
    pos = npos;
    npos = inqLines.indexOf("+INQ", npos+1);
   }

  if(result == MIN) result = 0;
  return result;
}

int BTmodule::getRssi(String line, String adresses[5]){
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