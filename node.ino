#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9,10);

#define RELAYBROADCAST(x) (0xAA00000000LL + x)
#define NODEACK(x) (0xCC00000000LL + x)

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
  byte retries = 0;             //how many times have we tried to rx
  const byte MAX_RETRIES = 5;  //how many times will we try?
  

void setup(void){
  Serial.begin(57600);
  radio.begin();
  radio.setRetries(15,15);
  radio.enableDynamicPayloads();
  randomSeed(analogRead(0));
  Serial.println("node starting...");
  radio.openReadingPipe(1 ,NODEACK(1));
  radio.openWritingPipe(RELAYBROADCAST(2));
  radio.startListening();
 }

void loop(void){
  header.ID = random(1, 0xffff);    //this is above the label for testing
  retry:
    radio.stopListening();
    header.type = 3;
    header.hops = 0;
    header.src = 0xabcd;
    
    header.sensor.temp = 78.8;
    Serial.println(header.ID, HEX);
    //send a relay broadcast to any relay that can hear
    radio.openWritingPipe(RELAYBROADCAST(1));
    radio.write( &header, sizeof(header), true );
    radio.startListening();
    // Wait here until we get a response, or timeout (250ms)
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout )
      if (millis() - started_waiting_at > 200 )
        timeout = true;

    // Describe the results
    if ( timeout ){
      Serial.println("NACK");
      retries++;
      if (retries < MAX_RETRIES)
        goto retry;    //it didnt send and we still have some retries left
      else
        retries = 0;  //it didnt send and we have tried as many times as we are willing
        goto done;    //reset counter and wait until next cycle to try another time
    }
    else{
      long src;      //ack returns just the header.ID, so check what was returned with what was sent
      radio.read( &src, radio.getDynamicPayloadSize() );
      if (src == header.ID)
        Serial.print("ACK: ");Serial.println(src, HEX);
        retries = 0;
    }
  done:
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
