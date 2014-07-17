#include <packet.h>
#include <Arduino.h>

void printPkt(const dataPkt * pkt){
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

void printPkt(const reqPkt * pkt){
	uint8_t i;

	Serial.println(F("Request packet:"));
	for(i = 0; i < 4 && pkt->s[i]; i++){
		Serial.print('(');
		Serial.print(pkt->s[i]);
		Serial.print(',');
		Serial.print(pkt->e[i]);
		Serial.println(')');
	}

    return;
}