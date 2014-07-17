#include <Arduino.h>
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include <packet.h>
#include <stdio.h>

#define MAX_BUF_LEN 40
#define PAYLOADLEN 32 //max length
#define RXNAME "serv1" // 5 bytes
#define TXNAME "serv1" // 5 bytes

int freeRam();
void handleLine(char *, const uint8_t);
void xmit(const char *, const uint8_t);
void checkRecv();

uint32_t timer = 0;
uint32_t pktCount = 0;
uint32_t pktTotal = 0;

void setupMirf(){
	Mirf.spi = &MirfHardwareSpi;
	Mirf.init();
  	Mirf.setRADDR((byte *) RXNAME);
    Mirf.setTADDR((byte *) TXNAME);
  	Mirf.payload = PAYLOADLEN;
  	Mirf.config();
}

void setup(){
	pktCount = 0;
	pktTotal = 0;
	Serial.begin(115200);

	Serial.println(F("Clctr"));

	setupMirf();

	Serial.println(F("Ready"));

	return;
}

void recv(uint8_t * str, uint8_t len){
	printPkt((dataPkt *) str);

	++pktCount;
	return;
}

int main(){

	init();
	setup();
	
	uint8_t flag = 0;
	char buf[MAX_BUF_LEN];
	uint8_t blen = 0;
	
	while(1){
		if(millis() - timer > 1000){
			timer = millis();
			Serial.println();
			Serial.print(F("Local time: "));
			Serial.print(millis()/1000);
			Serial.println("s");

			//if(pktCount) pktCount--;
			Serial.print(F("Received "));
			Serial.print(pktCount);
			Serial.print(F(" unexpected packets, total "));
			pktTotal += pktCount;
			pktCount = 0;
			Serial.println(pktTotal);

			Serial.print(F("Ram: "));
			Serial.println(freeRam());
		}

		checkRecv();
		if(Serial.available()){
			char ch = Serial.read();
			if(ch == '\r') Serial.print("\n");
			Serial.print(ch);
			// if(ch == '\r') Serial.println("!");
			
			if(ch == '\r'){
				if(flag){
					Serial.print(F("Error: max line is "));
					Serial.println(MAX_BUF_LEN);
					flag = 0;
				}
				else {
					handleLine(buf, blen);
				}

				blen = 0;
				continue;
			}

			if(blen < MAX_BUF_LEN){
				buf[blen++] = ch;
			}
			else {
				flag = 1;
			}

		}
	}
	return 0;
}

void xmit(const char * payload, const uint8_t len){
	if(len > PAYLOADLEN){
		Serial.print(len);
		Serial.println(F(" bytes; too long"));
		return;
	}

	uint8_t data[PAYLOADLEN];

	uint8_t i;

	for(i = 0; i<len;i++){
		data[i] = payload[i];
	}
	while(i < PAYLOADLEN) data[i++] = 0;

	Mirf.send(data);

	// while(Mirf.isSending()){
	// 	Serial.println("wait..");
	// }

	Serial.print(F("Sent: "));
	for(i = 0;i<PAYLOADLEN;i++)
		Serial.write(data[i]);
	Serial.print(" (");
	Serial.print(len);
	Serial.println(F(") bytes"));
	// Serial.println();
	return;
}

void checkRecv(){
	if(Mirf.isSending() || !Mirf.dataReady()){
		return;
	}

	Serial.println(F("\nGot packet!"));

	uint8_t data[PAYLOADLEN];
	uint8_t i;

	Mirf.getData(data);

	for(i = 0;i<PAYLOADLEN;i++){
		Serial.write(data[i]);
	}
	Serial.println();

	recv(data, PAYLOADLEN);

	return;
}



int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}


bool match(const char * a, uint8_t alen, const char * b){
	//debug
	Serial.print(F("Matching "));
	for(uint8_t i = 0; i < alen; i++) Serial.print(a[i]);
	Serial.print(" with ");
	Serial.println(b);

	uint8_t i;
	for(i = 0; b[i] && i < alen; i++){
		if(a[i] == b[i]) continue;
		return false;
	}

	if(b[i] == '\0' && i == alen) return true;

	return false;
}

uint8_t advanceIdxPair(char * buf, const uint8_t len, uint8_t * start, uint8_t * end){
	//advance s to the start of the next token, or end-line (s == len).
	for(*start = *end; *start < len && buf[*start] == ' '; ++(*start));
	//advance t to the first non-token character after s, or end-line (t == len).
	for(*end = *start; *end < len && buf[*end] != ' '; ++(*end));

	return !(*start == *end);
}

void handleLine(char * buf, const uint8_t len){
	// printLine(buf, len);
	
	uint8_t s = 0, t = 0;
	// //advance s to the start of the next token, or end-line (s == len).
	// for(s = t; s < len && buf[s] == ' '; ++s);
	// //advance t to the first non-token character after s, or end-line (t == len).
	// for(t = s; t < len && buf[t] != ' '; ++t);

	if(!advanceIdxPair(buf, len, &s, &t)) return;

	if(match(buf + s, t - s, "r")){
		char tmp;
		uint32_t ids, ide;

		if(!advanceIdxPair(buf, len, &s, &t)) return;
		
		tmp = buf[t];
		buf[t] = 0;
		ids = atoi(buf+s);
		buf[t] = tmp;

		if(!advanceIdxPair(buf, len, &s, &t)) return;
		
		tmp = buf[t];
		buf[t] = 0;
		ide = atoi(buf+s);
		buf[t] = tmp;


		// for(uint8_t i = s; i<t;i++) Serial.print(buf[i]); Serial.println();
		
		reqPkt pkt;
		pkt.s[0] = ids;
		pkt.e[0] = ide;
		pkt.s[1] = 0;
		pkt.e[1] = 0;

		Serial.print(F("Sending: "));
		Serial.print(pkt.s[0]);
		Serial.print('-');
		Serial.print(pkt.e[0]);
		Serial.print(F(" pktsize: "));
		Serial.println(sizeof(pkt));

		xmit((const char *)&pkt, sizeof(pkt));
	}
	else if(match(buf + s, t - s, "s")){

		if(!advanceIdxPair(buf, len, &s, &t)) return;
		
		xmit(buf + s, t-s);
	}
	else if(match(buf + s, t - s, "hello")){
		Serial.println(F("hi :D"));
	}
	return;
}
