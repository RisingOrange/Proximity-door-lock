#include "MyWebserver.h"

//Http authentication data
const char* www_username = "admin";
const char* www_password = "esp8266";

//AP authentication data
const char* ssid = "proximity lock";
const char* password = "12345678"; //this has to have a minimum of 8 chars

//static ip adress of the webserver
IPAddress apIP(10, 10, 10, 1);

void split(String in, String seperator, String (&outRef) [5]);

MyWebserver::MyWebserver(){
  //make the esp start in ap_sta mode, but with AP off
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPdisconnect (true);

  Serial.begin(9600);

  //set the site template
  site_template = webpage;

  //initialize server
  server = new ESP8266WebServer(80);
  
  //associate the handler functions with the server paths
  server->on("/", std::bind(&MyWebserver::handleRootPath, this));
  server->on("/submit", std::bind(&MyWebserver::handleSubmitPath, this));

  //default httpAuthentication state is off
  isHttpAuthenticationActive = false;

}

void MyWebserver::connectToWifi(const char* ssid, const char* pass){
  WiFi.begin(ssid, pass);
 
  while (WiFi.status() != WL_CONNECTED) {
 
    delay(500);
    Serial.println("Waiting to connect…");
 
  }
  
  Serial.print("IP address: ");
  own_ip = WiFi.localIP().toString();
  Serial.println(own_ip);
}

void MyWebserver::startAP(){
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));   // subnet FF FF FF 00
  WiFi.softAP(ssid, password);
  own_ip = WiFi.softAPIP().toString();

  Serial.println(own_ip);
}

void MyWebserver::quitAP(){
  WiFi.softAPdisconnect (true);
}

void MyWebserver::setHttpAuthentication(bool state){
  isHttpAuthenticationActive = state;
}

void MyWebserver::begin(){

  //prepare the site for the first time
  prepareSite();
  
  //start server
  server->begin(); 

}

void MyWebserver::handleClient(){
  server->handleClient();
}

void MyWebserver::requestAuthentication(){
  if (!server->authenticate(www_username, www_password)) {
    return server->requestAuthentication();
  }

}

void MyWebserver::handleRootPath(){

  if (isHttpAuthenticationActive) requestAuthentication();
  server->send(200, "text/html", site);
} 
  
void MyWebserver::handleSubmitPath() {

  if (isHttpAuthenticationActive) requestAuthentication();
  
  if (server->args() == 0 ) return;

  //send response to client so he can stop waiting 
  server->send(200, "text/plain", "received");

  if (server->argName(0) != "adresses") return;
  String elementsString = server->arg(0);
  
  if(elementsString.length() == 0){
    for(int i=0; i<sizeof(theArray)/sizeof(String); i++) theArray[i] = String("");
  } else {
    split(elementsString, ",", theArray);
  }

  //for(int i=0; i<sizeof(theArray)/sizeof(String); i++) Serial.println(theArray[i]);
  //Serial.println();

  prepareSite();
}

void MyWebserver::prepareSite() {

    String htmlList = "";
    for(int i = 0; i<sizeof(theArray)/sizeof(String); i++){
      if (theArray[i] == String("")) break;
      htmlList += "\n<li>";
      htmlList += theArray[i];
      htmlList += "</li>";
    } htmlList += "\n";
    
    site = site_template;
    site.replace("|LIST|", htmlList);

    site.replace("|IP|", own_ip);
  }

void MyWebserver::updateArray(String (&arrRef) [5]) {
  for (int i = 0; i<sizeof(arrRef)/sizeof(String); i++) arrRef[i] = theArray[i];
}

void split(String in, String seperator, String (&outRef) [5]) {
  //helper function, seperates the in-String on the seperator in substrings and writes them to the outRef
  //the outRef has a fixed number of elements, if there are less substrings after splitting all other places of the array are set to ""
  //example split("1,asfas,204", ",", result) --> result = {"1", "asfas", "204", "", ""} 
  
  int pos = 0;
  int idx = 0;
  
  while(true){
    int npos = in.indexOf(seperator, pos);
    if (npos == -1) break;
    String cur_substr = in.substring(pos, npos);
    outRef[idx] = cur_substr;
    pos = npos+1;
    idx++;
  } 
  outRef[idx] = in.substring(pos);
  idx++;
  
  for (; idx<sizeof(outRef)/sizeof(String); idx++) outRef[idx] = String("");
}