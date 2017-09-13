#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include "FS.h"
String arch;
int ledPin = 13; // GPIO13
WiFiServer server(80);

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    delay(10);

    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);

    SPIFFS.begin();
    File f = SPIFFS.open("/test.txt", "r");
    if (!f) {
      Serial.println("file open failed");
    }
    arch = f.readStringUntil('EOF');
    Serial.println(arch);
    
    
    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
    //wifiManager.resetSettings();
    
    //set custom ip for portal
    //wifiManager.setSTAStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
    //wifiManager.setSTAStaticIPConfig(IPAddress(666,666,6,6),IPAddress(666,666,6,6),IPAddress(666,666,6,6));
    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.setCustomHeadElement("<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;color:#fff;background-color:#000;} input{width:95%;} body{text-align: center;font-family:verdana; background-color:#000;color:#fff;} button{border:0;border-radius:0.3rem;background-color:#E94F37;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;} .q{float: right;width: 64px;text-align: right;}</style>");
    wifiManager.autoConnect("Owledtronics Wifi Config");
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();

    
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
    server.begin();
    Serial.println("Server started");
    Serial.print("Use this URL to connect: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");

    
}

void loop() {
    // put your main code here, to run repeatedly:
    // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
 
  int value = LOW;
  if (request.indexOf("/LED=ON") != -1)  {
    digitalWrite(ledPin, HIGH);
    value = HIGH;
  }
  if (request.indexOf("/LED=OFF") != -1)  {
    digitalWrite(ledPin, LOW);
    value = LOW;
  }
 
// Set ledPin according to the request
//digitalWrite(ledPin, value);
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;color:#fff;background-color:#000;} input{width:95%;} body{text-align: center;font-family:verdana; background-color:#000;color:#fff;} button{border:0;border-radius:0.3rem;background-color:#E94F37;color:#fff;line-height:2.4rem;font-size:1.2rem;width:75%;} .q{float: right;width: 64px;text-align: right;}</style>");
 
  client.print("Led pin is now: ");
 
  if(value == HIGH) {
    client.print("On");
  } else {
    client.print("Off");
  }
  client.println("<br><br>");
  client.println("<a href=\"/LED=ON\"\"><button>Turn On </button></a>");
  client.println("<a href=\"/LED=OFF\"\"><button>Turn Off </button></a><br />");  
  client.println("<br><br>");
  client.println("<br><br>");
  client.println(arch);
  client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");



    
}
