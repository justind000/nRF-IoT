#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9,10);

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

  HEADER header;

  long cnt[10] = {};
  byte cntID = 0;

void setup(void){
  Serial.begin(57600);
  radio.begin();
  radio.setRetries(15,15);
  radio.enableDynamicPayloads();
  randomSeed(analogRead(0));
  Serial.println("node starting...");
  radio.openReadingPipe(1 ,RELAYBROADCAST(2));
  radio.openWritingPipe(RELAYBROADCAST(2));
  radio.startListening();
 }

void loop(void){
    radio.stopListening();
    header.type = 3;
    header.hops = 0;
    header.src = 0xabcd;
    header.ID = random(1, 0xffff);
    header.sensor.temp = 78.8;
    Serial.println(header.ID, HEX);
  //send a node broadcast to any relay that can hear
  radio.openWritingPipe(RELAYBROADCAST(1));
  bool ok = radio.write( &header, sizeof(header), true );
  radio.startListening();
  
  delay(3000);

  //testing
if ( Serial.available() )
  {
    radio.stopListening();
    Serial.read();
    header.type = 3;
    header.hops = 0;
    header.src = 0xabcd;
    header.ID = random(1, 0xffff);

    Serial.println(header.ID, HEX);
  //send a relay broadcast
  radio.openWritingPipe(RELAYBROADCAST(2));
  bool ok = radio.write( &header, sizeof(header), true );
  radio.startListening();
  }
}
