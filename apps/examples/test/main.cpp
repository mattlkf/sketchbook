#include <Arduino.h>
#include <Narcoleptic.h>

int array[100] = {0};

int main(){
	init();
	pinMode(13, OUTPUT);
	int a = 1, b = 2, c = 3;
	char name[] = "Matthew";
	while(1){
		digitalWrite(13, LOW);
		delay(500);
		digitalWrite(13, HIGH);
		delay(100);
		name[0]++;
	}
}