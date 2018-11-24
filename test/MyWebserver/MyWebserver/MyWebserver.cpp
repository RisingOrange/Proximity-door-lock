#include "MyWebserver.h"

//Http authentication data
const char* www_username = "admin";
const char* www_password = "esp8266";

//AP authentication data
const char* ssid = "proximity lock";
const char* password = "12345678"; //this has to have a minimum of 8 chars

void split(String in, String seperator, String (&outRef) [5]);

MyWebserver::MyWebserver(){

  Serial.begin(9600);

  //initialize theArray empty
  for(int i = 0; i<sizeof(theArray)/sizeof(String); i++) theArray[i] = ".";

  //set the site template
  site_template = "<!DOCTYPE html><html>\n<head>\n<meta name='viewport' content= 'width=device-width, initial-scale=1'>\n<style>\nbody {\n  margin: 0;\n  min-width: 250px;\n}\n\n/* Include the padding and border in an element\'s total width and height */\n* {\n  box-sizing: border-box;\n}\n\n/* Remove margins and padding from the list */\nul {\n  margin: 0;\n  padding: 0;\n}\n\n/* Style the list items */\nul li {\n  position: relative;\n  padding: 12px 8px 12px 40px;\n  list-style-type: none;\n  background: #eee;\n  font-size: 18px;\n  transition: 0.2s;\n  \n  /* make the list items unselectable */\n  -webkit-user-select: none;\n  -moz-user-select: none;\n  -ms-user-select: none;\n  user-select: none;\n}\n\n/* Set all odd list items to a different color (zebra-stripes) */\nul li:nth-child(odd) {\n  background: #f9f9f9;\n}\n\n\n/* Style the close button */\n.close {\n  position: absolute;\n  right: 0;\n  top: 0;\n  padding: 12px 16px 12px 16px;\n}\n\n.close:hover {\n  background-color: #f44336;\n  color: white;\n}\n\n/* Style the header */\n.header {\n  background-color: #f44336;\n  padding: 30px 40px;\n  color: white;\n  text-align: center;\n}\n\n/* Clear floats after the header */\n.header:after {\n  content: '';\n  display: table;\n  clear: both;\n}\n\n/* Style the input */\ninput {\n  margin: 0;\n  border: none;\n  border-radius: 0;\n  width: 75%;\n  padding: 10px;\n  float: left;\n  font-size: 16px;\n}\n\n/* Style the 'Add' button */\n.addBtn {\n  padding: 10px;\n  width: 25%;\n  background: #d9d9d9;\n  color: #555;\n  float: left;\n  text-align: center;\n  font-size: 16px;\n  cursor: pointer;\n  transition: 0.3s;\n  border-radius: 0;\n}\n\n.addBtn:hover {\n  background-color: #bbb;\n}\n</style>\n</head>\n<body>\n\n<div id='myDIV' class='header'>\n  <h2 style='margin:5px'>Bluetooth Adresses</h2>\n  <input type='text' id='myInput' placeholder='...'>\n  <span onclick='newElement()' class='addBtn'>Add</span>\n</div>\n\n<ul id='myUL'>";
  site_template += "|list_html|";
  site_template += "</ul>\n\n<p><button type=\'button\' onclick=\'submit()\' >Submit </button></p>\n\n<script>\n/* Create a 'close' button and append it to each list item */\nvar myNodelist = document.getElementsByTagName('LI');\nvar i;\nfor (i = 0; i < myNodelist.length; i++) {\n  var span = document.createElement('SPAN');\n  var txt = document.createTextNode('X');\n  span.className = 'close';\n  span.appendChild(txt);\n  myNodelist[i].appendChild(span);\n}\n\n/* Click on a close button to hide the current list item */\nvar close = document.getElementsByClassName('close');\nvar i;\nfor (i = 0; i < close.length; i++) {\n  close[i].onclick = function() {\n    var div = this.parentElement;\n    div.parentNode.removeChild(div);\n  }\n}\n\n/* Create a new list item when clicking on the 'Add' button */\nfunction newElement() {\n  \nif (myNodelist.length >= 5){\n     alert('there can not be more than 5 devices registered');\n     return;\n  }  \n    var li = document.createElement('li');\n  var inputValue = document.getElementById('myInput').value;\n  var t = document.createTextNode(inputValue);\n  li.appendChild(t);\n  if (inputValue === \'\') {\n    alert('You must write something!');\n  } else {\n    document.getElementById('myUL').appendChild(li);\n  }\n  document.getElementById('myInput').value = '';\n\n  var span = document.createElement('SPAN');\n  var txt = document.createTextNode('X');\n  span.className = 'close';\n  span.appendChild(txt);\n  li.appendChild(span);\n\n  for (i = 0; i < close.length; i++) {\n    close[i].onclick = function() {\n      var div = this.parentElement;\n      div.parentNode.removeChild(div);\n    }\n  }\n}\n\n/* send http get request to the given url */\nfunction httpGet(theUrl) { \n   var xmlHttp = new XMLHttpRequest(); \n   xmlHttp.open( \'GET\', theUrl, true ); /* true for synchronous request */\n   xmlHttp.send( null ); \n}\n\n/* send data from li elements to own server */\nfunction submit() { \n   submitUrl = \'http://";
  site_template += "|IP|";
  site_template += "/submit?adresses=\'; \n   var myNodelist = document.getElementsByTagName(\'LI\'); \n   \n   var i = 0; for (i = 0; i < myNodelist.length; i++) { \n      submitUrl += myNodelist[i].textContent; \n      submitUrl = submitUrl.slice(0, -1); \n      submitUrl += \',\' \n   } \n   \n   if (myNodelist.length != 0) submitUrl = submitUrl.slice(0, -1); \n   alert(submitUrl); \n   httpGet(submitUrl); \n   /*alert(\'done\');*/ \n}\n\n\n</script>\n\n</body>\n</html>\n";

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

void MyWebserver::beginAP(){
  WiFi.mode(WIFI_AP_STA);  
  WiFi.softAP(ssid, password);
  own_ip = WiFi.softAPIP().toString();

  Serial.println(own_ip);
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
    for(int i=0; i<sizeof(theArray)/sizeof(String); i++) theArray[i] = String(".");
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
      if (theArray[i] == ".") break;
      htmlList += "\n<li>";
      htmlList += theArray[i];
      htmlList += "</li>";
    } htmlList += "\n";
    
    site = site_template;
    site.replace("|list_html|", htmlList);

    site.replace("|IP|", own_ip);
  }

void MyWebserver::updateArray(String (&arrRef) [5]) {
  for (int i = 0; i<sizeof(arrRef)/sizeof(String); i++) arrRef[i] = theArray[i];
}

void split(String in, String seperator, String (&outRef) [5]) {
  //helper function, seperates the in-String on the seperator in substrings and writes them to the outRef
  //the outRef has a fixed number of elements, if there are less substrings after splitting all other places of the array are set to "."
  //example split("1,asfas,204", ",", result) --> result = {"1", "asfas", "204", ".", "."} 
  
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
  
  for (; idx<sizeof(outRef)/sizeof(String); idx++) outRef[idx] = String(".");
}