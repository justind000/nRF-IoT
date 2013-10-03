#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9,10);

const long long       // Use pipe + 1 for nodes, pipe + 2 for relays
  RELAYBROADCAST  = 0xAA00000000LL,
  NODEACK         = 0xCC00000000LL;

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
 
  long cnt[10] = {};
  byte cntID = 0;
  byte retries = 0;                 // How many times have we tried to rx
  const byte MAX_RETRIES = 5;       // How many times will we try?
  
  long myID;

void setup(void){
	
  Serial.begin(57600);
  radio.begin();
  radio.setRetries(15,15);
  radio.enableDynamicPayloads();
  randomSeed(analogRead(0));
  Serial.println(F("node starting..."));
  
  radio.openReadingPipe(1 ,NODEACK + 1);  // Read the 'node' pipe
  
  myID = random(1, 0xffff);      // Identify this packet
  xmit(myID, 2);           // Send using the 'relay' pipe
 }

void loop(void){

  myID = random(1, 0xffff);      // Identify this packet
  
  xmit(myID, 1);            // Send some data
   
  wait(MAX_RETRIES, myID);        // Wait for it to be acknowledged
  
  delay(3000);              // Pause before repeating
  
  beARelay();               // If serial port connected send some more data (for what purpose?)
}

// Get Ack from relay or timeout
void wait(byte retries, long myID) {
  
	bool reply;
	
	do {
		
 		unsigned long started_waiting_at = millis();
  
	  while (millis() - started_waiting_at < 250) {
	    // Wait here until we get a response, or timeout (250ms)
	    
	    if (reply = radio.available()) break;
	  }

	  retries--;
	  
  } while (retries > 0 && !reply);
  
  
  if (reply) {
	  
	  ack(reply, myID);
  }
  else {
	  
	  nak(myID);
  }
}

// Signal a NAK
void nak(long myID) {
  
  Serial.print(F("NACK:      ")); Serial.println(myID, HEX);
  
}

// Signal an ACK
void ack(bool reply, long myID) {
  
  if (reply) {
    
    long src;      //ack returns just the header.ID, so check what was returned with what was sent
    radio.read( &src, radio.getDynamicPayloadSize() );
    
    if (src == myID) {
      Serial.print(F("ACK:            ")); Serial.println(src, HEX);
    }
//     else {	// Display packets destined for nodes other than us
// 	    Serial.print(src, HEX);Serial.print(" = "); Serial.println(myID, HEX);
//     }
  }
  else {
	  Serial.println(F("NOT AVAILABLE"));
  }
}

// If something is typed on the Serial console, pretend to be a relay and send a relay broadcast
void beARelay() {
 
if ( Serial.available() )
  {
    Serial.read();
       
    myID = random(1, 0xffff);
    
    xmit(myID, 2);

    Serial.print(F("TEST: ")); Serial.println(myID, HEX);
  }
}

// Send some data to the base
void xmit(long myID, byte pipe_id) {

  HEADER header;
  
  header.ID = myID;
  header.type = 3;
  header.hops = 0;
  header.src = 0xabcd;
  
  header.sensor.temp = 18.8;

  Serial.print(F("XMIT: "));Serial.println(header.ID, HEX);

  byte retries = 5;
  bool ok;
  
  do {
	  ok = relay(header, pipe_id);           // Send using the 'node' pipe
  } while (!ok && --retries > 0);
  
}

// Send packet to any relay that can hear this node
bool relay(struct HEADER header, byte pipe_id) {

  radio.stopListening();
  radio.openWritingPipe(RELAYBROADCAST + pipe_id);
  bool ok = radio.write( &header, sizeof(header), true );
  
  Serial.print(ok ? F("SENT            ") : F("FAILED TO SEND  "));	// Seems to fail to transmit often
  Serial.println(header.ID, HEX);

  radio.startListening();
  
  return ok;
}
