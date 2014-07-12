#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <Adafruit_GPS.h>

#define SD_CS_PIN 10

SoftwareSerial mySerial(2, 3);

Adafruit_GPS GPS(&mySerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO  false

/* set to true to only log to SD when GPS has a fix, for debugging, keep it false */
#define LOG_FIXONLY true

#define DATA_SAVE_INTERVAL 1000

uint32_t timer;

File logfile; //for raw GPS serial data
File datafile; //for processed data


void setupFile(){
	Serial.print("Initializing SD card...");
  
    if (!SD.begin(SD_CS_PIN)) {
    Serial.println("initialization failed!");
    return;
	}
	Serial.println("initialization done.");

	char filename[15];
  	strcpy(filename, "gpslog00.txt");
  	for (uint8_t i = 0; i < 100; i++) {
		filename[6] = '0' + i/10;
		filename[7] = '0' + i%10;
		// create if does not exist, do not open existing, write, sync after write
		if (! SD.exists(filename)) {
			break;
		}
	}

	logfile = SD.open(filename, FILE_WRITE);
  	if( ! logfile ) {
    	Serial.print("Couldnt create "); Serial.println(filename);
  	}
  	Serial.print("Writing to "); Serial.println(filename);

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
  GPS.sendCommand(PGCMD_ANTENNA);

  return;
}

void setup(){
	// connect at 115200 so we can read the GPS fast enuf and
  // also spit it out
  Serial.begin(115200);
  Serial.println("Adafruit GPS library basic test!");


  setupFile();

  setupGPS();
  delay(1000);

  timer = millis();


}

int main(){
	init();
	setup();

	while(1){
		char c = GPS.read();
		// if you want to debug, this is a good time to do it!
		if (GPSECHO)
			if (c) UDR0 = c;  

		if(GPS.newNMEAreceived()){
		    char *stringptr = GPS.lastNMEA();
		    uint8_t stringsize = strlen(stringptr);
		    if (stringsize != logfile.write((uint8_t *)stringptr, stringsize))    //write the string to the SD file
		    	Serial.println("Write error");
		    logfile.write('\n');
		    if (strstr(stringptr, "RMC"))   logfile.flush();
		    Serial.println();

		    GPS.parse(GPS.lastNMEA());
		}

		// if millis() or timer wraps around, we'll just reset it
		if (timer > millis())  timer = millis();

		// approximately every 2 seconds or so, print out the current stats
		if (millis() - timer > 2000) { 
			timer = millis(); // reset the timer

			// if(!LOG_FIXONLY || GPS.fix()){

			// }

			Serial.print("\nTime: ");
			    Serial.print(GPS.hour, DEC); Serial.print(':');
			    Serial.print(GPS.minute, DEC); Serial.print(':');
			    Serial.print(GPS.seconds, DEC); Serial.print('.');
			    Serial.println(GPS.milliseconds);
			    Serial.print("Date: ");
			    Serial.print(GPS.day, DEC); Serial.print('/');
			    Serial.print(GPS.month, DEC); Serial.print("/20");
			    Serial.println(GPS.year, DEC);
			    Serial.print("Fix: "); Serial.print((int)GPS.fix);
			    Serial.print(" quality: "); Serial.println((int)GPS.fixquality); 
			    if (GPS.fix) {
			      Serial.print("Location: ");
			      Serial.print(GPS.latitude, 4); Serial.print(GPS.lat);
			      Serial.print(", "); 
			      Serial.print(GPS.longitude, 4); Serial.println(GPS.lon);
			      
			      Serial.print("Speed (knots): "); Serial.println(GPS.speed);
			      Serial.print("Angle: "); Serial.println(GPS.angle);
			      Serial.print("Altitude: "); Serial.println(GPS.altitude);
			      Serial.print("Satellites: "); Serial.println((int)GPS.satellites);
			    }
    	}

	}

	return 0;
}