#include <packet.h>
#include <Arduino.h>

void printPkt(const dataPkt * pkt){
	Serial.print(F("Packet "));
	Serial.println(pkt->seqId);
	Serial.print(F("Source: "));
	Serial.println(pkt->srcId);

	Serial.print(F("Day: "));
	Serial.println(pkt->day);
	Serial.print(F("Mth: "));
	Serial.println(pkt->mth);
	Serial.print(F("Year: "));
	Serial.println(pkt->year);

	Serial.print(F("Hour: "));
	Serial.println(pkt->hour);
	Serial.print(F("Min: "));
	Serial.println(pkt->min);
	Serial.print(F("Sec: "));
	Serial.println(pkt->sec);

	Serial.print(F("Location: "));
	Serial.print(pkt->latitude, 4);
	Serial.print(", "); 
	Serial.println(pkt->longitude, 4);

	Serial.print(F("Angle: "));
	Serial.println(pkt->avgAngle, 4);
	Serial.print(F("Bumpiness: "));
	Serial.println(pkt->varVertical);

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

void printPktLine(const dataPkt * pkt){
	Serial.print("$ ");
	Serial.print(pkt->srcId); Serial.print(" ");
	Serial.print(pkt->seqId); Serial.print(" ");
	Serial.print(pkt->day); Serial.print(" ");
	Serial.print(pkt->mth); Serial.print(" ");
	Serial.print(pkt->year); Serial.print(" ");
	Serial.print(pkt->hour); Serial.print(" ");
	Serial.print(pkt->min); Serial.print(" ");
	Serial.print(pkt->sec); Serial.print(" ");
	Serial.print(pkt->longitude); Serial.print(" ");
	Serial.print(pkt->latitude); Serial.print(" ");
	Serial.print(pkt->avgAngle); Serial.print(" ");
	Serial.print(pkt->varVertical); Serial.println();

}