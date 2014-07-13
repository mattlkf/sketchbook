/*
Idea: I want to be able to browse the files on an SD card from an Arduino.

Commands to implement (in order):

cat <filename in current directory>
	Output the contents of a file.
	Exceptions:
		If the file doesn't exist

nano <filename>
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

rm <filename>
	Remove a file

*/


#include <Arduino.h>
#include <SD.h>
#include <string.h>

#define SD_CS_PIN 10

#define MAX_BUF_LEN 25

#define MAX_PATH_LEN 25

#define CL_LENGTH 50

void printLine(const char *, const uint8_t);
void handleLine(char *, const uint8_t);
void catFile(const char *, const uint8_t);
void lsDir(const char *, const uint8_t);
void changeDir(const char *, const uint8_t);
void nano(const char *, const uint8_t);
void rm(char *, const uint8_t);
void size(char *, const uint8_t);

char path[MAX_PATH_LEN] = "/";
uint8_t plen = 1;

void setup(){
	Serial.begin(115200);

	Serial.println("Waiting for SD card");
	while(!SD.begin(SD_CS_PIN)){
		delay(100);
	}

	Serial.println("Ready");
	//if(!SD.exists("/")) Serial.println("root dir doesn't exist");
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
					Serial.print("Error: max line is ");
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

	if(match(buf + s, t - s, "hello")){
		Serial.println("hi :D");
	}
	else if(match(buf + s, t - s, "cat")){

		if(!advanceIdxPair(buf, len, &s, &t)) return;

		// Serial.print("s: ");
		// Serial.println(s);
		// Serial.print("t: ");
		// Serial.println(t);
		// Serial.print("len: ");
		// Serial.println(len);
		
		catFile(buf + s, t-s);
	}
	else if(match(buf+s, t-s, "ls")){

		if(!advanceIdxPair(buf, len, &s, &t)){
			lsDir(path, plen);
		}
		else lsDir(buf + s, t-s);
	}	

	else if(match(buf+s, t-s, "cl")){

		for(uint8_t i = 0; i < CL_LENGTH; i++){
			Serial.println();
		}
	}

	else if(match(buf+s, t-s, "exists")){
		
		if(!advanceIdxPair(buf, len, &s, &t)) return;
		
		char tmp = buf[t];
		buf[t] = 0;

		Serial.print("File ");
		Serial.print(buf+s);

		if(SD.exists(buf+s)){
			Serial.println(" exists");
		}
		else{
			Serial.println(" doesn't exist");
		}

		buf[t] = tmp;
	}
	else if(match(buf+s, t-s, "mkdir")){
		if(!advanceIdxPair(buf, len, &s, &t)) return;

		char tmp = buf[t];
		buf[t] = 0;

		if(SD.exists(buf+s)){
			Serial.println("It already exists");
		}
		else{
			SD.mkdir(buf+s);
		}

		buf[t] = tmp;	
	}
	else if(match(buf+s, t-s, "cd")){
		advanceIdxPair(buf, len, &s, &t);

		changeDir(buf + s, t-s);
	}

	else if(match(buf+s, t-s, "nano")){
		advanceIdxPair(buf, len, &s, &t);
		
		char tmp = buf[t];
		buf[t] = 0;
		nano(buf + s, t-s);
		buf[t] = tmp;
	}

	else if(match(buf+s, t-s, "rm")){
		advanceIdxPair(buf, len, &s, &t);
		
		char tmp = buf[t];
		buf[t] = 0;
		rm(buf + s, t-s);
		buf[t] = tmp;
	}

	else if(match(buf+s, t-s, "sz")){
		advanceIdxPair(buf, len, &s, &t);
		
		char tmp = buf[t];
		buf[t] = 0;
		size(buf + s, t-s);
		buf[t] = tmp;
	}
	return;
}

bool validFileName(const char * f, const uint8_t len){
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
	if(!validFileName(f, len)){
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
		return;
	}

	File myFile = SD.open(fname);
	
	if(!myFile){
		Serial.println("Error: couldn't open file");
		return;
	}
	if(myFile.isDirectory()){
		Serial.println("Error: is a directory");
		return;
	}

    Serial.println("---");
	// read from the file until there's nothing else in it:
    char ch = 0;
    while (myFile.available()) {
    	// insert a \n, if we see a \r without a \n behind
    	if(ch == '\r'){
    		ch = myFile.read();
    		if(ch != '\n') Serial.write('\n');
    	}
		else{
			ch = myFile.read();
		}
        Serial.write(ch);
    }
    Serial.println("---");
	myFile.close();

	return;

}

