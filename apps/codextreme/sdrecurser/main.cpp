#include <Arduino.h>
#include <SD.h>

File root;

void printDirectory(File dir, int numTabs) {
   while(true) {

     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       // return to the first file in the directory
       dir.rewindDirectory();
       break;
     }
     // for (uint8_t i=0; i<numTabs; i++) {
     //   Serial.print('\t');
     // }
     Serial.print(entry.name());
     Serial.println();
     // if (entry.isDirectory()) {
     //   Serial.println("/");
     //   printDirectory(entry, numTabs+1);
     // } else {
     //   // files have sizes, directories do not
     //   Serial.print("\t\t");
     //   Serial.println(entry.size(), DEC);
     // }
   }
}
 

void setup()
{
  Serial.begin(9600);
  pinMode(10, OUTPUT);

  SD.begin(10);

  root = SD.open("/");

  printDirectory(root, 0);


//  root.close();
  Serial.println("done!");
}

void loop()
{
  // nothing happens after setup finishes.
}


int main(){
  init();
  setup();
  while(1){
    if(Serial.available()){
      if(Serial.read() == 'x'){
  //      root = SD.open("/");

        printDirectory(root, 0);


    //    root.close();
        Serial.println("done!");
      }
    }
    //loop();
  }
}