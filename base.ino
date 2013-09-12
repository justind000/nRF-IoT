#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9,10);

#define BASEBROADCAST(x) (0xBB00000000LL + x)
#define RELAYBROADCAST(x) (0xAA00000000LL + x)

struct SENSOR{
  float temp;
  float humidity;
  float pressure;
};

struct HEADER{
  long type;
  long hops;
  long src;
  long ID;
  SENSOR sensor;
};

  HEADER header;        //used throughout

  long cnt[10] = {};    //array used to store the last 10 header.IDs
  byte cntID = 0;       //counter for header.ID array
  
bool DupID(long id){
  //this function keeps the last 10 header.IDs, then searches through them all
  //when cntID reaches 10, it rolls over to 0 again
  //if a match is found, it returns true, falst otherwise
  bool found = false;
  for (int i = 0; i < 10; i++)
    if (cnt [i] == id){
      found = true;
      break;}
  if (cntID < 10){cnt[cntID] = id;}
  else{cntID = 0;cnt[cntID] = id;}
  cntID++;
  return found;
}

void setup(void){
  Serial.begin(57600);
  
  //setup radio
  radio.begin();
  radio.setRetries( 15,15 );
  radio.enableDynamicPayloads();
  radio.openReadingPipe( 1, BASEBROADCAST(1) );    //Nodes send on this
  radio.openWritingPipe( BASEBROADCAST(2) );
  radio.startListening();
  Serial.println("base starting...");
 }

void loop(void){
  //check to see if we have anything available
   if ( radio.available() ){
      bool done = false;
      while (!done){
        //read whatever is available
        done = radio.read( &header, radio.getDynamicPayloadSize() );
        Serial.print( "Got message from 0x" ); Serial.print( header.src, HEX );Serial.print( " ID:" );Serial.print( header.ID, HEX ); Serial.print( " Hops: " );Serial.println(header.hops);
        
      }
    }
}
