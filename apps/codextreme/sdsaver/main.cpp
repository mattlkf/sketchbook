#include <Arduino.h>
#include <SD.h>

#define SD_CS_PIN 10 //it MUST be 10 in order for the SD library to work
#define SD_FILE_NAME "test4.txt"

File myFile;

void setup(){
	Serial.begin(9600);
	Serial.println("Press x to start");
	while(1){
		if(Serial.available()){
			if(Serial.read() == 'x') break;
		}
	}
	Serial.print("Initializing SD card...");
  
    if (!SD.begin(SD_CS_PIN)) {
    Serial.println("initialization failed!");
    return;
	}
	Serial.println("initialization done.");

	myFile = SD.open(SD_FILE_NAME, FILE_WRITE);

	if (myFile) {
	    Serial.print("Writing to ");
	    Serial.print(SD_FILE_NAME);
	    Serial.print("...");

	    for(int i = 0 ; i < 10; i++){

	    myFile.print(i);
	    myFile.println(": testing 1, 2, 3.");
		}
	    // close the file:
	    myFile.close();
	    Serial.println("done.");
	} else {
	    // if the file didn't open, print an error:
	    Serial.print("error opening ");
	    Serial.println(SD_FILE_NAME);
	}

	delay(1000);

	myFile = SD.open(SD_FILE_NAME);
  if (myFile) {
    Serial.print("Reading from ");
    Serial.println(SD_FILE_NAME);
    
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
        Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.print("error opening ");
    Serial.println(SD_FILE_NAME);
  }
  
}

int main(){
	init();

	setup();

	while(1){
		delay(500);
	}
}