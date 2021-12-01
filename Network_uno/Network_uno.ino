#include <avr/pgmspace.h>
#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include "printf.h"

const uint16_t node_address_set[10] = { 00, 02, 05, 012, 015, 022, 025, 032, 035, 045 };
 
// 0 = Master
// 1-2 (02,05)   = Children of Master(00)
// 3,5 (012,022) = Children of (02)
// 4,6 (015,025) = Children of (05)
// 7   (032)     = Child of (02)
// 8,9 (035,045) = Children of (05)
//UNO
uint8_t NODE_ADDRESS = 3;  
RF24 radio(9,10);
//nano
//uint8_t NODE_ADDRESS = 1;  
//RF24 radio(9,10);
//esp
//uint8_t NODE_ADDRESS = 0;
//RF24 radio(2,4);                               
RF24Network network(radio); 
uint16_t this_node;                           

const unsigned long interval = 1000;     
unsigned long last_time_sent;

uint16_t xData = 0;
uint16_t yData = 0;
uint16_t got_Voltage[2];
uint16_t voltage[2];

  
const short max_active_nodes = 10;            
uint16_t active_nodes[max_active_nodes];
short num_active_nodes = 0;
short next_ping_node_index = 0;


bool send_T(uint16_t to);                     
bool send_N(uint16_t to);
void handle_T(RF24NetworkHeader& header);
void handle_N(RF24NetworkHeader& header);
void add_node(uint16_t node);


void setup(){
  
  Serial.begin(115200);
  printf_begin();


  this_node = node_address_set[NODE_ADDRESS];            // Which node are we?
  
  SPI.begin();                                           // Bring up the RF network
  radio.begin();
  radio.setPALevel(RF24_PA_HIGH);
  network.begin(100, this_node );
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
}

void loop()
{  
  network.update();                                      
   while ( network.available() )  
   {                      
    RF24NetworkHeader header;                            
    network.peek(header);
    switch (header.type)
    {                              
      case 'T': handle_T(header); break;
      case 'N': handle_N(header); break;
      default:  printf_P(PSTR("*** WARNING *** Unknown message type %c\n\r"),header.type);
                network.read(header,0,0);
                break;
    };
   }
  unsigned long now = millis();                         
  if ( now - last_time_sent >= interval ){
    last_time_sent = now;
    uint16_t to = 00;                                   
    if ( num_active_nodes ){                          
        to = active_nodes[next_ping_node_index++];      
        if ( next_ping_node_index > num_active_nodes ){ 
	    next_ping_node_index = 0;                   
	    to = 00;                                    
        }
    }
    bool ok;

    
    if ( this_node > 00 || to == 00 ){                  
        ok = send_T(to);   
    }else{                                                
        ok = send_N(to);
    }
    
    if (ok){                                              
        printf_P(PSTR("%lu: APP Send ok\n\r"),millis());
    }else{
        printf_P(PSTR("%lu: APP Send failed\n\r"),millis());
        last_time_sent -= 100;                            
    }
  }



}

bool send_T(uint16_t to)
{
  RF24NetworkHeader header(to, 'T');
  voltage[0] = (analogRead(A0));
  voltage[1] = (analogRead(A1));
//  voltage[1] = 0;
  xData = voltage[0];
  yData = voltage[1];  
  Serial.println("Sending A0 Voltage: "+String(xData)+ " A1 Voltage: "+String(yData)+" to  "+String(to));

  return network.write(header,&voltage, sizeof( voltage ));
}

bool send_N(uint16_t to)
{
  RF24NetworkHeader header(to,'N' );
  
  printf_P(PSTR("---------------------------------\n\r"));
  printf_P(PSTR("%lu: APP Sending active nodes to 0%o...\n\r"),millis(),to);
  return network.write(header,active_nodes,sizeof(active_nodes));
}


void handle_T(RF24NetworkHeader& header){

  unsigned long message;                                                                      
  network.read(header,&got_Voltage, sizeof( got_Voltage ));
  xData = got_Voltage[0];
  yData = got_Voltage[1];
  Serial.println("Recived A0 Voltage: "+String(xData)+ "  A1 Voltage: "+String(yData)+" From  "+String(header.from_node));



  if ( header.from_node != this_node || header.from_node > 00 )                                
    add_node(header.from_node);
}

void handle_N(RF24NetworkHeader& header)
{
  static uint16_t incoming_nodes[max_active_nodes];

  network.read(header,&incoming_nodes,sizeof(incoming_nodes));
  printf_P(PSTR("%lu: APP Received nodes from 0%o\n\r"),millis(),header.from_node);

  int i = 0;
  while ( i < max_active_nodes && incoming_nodes[i] > 00 )
    add_node(incoming_nodes[i++]);
}


void add_node(uint16_t node){
  
  short i = num_active_nodes;                                    
  while (i--)  {
    if ( active_nodes[i] == node )
        break;
  }
  
  if ( i == -1 && num_active_nodes < max_active_nodes ){         
      active_nodes[num_active_nodes++] = node; 
      printf_P(PSTR("%lu: APP Added 0%o to list of active nodes.\n\r"),millis(),node);
  }
}
