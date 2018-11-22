#define webpage "\
<!DOCTYPE html>\
\n<html>\
\n<head>\
\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
\n<style>\
\nbody {\
\n  margin: 0;\
\n  min-width: 250px;\
\n}\
\n\
\n/* Include the padding and border in an element's total width and height */\
\n* {\
\n  box-sizing: border-box;\
\n}\
\n\
\n/* Remove margins and padding from the list */\
\nul {\
\n  margin: 0;\
\n  padding: 0;\
\n}\
\n\
\n/* Style the list items */\
\nul li {\
\n  position: relative;\
\n  padding: 12px 8px 12px 40px;\
\n  list-style-type: none;\
\n  background: #eee;\
\n  font-size: 18px;\
\n  transition: 0.2s;\
\n  \
\n  /* make the list items unselectable */\
\n  -webkit-user-select: none;\
\n  -moz-user-select: none;\
\n  -ms-user-select: none;\
\n  user-select: none;\
\n}\
\n\
\n/* Set all odd list items to a different color (zebra-stripes) */\
\nul li:nth-child(odd) {\
\n  background: #f9f9f9;\
\n}\
\n\
\n\
\n/* Style the close button */\
\n.close {\
\n  position: absolute;\
\n  right: 0;\
\n  top: 0;\
\n  padding: 12px 16px 12px 16px;\
\n}\
\n\
\n.close:hover {\
\n  background-color: #f44336;\
\n  color: white;\
\n}\
\n\
\n/* Style the header */\
\n.header {\
\n  background-color: #f44336;\
\n  padding: 30px 40px;\
\n  color: white;\
\n  text-align: center;\
\n}\
\n\
\n/* Clear floats after the header */\
\n.header:after {\
\n  content: \"\";\
\n  display: table;\
\n  clear: both;\
\n}\
\n\
\n/* Style the input */\
\ninput {\
\n  margin: 0;\
\n  border: none;\
\n  border-radius: 0;\
\n  width: 75%;\
\n  padding: 10px;\
\n  float: left;\
\n  font-size: 16px;\
\n}\
\n\
\n/* Style the \"Add\" button */\
\n.addBtn {\
\n  padding: 10px;\
\n  width: 25%;\
\n  background: #d9d9d9;\
\n  color: #555;\
\n  float: left;\
\n  text-align: center;\
\n  font-size: 16px;\
\n  cursor: pointer;\
\n  transition: 0.3s;\
\n  border-radius: 0;\
\n}\
\n\
\n.addBtn:hover {\
\n  background-color: #bbb;\
\n}\
\n</style>\
\n</head>\
\n<body>\
\n\
\n<div id=\"myDIV\" class=\"header\">\
\n  <h2 style=\"margin:5px\">Bluetooth Adresses</h2>\
\n  <input type=\"text\" id=\"myInput\" placeholder=\"...\">\
\n  <span onclick=\"newElement()\" class=\"addBtn\">Add</span>\
\n</div>\
\n\
\n<ul id=\"myUL\">\
\n|LIST|\
\n</ul>\
\n\
\n<p><button type='button' onclick='submit()' >Submit </button></p>\
\n\
\n<script>\
\n/* Create a \"close\" button and append it to each list item */\
\nvar myNodelist = document.getElementsByTagName(\"LI\");\
\nvar i;\
\nfor (i = 0; i < myNodelist.length; i++) {\
\n  var span = document.createElement(\"SPAN\");\
\n  var txt = document.createTextNode(\"X\");\
\n  span.className = \"close\";\
\n  span.appendChild(txt);\
\n  myNodelist[i].appendChild(span);\
\n}\
\n\
\n/* Click on a close button to hide the current list item */\
\nvar close = document.getElementsByClassName(\"close\");\
\nvar i;\
\nfor (i = 0; i < close.length; i++) {\
\n  close[i].onclick = function() {\
\n    var div = this.parentElement;\
\n    div.parentNode.removeChild(div);\
\n  }\
\n}\
\n\
\n/* Create a new list item when clicking on the \"Add\" button */\
\nfunction newElement() {\
\n  if (myNodelist.length >= 5){\
\n    alert('there can\\\'t be more than 5 adresses registered');\
\n    return;\
\n  }\
\n  \
\n  var li = document.createElement(\"li\");\
\n  var inputValue = document.getElementById(\"myInput\").value;\
\n  var t = document.createTextNode(inputValue);\
\n  li.appendChild(t);\
\n  if (inputValue === '') {\
\n    alert(\"You must write something!\");\
\n  } else {\
\n    document.getElementById(\"myUL\").appendChild(li);\
\n  }\
\n  document.getElementById(\"myInput\").value = \"\";\
\n\
\n  var span = document.createElement(\"SPAN\");\
\n  var txt = document.createTextNode(\"X\");\
\n  span.className = \"close\";\
\n  span.appendChild(txt);\
\n  li.appendChild(span);\
\n\
\n  for (i = 0; i < close.length; i++) {\
\n    close[i].onclick = function() {\
\n      var div = this.parentElement;\
\n      div.parentNode.removeChild(div);\
\n    }\
\n  }\
\n}\
\n\
\n/* send http get request to the given url */\
\nfunction httpGet(theUrl) { \
\n   var xmlHttp = new XMLHttpRequest(); \
\n   xmlHttp.open( 'GET', theUrl, true ); /* true for asynchronous request */\
\n   \
\n   xmlHttp.send(); \
\n}\
\n\
\n/* send data from li elements to own server */\
\nfunction submit() { \
\n   submitUrl = 'http://|IP|/submit?adresses='; \
\n   var myNodelist = document.getElementsByTagName('LI'); \
\n   \
\n   var i = 0; for (i = 0; i < myNodelist.length; i++) { \
\n      submitUrl += myNodelist[i].textContent; \
\n      submitUrl = submitUrl.slice(0, -1); \
\n      submitUrl += ',' \
\n   } \
\n\
\n   if (myNodelist.length != 0) submitUrl = submitUrl.slice(0, -1); \
\n   alert(submitUrl); \
\n   httpGet(submitUrl); \
\n}\
\n\
\n\
\n</script>\
\n\
\n</body>\
\n</html>\
\n\
"