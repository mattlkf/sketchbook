#include <packet.h>
#include <Arduino.h>

void printPkt(dataPkt * pkt){
	Serial.print("Packet ");
	Serial.println(pkt->seqId);

	Serial.print("Location: ");
	Serial.print(pkt->latitude, 4);
	Serial.print(", "); 
	Serial.println(pkt->longitude, 4);

    return;
}