void lsDir(const char * f, const uint8_t len){

	//the whole purpose of this is to make a null-terminated string
	char pathname[MAX_PATH_LEN];
	for(uint8_t i = 0; i<len;i++)
		pathname[i] = f[i];
	pathname[len] = 0; //add a null terminator
	
	Serial.print("Going to list files in ");
	Serial.println(pathname);
	

	File myFile = SD.open(pathname);
	if(!myFile){
		Serial.print("Error: could not open ");
		Serial.println(pathname);
		return;
	}

	if(!myFile.isDirectory()){
		Serial.print(pathname);
		Serial.println(" isn't a directory");
		return;
	}

	//delay(3000);
	myFile.rewindDirectory();
	while(1){
		File entry = myFile.openNextFile();
		if(!entry) break;
		Serial.println(entry.name());
		entry.close();
	}
	myFile.rewindDirectory();

	myFile.close();

	
	return;
}

void changeDir(const char * f, const uint8_t len){
	if(len == 0){
		return;
	}

	if(len > MAX_PATH_LEN){
		Serial.println("Error: exceeded max plen");
		return;
	}
	//the whole purpose of this is to make a null-terminated string
	char pathname[MAX_PATH_LEN];
	for(uint8_t i = 0; i<len;i++)
		pathname[i] = f[i];
	pathname[len] = 0; //add a null terminator

	
	if(pathname[0] == '.'){
		if(len == 2 && pathname[1] == '.'){
			while(plen > 1 && path[(--plen)-1] != '/');
			path[plen] = 0;
		}
	}
	else if(pathname[0] == '/'){
		// the len == 1 is for when we are cd-ing to root dir.
		// SD.exists("/") is false. somehow.
		if(len == 1 || SD.exists(pathname)){
			plen = 0;
			for(uint8_t i = 0; i<len;i++){
				path[plen++] = f[i];
			}
			path[plen] = 0;
		}
		else{
			Serial.print("Error: bad path ");
			Serial.println(pathname);
		}
	}
	else{
		if(len + plen > MAX_PATH_LEN){
			Serial.println("Error: exceeded max plen");
		}
		else{
			for(uint8_t i = 0; i<len;i++){
				path[plen++] = f[i];
			}
			path[plen] = 0;
			
			if(!SD.exists(path)){
				Serial.println("Error: new path is bad");
				plen -= len;
				path[plen] = 0;
			}	
		}
	}

	if(path[plen-1] != '/'){
		path[plen++] = '/';
		path[plen] = 0;
	}

	Serial.print("Path is now ");
	Serial.println(path);

	return;
}

void nano(const char *f, const uint8_t len){

	File myFile = SD.open(f, FILE_WRITE);

	if(!validFileName(f, len)){
		Serial.println("Error: filename must be in 8.3 format");
		return;
	}


	if(!myFile){
		Serial.print("Error: cannot open ");
		Serial.print(f);
		Serial.println(" for writing");
		Serial.println(len);
		return;
	}

	Serial.print("Enter a line for ");
	Serial.println(f);

	uint16_t count = 0;

	while(1){
		if(Serial.available()){
			char ch = Serial.read();
			Serial.print(ch);

			myFile.print(ch);
			++count;
			if(ch == '\r'){
				Serial.print("\n");
				myFile.print("\n");
				++count;
				break;
			}
		}
	}
	Serial.print("Wrote ");
	Serial.print(count);
	Serial.println(" characters");
	myFile.close();

	return;
}

void rm(char *f, const uint8_t len){
	if(!validFileName(f, len)){
		Serial.println("Error: filename must be in 8.3 format");
		return;
	}

	if(!SD.exists(f)){
		Serial.print(f);
		Serial.println(" doesn't exist.");
		return;
	}

	File myFile = SD.open(f);

	if(!myFile){
		Serial.print("File error");
		return;
	}

	if(myFile.isDirectory()){
		Serial.println("Is a directory");
		return;
	}

	myFile.close();

	SD.remove(f);

	Serial.print("Removed ");
	Serial.println(f);
	return;
}

void size(char *f, const uint8_t len){
	if(!validFileName(f, len)){
		Serial.println("Error: filename must be in 8.3 format");
		return;
	}

	if(!SD.exists(f)){
		Serial.print(f);
		Serial.println(" doesn't exist.");
		return;
	}

	File myFile = SD.open(f);

	if(!myFile){
		Serial.print("File error");
		return;
	}

	if(myFile.isDirectory()){
		Serial.println("Is a directory");
		return;
	}

	Serial.print("Size: ");
	Serial.print(myFile.size());
	Serial.println(" bytes");

	myFile.close();
	return;
}