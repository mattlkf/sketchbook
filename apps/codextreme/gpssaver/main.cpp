#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <Adafruit_GPS.h>

#define SD_CS_PIN 10

// SoftwareSerial mySerial(2, 3);
SoftwareSerial mySerial(3,2); //modified to match my new wiring board
SoftwareSerial cheeseSerial(4,5);

Adafruit_GPS GPS(&mySerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO  true
#define GPSPRINT true

/* set to true to only log to SD when GPS has a fix, for debugging, keep it false */
#define LOG_FIXONLY true

#define CONFIG_FILE "config.txt"
#define DATA_FILE "datapkts.txt"

uint16_t gNodeId = 1; //a throwback to SnS :D
uint32_t seqNum = 0;

File logfile; //for raw GPS serial data
File datafile; //for processed data
File configfile; //for the persistent seqId number

struct dataPkt{
	uint16_t srcId;
	uint32_t seqId;

	uint8_t hour, min, sec;
	uint8_t year, mth, day;
	float latitude, longitude;
};

int freeRam () 
{
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void updateSeqNum(){
	seqNum++;

	File myFile = SD.open(CONFIG_FILE, FILE_WRITE);
	myFile.seek(0);
	myFile.write((char*)&seqNum, sizeof(seqNum));
	myFile.close();

	// configfile.seek(0);
	// configfile.write((char*)&seqNum, sizeof(seqNum));
	// configfile.flush();
}

void printPkt(dataPkt * pkt){
	Serial.print("Packet ");
	Serial.println(pkt->seqId);

	Serial.print("Location: ");
	Serial.print(pkt->latitude, 4);
	Serial.print(", "); 
	Serial.println(pkt->longitude, 4);

    return;
}

void makePkt(dataPkt * pkt){
	pkt->hour = GPS.hour;
	pkt->min = GPS.minute;
	pkt->sec = GPS.seconds;
	pkt->year = GPS.year;
	pkt->mth = GPS.month;
	pkt->day = GPS.day;
	pkt->latitude = GPS.latitude;
	pkt->longitude = GPS.longitude;

	pkt->seqId = seqNum;
	pkt->srcId = gNodeId;

	updateSeqNum();
	printPkt(pkt);
	
    Serial.print("Ram: ");
    Serial.println(freeRam());

	return;
}

void savePkt(dataPkt * pkt){
	uint8_t written = datafile.write((char*)pkt, sizeof(*pkt));
	datafile.flush();
	Serial.print("Wrote ");
	Serial.print(sizeof(*pkt));
	Serial.print("/");
	Serial.print(written);
	Serial.println(" to file");
	return;
}

File openWriteFile(char * name){
	File filePtr = SD.open(name, FILE_WRITE);
	if(!filePtr){
		Serial.print(F("Couldn't open "));
	}
	else{
		Serial.print("Opened: ");
	}
	Serial.println(name);

	return filePtr;
}

void setupFile(){
	Serial.print(F("Initializing SD..."));
  
    while(!SD.begin(SD_CS_PIN)) {
	    Serial.print(".");
	    delay(1000);
	}
	Serial.println("Done.");

	char filename[15];
  	//strcpy(filename, "gpslog00.txt");
  	for (uint8_t i = 0; i < 1000; i++) {
		sprintf(filename, "gps%03d.txt", i);
		// create if does not exist, do not open existing, write, sync after write
		if (! SD.exists(filename)) {
			break;
		}
	}

	logfile = openWriteFile(filename);
	datafile = openWriteFile(DATA_FILE);
	//configfile = openWriteFile(CONFIG_FILE);

  	delay(2000);

  	return;
}

void setupGPS(){

	// 9600 NMEA is the default baud rate for MTK - some use 4800
	GPS.begin(9600);

	// You can adjust which sentences to have the module emit, below

	// uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
	//GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
	// uncomment this line to turn on only the "minimum recommended" data for high update rates!
	GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
	// uncomment this line to turn on all the available data - for 9600 baud you'll want 1 Hz rate
	//GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_ALLDATA);

	// Set the update rate
	// 1 Hz update rate
	//GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
	// 5 Hz update rate- for 9600 baud you'll have to set the output to RMC or RMCGGA only (see above)
	GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
	// 10 Hz update rate - for 9600 baud you'll have to set the output to RMC only (see above)
	//GPS.sendCommand(PMTK_SET_NMEA_UPDATE_10HZ);

	// Request updates on antenna status, comment out to keep quiet
	// GPS.sendCommand(PGCMD_ANTENNA);

	return;
}


void setupConfig(){
	if(!SD.exists(CONFIG_FILE)){
		File myFile = SD.open(CONFIG_FILE, FILE_WRITE);
		seqNum = 0;
		myFile.write((char*)&seqNum, sizeof(seqNum));
		myFile.close();

		Serial.println("Newfile");
	}

	File myFile = SD.open(CONFIG_FILE);
	myFile.read((char*)&seqNum, sizeof(seqNum));
	myFile.close();

	Serial.print("SeqNum: ");
	Serial.println(seqNum);

	return;
}

void setup(){
	// connect at 115200 so we can read the GPS fast enuf and
  // also spit it out
  Serial.begin(115200);

  Serial.println("setupFile");
  delay(500);
  setupFile();


  Serial.println("setupConfig");
  delay(500);
  setupConfig();


  Serial.println("setupGPS");
  delay(500);
  setupGPS();

}

// void printGPS(){
// 	Serial.print("\nTime: ");
// 	Serial.print(GPS.hour, DEC); Serial.print(':');
// 	Serial.print(GPS.minute, DEC); Serial.print(':');
// 	Serial.print(GPS.seconds, DEC); Serial.print('.');
// 	Serial.println(GPS.milliseconds);
// 	Serial.print("Date: ");
// 	Serial.print(GPS.day, DEC); Serial.print('/');
// 	Serial.print(GPS.month, DEC); Serial.print("/20");
// 	Serial.println(GPS.year, DEC);
// 	Serial.print("Fix: "); Serial.print((int)GPS.fix);
// 	Serial.print(" quality: "); Serial.println((int)GPS.fixquality); 
// 	if (GPS.fix) {
// 		Serial.print("Location: ");
// 		Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
// 		Serial.print(", "); 
// 		Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);

// 		Serial.print("Speed (knots): "); Serial.println(GPS.speed);
// 		Serial.print("Angle: "); Serial.println(GPS.angle);
// 		Serial.print("Altitude: "); Serial.println(GPS.altitude);
// 		Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
// 	}
// 	return;
// }

int main(){
	init();
	setup();

	while(1){
		char c = GPS.read();
		// if you want to debug, this is a good time to do it!
		if (GPSECHO)
			Serial.write(c);  

		if(GPS.newNMEAreceived()){
		    char *stringptr = GPS.lastNMEA();
		    uint8_t stringsize = strlen(stringptr);
		    uint8_t writtensize;
		    writtensize = logfile.write((uint8_t *)stringptr, stringsize);
		    if (stringsize != writtensize){    //write the string to the SD file
		    	Serial.print("Write error ");
		    	Serial.print(stringsize);
		    	Serial.print(" ");
		    	Serial.println(writtensize);
		    }
		    Serial.println();
		    logfile.write('\r\n');
		    logfile.flush();
		    if (strstr(stringptr, "RMC")){
			    GPS.parse(stringptr);

			    if(GPS.fix){
				    dataPkt pkt;
				    makePkt(&pkt);
				    savePkt(&pkt);
				    // sendPkt(&pkt);
				}
			    else{
			    	Serial.println("No fix");
				    Serial.print("Ram: ");
				    Serial.println(freeRam());
			    }

			}
			else{
				Serial.print("Not RMC: ");
				for(uint8_t i = 0; i<5;i++) Serial.write(stringptr[i]);
				Serial.write('\n');
			}

		}
	}

	return 0;
}