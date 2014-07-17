#include <Arduino.h>
#include <SoftwareSerial.h>
#include <CheeseSock.h>
#include <packet.h>

// nRF24 stuff
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#define MIRF_PAYLOADLEN 32 //max length
#define RXNAME "serv1" // 5 bytes
#define TXNAME "serv1" // 5 bytes

#define MAX_QUEUE_LEN 40

void setupMirf();
void xmit(const uint8_t *, const uint8_t);
void checkRecv();

bool canSend = false;
//end of Mirf stuff

uint8_t queue[MAX_QUEUE_LEN], qlen = 0;
void flush();
void enqueue(const uint8_t *, uint8_t);


CheeseSock csock(new SoftwareSerial(4,5));

void sendok(const uint8_t * str, uint8_t len){
	if(str[0] == 'a') canSend = true;
	if(str[0] == 'b') canSend = false;
	return;
}

void callback(const uint8_t * str, uint8_t len){
#ifdef NETMOD_DEBUG
	Serial.println("Received!");
	Serial.write(str, len);
	Serial.println("\n---");
#endif

#ifdef NETMOD_PRINTPKT
	printPkt((dataPkt *)str);
#endif

	xmit(str, len);

	return;
}

uint32_t timer = 0;

void setup(){
	Serial.begin(115200);
	Serial.println(F("Netmod Ready"));

	setupMirf();

	csock.registerFn(1, sendok);
	csock.registerFn(2, callback);

	csock.begin(9600, 0);


	timer = millis();
}

int main(){
	init();

	setup();

	while(1){
		csock.run();
		if(canSend){
			flush();
		}

		checkRecv();
	}
	return 0;
}

void setupMirf(){
	Mirf.spi = &MirfHardwareSpi;
	Mirf.init();
  	Mirf.setRADDR((byte *) RXNAME);
    Mirf.setTADDR((byte *) TXNAME);
  	Mirf.payload = MIRF_PAYLOADLEN;
  	Mirf.config();
}

void xmit(const uint8_t * payload, const uint8_t len){
	if(len > MIRF_PAYLOADLEN){
		Serial.print(len);
		Serial.println(" bytes; too long");
		return;
	}

	uint8_t data[MIRF_PAYLOADLEN];

	uint8_t i;

	for(i = 0; i<len;i++){
		data[i] = payload[i];
	}
	while(i < MIRF_PAYLOADLEN) data[i++] = 0;

	Mirf.send(data);

	// while(Mirf.isSending()){
	// 	Serial.println("wait..");
	// }

	Serial.print("Sent: ");
	for(i = 0;i<MIRF_PAYLOADLEN;i++)
		Serial.write(data[i]);
	Serial.print(" (");
	Serial.print(len);
	Serial.println(") bytes");
	// Serial.println();

	return;
}

void checkRecv(){
	if(Mirf.isSending() || !Mirf.dataReady()){
		return;
	}

	Serial.println("Got packet!");

	uint8_t data[MIRF_PAYLOADLEN];
	uint8_t i;

	Mirf.getData(data);

	for(i = 0;i<MIRF_PAYLOADLEN;i++){
		Serial.write(data[i]);
	}
	Serial.println();

	//
	enqueue(data, MIRF_PAYLOADLEN);
	
	return;
}

void enqueue(const uint8_t * s, uint8_t len){
	if(len > MAX_QUEUE_LEN) return;
	if(qlen > 0) return;

	for(uint8_t i = 0;i<len;i++){
		queue[i] = s[i];
	}

	qlen = len;
	return;
}

void flush(){
	if(qlen == 0) return;
	csock.send(2, queue, qlen);
	qlen = 0;
	return;
}