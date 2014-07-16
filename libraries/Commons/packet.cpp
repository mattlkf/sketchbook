#include <packet.h>
#include <Arduino.h>

void printPkt(dataPkt * pkt){
	Serial.print("Packet ");
	Serial.println(pkt->seqId);
	Serial.print("Source: ");
	Serial.println(pkt->srcId);

	Serial.print("Day: ");
	Serial.println(pkt->day);
	Serial.print("Mth: ");
	Serial.println(pkt->mth);
	Serial.print("Year: ");
	Serial.println(pkt->year);

	Serial.print("Hour: ");
	Serial.println(pkt->hour);
	Serial.print("Min: ");
	Serial.println(pkt->min);
	Serial.print("Sec: ");
	Serial.println(pkt->sec);

	Serial.print("Location: ");
	Serial.print(pkt->latitude, 4);
	Serial.print(", "); 
	Serial.println(pkt->longitude, 4);

    return;
}