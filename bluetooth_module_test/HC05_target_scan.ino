/*
INQMs first arg has to be set to 1 in HC05
AT+CLASS has to be set to 0

some test data bits: 
uint8_t hexNum = 0xFFC2;
String data = "+INQ:A086:C6:8F4506,7A020C,FF0C";

comment: 
the targetScan function throws errors often, this is due to the fact that the HC05 does not react to
AT+INQ and AT+INQC commands as intended most of the time,
it should work anyway, the Hc05 is just permanently in inquiring mode
*/

#include <TimeLib.h>

#include <SoftwareSerial.h>

SoftwareSerial BTserial(2, 3); // RX | TX

int button = 10;
int pin34 = 4;

String target_adress = "A086:C6:8F4506";

void setup() {
  // put your setup code here, to run once:

  pinMode(button, INPUT);
  pinMode(pin34, OUTPUT);

  //waiting 2 seconds before powering pin34 of the HC05 prevents it from entering the permanent AT mode
  //during which inquiring does not work
  digitalWrite(pin34, LOW);
  delay(2000);
  digitalWrite(pin34, HIGH);
  
  Serial.begin(9600);  
  Serial.println("Arduino is ready");
  Serial.println("Remember to select Both NL & CR in the serial monitor");

  BTserial.begin(9600);  

  //Serial.println(hexNum, DEC);
  //Serial.println(getRssi(data));
}

bool targetScan(int duration){
  /*blocks all other processes for its duration and listens for inq-data on BTserial, 
  if the device with the target adress has a high enough rssi --> return true, 
  if time runs out before this happens, return false*/

  BTserial.println("AT+INQ");
  
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
      Serial.println(IncomingString);
      
      if(IncomingString.indexOf("+INQ") != -1){
        int rssi = getNearestRssiValue(IncomingString);
        Serial.print("rssi: ");
        Serial.println(rssi);
        if (rssi != 0 and rssi >= -60) {
          Serial.println("returning true");
          BTserial.println("AT+INQC");
          return true;
        }
      }
    }
    
    curTime = now();
  }
  
  Serial.println("time ended");
  BTserial.println("AT+INQC");
  return false;
}

int getNearestRssiValue(String inqLines){
  //parses multi-line blocks of inq data and returns the biggest, non-zero rssi value of the target_adress
  
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
    //Serial.print("rssi: ");
    //Serial.println(rssi);
    
    pos = npos;
    npos = inqLines.indexOf("+INQ", npos+1);
   }

  if(result == MIN) result = 0;
  return result;
}

int getRssi(String line){
  //parses line of inq data and returns 0 when adress does not equal target_adress, else the rssi
  
  int comma_pos = line.indexOf(",", 0);
  String cur_adress = line.substring(line.indexOf(":")+1, comma_pos);
  if (cur_adress != target_adress) {
    return 0;
  }

  comma_pos = line.indexOf(",", comma_pos+1);
  String hexStr = line.substring(comma_pos+1);
  //Serial.print(hexStr);
  return (int)strtol(&hexStr[0], NULL, 16);
}

void loop() {

  // put your main code here, to run repeatedly:
  String IncomingString = "";
  bool StringReady = false;

  bool targetNear = false;
  if(digitalRead(button) == HIGH){
    bool targetNear = targetScan(5);
  }
  

  //the compiler produced segfault errors when the Serial.print(ln) statement was inside the curly braces 
  //following the if statement with button-input above
  if(targetNear){
    Serial.println("target near");
  }
  
  while (BTserial.available()){
   Serial.write(BTserial.read());
  }

  // Keep reading from Arduino Serial Monitor and send
  if (Serial.available()) {
    BTserial.write(Serial.read());
  }
  
}
