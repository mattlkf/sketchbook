#include <Arduino.h>
#include <SoftwareSerial.h>
#include <CheeseSock.h>
#include <packet.h>

// nRF24 stuff
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#ifdef ACCEL
	#include <Wire.h>
	#include <Adafruit_Sensor.h>
	#include <Adafruit_ADXL345_U.h>
	/* Assign a unique ID to this sensor at the same time */
	Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

	void setupAccel();
	void readAccel();

	float avgTilt = 0;
	float varZ = 0;
	float meanZ = 0;
	float m2 = 0;
	uint16_t nsamples = 0;
#endif

#ifdef DEMO
	#define switchpin 3
	uint32_t demotimer;
	void demosend();
#endif

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

void queryAccel(const uint8_t * str, uint8_t len){
	//finalise the calculation of tilt and variance
	
	if(nsamples > 0) avgTilt /= (float) nsamples;
	else avgTilt = 0;

	if(nsamples < 2) varZ = 0;
	else varZ = m2/(float)(nsamples - 1);

	// print, if we want

	// Serial.print(F("Avg Tilt: "));
	// Serial.println(avgTilt);
	// Serial.print(F("Var Z: "));
	// Serial.println(varZ);

	//send it over CheeseSock to the GPSSAVER
	uint8_t buf[8];
	for(uint8_t i = 0 ; i< 4;i++) buf[i] = ((uint8_t *) &avgTilt)[i];
	for(uint8_t i = 4 ; i< 8;i++) buf[i] = ((uint8_t *) &varZ)[i-4];

	csock.send(3, buf, 8);
	//reset the tilt and variance to zero

	avgTilt = 0;
	varZ = 0;
	meanZ = 0;
	m2 = 0;
	nsamples = 0;

	return;
}

uint32_t timer = 0;

void setup(){
	Serial.begin(115200);
	Serial.println(F("Netmod Ready"));

	setupMirf();

#ifdef ACCEL
	setupAccel();
#endif

	csock.registerFn(1, sendok);
	csock.registerFn(2, callback);
	csock.registerFn(3, queryAccel);

	csock.begin(9600, 0);


	timer = millis();

#ifdef DEMO
	pinMode(switchpin, INPUT_PULLUP);
#endif
}

int main(){
	init();

	setup();

	while(1){
		csock.run();

#ifdef ACCEL
		if(millis() - timer > 10){
			readAccel();
			timer = millis();
		}
#endif

#ifdef DEMO
		if(millis() - demotimer > 50){
			if(digitalRead(switchpin) == LOW){
				Serial.println(F("switch low"));
				demosend();
			}
			demotimer = millis();
		}
#endif

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



#ifdef ACCEL
void setupAccel(){
	if(!accel.begin()){
		Serial.println("Failed to init sensor");
	}

	else accel.setRange(ADXL345_RANGE_4_G);
}

void readAccel(){
	/* Get a new sensor event */ 
	sensors_event_t event; 
	accel.getEvent(&event);

	/* Display the results (acceleration is measured in m/s^2) */
#ifdef PRINT_ACCEL
	Serial.print(F("X: ")); Serial.print(event.acceleration.x);
	Serial.print(F(" Y: ")); Serial.print(event.acceleration.y);
	Serial.print(F(" Z: ")); Serial.print(event.acceleration.z); Serial.print(" m/s^2 ");

	Serial.print(F("  Angle: "));
	Serial.println(atan2(event.acceleration.z, event.acceleration.x));
#endif

	nsamples++;
	// avgTilt += atan2(event.acceleration.z, event.acceleration.x);
	avgTilt += asin(event.acceleration.z / 10.8);
	float delta = event.acceleration.x - meanZ;
	meanZ = meanZ + delta / (float) nsamples;
	m2 = m2 + delta*(event.acceleration.x - meanZ);

	return;
}

#endif

#ifdef DEMO

void demosend(){
//calc current accel stats
	if(nsamples > 0) avgTilt /= (float) nsamples;
	else avgTilt = 0;

	if(nsamples < 2) varZ = 0;
	else varZ = m2/(float)(nsamples - 1);

//make a packet
	dataPkt pkt;

	pkt.hour = 0;
	pkt.min = 0;
	pkt.sec = 0;
	pkt.year = 0;
	pkt.mth = 0;
	pkt.day = 0;
	pkt.latitude = 0;
	pkt.longitude = 0;

	pkt.avgAngle = avgTilt;
	pkt.varVertical = varZ;


	pkt.seqId = 10000;
	pkt.srcId = millis()%1000;

//send it
	xmit((const uint8_t *)&pkt, sizeof(pkt));

//reset vars
	avgTilt = 0;
	varZ = 0;
	meanZ = 0;
	m2 = 0;
	nsamples = 0;

	return;

}

#endif