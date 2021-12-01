#include "RF24Network.h"
#include "RF24.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
uint16_t xData = 0;
uint16_t yData = 0;
uint16_t got_Voltage[3];
uint16_t voltage[3];
uint16_t location;
String url = "";
RF24 radio(2,4);
RF24Network network(radio);
RF24Mesh mesh(radio,network);
const char* ssid = "Personal Wi-Fi";
const char* password = "94489537534";
uint32_t displayTimer = 0;

void setup() {
  Serial.begin(115200);
  mesh.setNodeID(0);
  Serial.println(mesh.getNodeID());
  mesh.begin();
}


void loop() {    

  mesh.update();
  mesh.DHCP();
  
  
  if(network.available()){
    RF24NetworkHeader header;
//    network.peek(header);
//    
//    uint32_t dat=0;
//    switch(header.type){
//      case 'M': network.read(header,&dat,sizeof(dat)); Serial.println(dat); break;
//      default: network.read(header,0,0); Serial.println(header.type);break;
      unsigned long message;                                                                      
  network.read(header,&got_Voltage, sizeof( got_Voltage ));
  xData = got_Voltage[0];
  yData = got_Voltage[1];
  location = got_Voltage[2];
  Serial.println("Recived A0 Voltage: "+String(xData)+ "  A1 Voltage: "+String(yData)+" From  "+String(location));
  url="http://airqualitysensor1.000webhostapp.com/espPostData.php?api_key=tPmAT5Ab3j7F9&Location="+String(location)+"&CO="+String(xData)+"&NO="+String(yData);

HTTPClient http;
url.replace("\r", "");
url.replace("\n", "");   
http.begin(url);
int httpCode = http.GET();
if (httpCode > 0)
{
    String payload = http.getString();   //Get the request response payload
    Serial.println(payload);
    payload.replace("\r", "");
    payload.replace("\n", "");
    if(payload.indexOf("Inserted") >= 0)
    Serial.println("Success");
    }
  }
  
  if(millis() - displayTimer > 5000){
    displayTimer = millis();
    Serial.println(" ");
    Serial.println(F("********Assigned Addresses********"));
     for(int i=0; i<mesh.addrListTop; i++){
       Serial.print("NodeID: ");
       Serial.print(mesh.addrList[i].nodeID);
       Serial.print(" RF24Network Address: 0");
       Serial.println(mesh.addrList[i].address,OCT);
     }
    Serial.println(F("**********************************"));
  }
}
