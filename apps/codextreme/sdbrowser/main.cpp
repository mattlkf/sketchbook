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

void printLine(const char *, const uint8_t len);
void handleLine(const char *, const uint8_t len);

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

bool match(const char * a, uint8_t s, uint8_t e, const char * b){
	//debug
	Serial.print("Matching ");
	for(uint8_t i = s; i < e; i++) Serial.print(a[i]);
	Serial.print(" with ");
	Serial.println(b);

	a += s;
	e -= s;

	for(uint8_t i = 0; b[i] && i < e; i++){
		if(a[i] == b[i]) continue;
		return false;
	}

	return true;
}

void handleLine(const char * buf, const uint8_t len){
	// printLine(buf, len);
	
	uint8_t s = 0;
	uint8_t t = 0;
	//advance s to the start of the next token, or end-line (s == len).
	while(s < len && buf[s] == ' ') ++s;
	//advance t to the first non-token character after s, or end-line (t == len).
	t = s;
	while(t < len && buf[t] != ' ') ++t;

	if(match(buf, s, t, "hello")){
		Serial.println("hi :D");
	}

	return;
}
