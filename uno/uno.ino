#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <SPI.h>

RF24 radio(9,10);
RF24Network network(radio);
RF24Mesh mesh(radio, network);


uint16_t nodeID = 10;
uint16_t got_Voltage[3];
uint16_t voltage[3];
uint16_t xData = 0;
uint16_t yData = 0;
uint16_t location;
uint32_t displayTimer = 0;
struct payload_t {
  unsigned long ms;
  unsigned long counter;
};

void setup() {

  Serial.begin(115200);
  mesh.setNodeID(nodeID);
  Serial.println(F("Connecting to the mesh..."));
  mesh.begin();
}



void loop() {

  mesh.update();

  if (millis() - displayTimer >= 1000) {
    displayTimer = millis();
    voltage[0] = (analogRead(A0));
    voltage[1] = (analogRead(A1));
    voltage[2]=nodeID;
    xData = voltage[0];
    yData = voltage[1];
    location = voltage[2];  
    Serial.println("Sending A0 Voltage: "+String(xData)+ " A1 Voltage: "+String(yData)+" location: "+String(location));
    if (!mesh.write(&voltage, 'M', sizeof(voltage))) {

      if ( ! mesh.checkConnection() ) {
        Serial.println("Renewing Address");
        if(!mesh.renewAddress()){

          mesh.begin();
        }
      } else {
        Serial.println("Send fail, Test OK");
      }
    } else {
      Serial.print("Send OK: "); Serial.println(displayTimer);
    }
  }

  while (network.available()) {
    RF24NetworkHeader header;
    payload_t payload;
    network.read(header, &got_Voltage, sizeof(got_Voltage));
    xData = got_Voltage[0];
    yData = got_Voltage[1];
    location = got_Voltage[2];
    Serial.println("Recived A0 Voltage: "+String(xData)+ "  A1 Voltage: "+String(yData)+" From  "+String(location));
  }
}
