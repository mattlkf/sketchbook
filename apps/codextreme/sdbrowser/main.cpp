/*
Idea: I want to be able to browse the files on an SD card from an Arduino.

Commands to implement (in order):

cat <filename in current directory>
	Output the contents of a file.
	Exceptions:
		If the file doesn't exist

touch <filename>
	Create a new file for writing.
	Exceptions:
		If the file already exists.

cd <name of next directory, must be a valid directory, or .. meaning the previous directory>
	Change directory.
	Exceptions:
		If the directory doesn't exist (going to an inner directory)
		If we are at the root and we try to go to an upper directory.

pwd
	Print the current directory.

ls
	Print the names of all the files and directories in the curent directory.

*/


#include <Arduino.h>
#include <SD.h>
#include <string.h>

#define MAX_BUF_LEN 255

#define SD_CS_PIN 10

void printLine(const char *, const uint8_t);
void handleLine(const char *, const uint8_t);
void catFile(const char *, const uint8_t);


void setup(){
	Serial.begin(9600);

	Serial.println("Waiting for SD card");
	while(!SD.begin(SD_CS_PIN)){
		delay(100);
	}

	Serial.println("Ready");
}

int main(){
	init();

	setup();


	uint8_t flag = 0;
	char buf[MAX_BUF_LEN];
	uint8_t blen = 0;
	while(1){
		if(Serial.available()){
			char ch = Serial.read();
			if(ch == '\r') Serial.print("\n");
			Serial.print(ch);
			// if(ch == '\r') Serial.println("!");
			
			if(ch == '\r'){
				if(flag){
					Serial.print("Error: max line is");
					Serial.println(MAX_BUF_LEN);
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

void printLine(const char * buf, const uint8_t len){
	Serial.println("Read:");
	for(int i = 0; i<len;i++) Serial.print(buf[i]);
	Serial.print("\r\n");

	return;
}

bool match(const char * a, uint8_t alen, const char * b){
	//debug
	Serial.print("Matching ");
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

void handleLine(const char * buf, const uint8_t len){
	// printLine(buf, len);
	
	uint8_t s = 0, t = 0;
	//advance s to the start of the next token, or end-line (s == len).
	for(s = t; s < len && buf[s] == ' '; ++s);
	//advance t to the first non-token character after s, or end-line (t == len).
	for(t = s; t < len && buf[t] != ' '; ++t);

	if(match(buf + s, t - s, "hello")){
		Serial.println("hi :D");
	}
	else if(match(buf + s, t - s, "cat")){
		for(s = t; s < len && buf[s] == ' '; ++s);
		for(t = s; t < len && buf[t] != ' '; ++t);

		// Serial.print("s: ");
		// Serial.println(s);
		// Serial.print("t: ");
		// Serial.println(t);
		// Serial.print("len: ");
		// Serial.println(len);
		
		catFile(buf + s, t-s);
	}

	return;
}

bool validFile(const char * f, const uint8_t len){
	uint8_t point = len;
	uint8_t i;

	for(i=0;i<len;i++){
		if(f[i] == '.'){
			if(point == len) point = i;
			else {
				Serial.println("Too many dots!");
				return false; // more than one dot!
			}
		}
	}

	if(point == 0 || point > 8 ){
		Serial.println("Name before point invalid");
		return false; //must be from 1 to 8 chars
	}
	if(len - point != 4){
		Serial.println(len);
		Serial.println(point);
		Serial.println("Post-dot must be len 3");
		return false; // must be exactly 4 nonpoint chars including the dot
	}
	for(i=0;i<len;i++){
		if(i != point)
			if(!((f[i]>='a' && f[i]<='z') || (f[i]>='A' && f[i]<='Z') || (f[i]>='0' && f[i]<='9')) ){
			Serial.println("Non-alphanumeric element found");
			return false;
		}
	}

	return true;
}

void catFile(const char * f, const uint8_t len){
	printLine(f, len);
	if(!validFile(f, len)){
		Serial.println("Error: filename must be in 8.3 format");
		return;
	}

	//the whole purpose of this is to make a null-terminated string
	char fname[13];
	for(uint8_t i = 0; i<len;i++)
		fname[i] = f[i];
	fname[len] = 0; //add a null terminator

	if(!SD.exists(fname)){
		Serial.print(fname);
		Serial.println(" doesn't exist.");	
	}

	File myFile = SD.open(fname);
	
	if(!myFile){
		Serial.println("Error: couldn't open file");
	}

    Serial.println("---");
	// read from the file until there's nothing else in it:
    while (myFile.available()) {
        Serial.write(myFile.read());
    }
    Serial.println("---");
	myFile.close();

	return;

}
