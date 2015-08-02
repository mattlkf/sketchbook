#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>

#define MAX_BUF 100
#define VERBOSE

#define v_offset 0

SoftwareSerial mySerial(2,3);
Adafruit_ADS1115 ads;  /* 16-bit version */

/* -- Timing stuff -- */
const uint32_t sample_period = 10; // 5ms -> 200 Hz
const uint32_t output_period = 1000; // 1000ms -> 1 Hz

/* -- Calibration timing stuff -- */
const uint32_t calib_delay = 2500;
const uint32_t calib_period = 10;
const uint32_t calib_length = 2500;

/* -- Line reading stuff -- */
char buf[MAX_BUF];
uint8_t bn = 0;
void readChar();
void handleLine();
int endLine();

const float c_mul = 0.675675676; // 0.125mV / (185 mV/A) * (1A / 1000mA)
const float v_mul = 0.125; // 0.125mV per bit
float c_sum = 0;
float v_sum = 0;
float p_sum = 0;
float r_sum = 0;
uint32_t sn = 0;
void reset_sampling();

void sample();

void output();

int16_t r0_latest, r1_latest, r2_latest, r3_latest;
void getLatest();

float r_avg[4];
void calibrate();

int main(){
    init();

    ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
    ads.begin();

    Serial.begin(9600);

    Serial.println("Calibrating...");
    calibrate();

    uint32_t sample_timer = millis();
    uint32_t output_timer = millis();

    while(1){
        if(millis() > sample_timer + sample_period){
            sample_timer = millis();
            sample();
            getLatest();
        }

        if(millis() > output_timer + output_period){
            output_timer = millis();
            output();
            reset_sampling();
        }

        if(mySerial.available()){
            readChar();
        }

        if(endLine()){
            handleLine();
        }

    }

    return 0;
}

void readChar(){
    buf[bn] = mySerial.read();
    if(bn < MAX_BUF) ++bn;
}

int endLine(){
    return bn && (buf[bn-1] == '\n');
}

void handleLine(){
    Serial.write(buf, bn);
}

void sample(){
    int16_t r_v = ads.readADC_SingleEnded(1);
    int16_t r_c = ads.readADC_SingleEnded(3);  

    // subtract offsets
    r_v -= (int16_t) r_avg[1];
    r_c -= (int16_t) r_avg[3];

    float c = (float)r_c * c_mul;
    float v = (float)r_v * v_mul;

    c_sum += c;
    v_sum += v;
    p_sum += c * v / 1000.0; // the 1000 is to ensure power is in mW.
    r_sum += r_c;

    ++sn;    
}

void reset_sampling(){
    sn = 0;
    c_sum = 0;
    v_sum = 0;
    p_sum = 0;
    r_sum = 0;
}

#ifdef VERBOSE
void output(){
    Serial.print("Current: ");
    Serial.print(c_sum / (float)sn);
    Serial.print("mA ");

    Serial.print("Voltage: ");
    Serial.print(v_sum / (float)sn);
    Serial.print("mV ");

    Serial.print("Power: ");
    Serial.print(p_sum / (float)sn);
    Serial.print("mW ");
    
    Serial.print(" R3: ");
    Serial.print(r_sum / (float)sn);

#ifdef GETLATEST
    Serial.print(" Latest: ");
    Serial.print("R0: ");
    Serial.print(r0_latest);
    Serial.print("R1: ");
    Serial.print(r1_latest);
    Serial.print("R2: ");
    Serial.print(r2_latest);
    Serial.print("R3: ");
    Serial.print(r3_latest);
#endif

    Serial.println();
}
#else
void output(){
    Serial.print(c_sum / (float)sn);
    Serial.print(v_sum / (float)sn);
    Serial.print(p_sum / (float)sn);

    Serial.println();    
}
#endif

void calibrate(){
    delay(calib_delay);

    uint32_t nsamples = 0;
    uint8_t i;
    for(i=0;i<4;i++) r_avg[i] = 0;

    uint32_t start = millis();
    uint32_t lastSample = start;
    
    uint32_t t = millis();
    while(t - start < calib_length){
        if(t - lastSample > calib_period){
            lastSample = t;
            for(i=0;i<4;i++){
                r_avg[i] += (float) ads.readADC_SingleEnded(i);
            }
            ++nsamples;
        }
        t = millis();
    }

    for(i=0;i<4;i++) r_avg[i] /= (float) nsamples;

#ifdef VERBOSE
    Serial.print("Samples: ");
    Serial.println(nsamples);
    for(i=0;i<4;i++){
        Serial.print(r_avg[i]);
        Serial.println();
    }
#endif
}


void getLatest(){
#ifdef GETLATEST
    r0_latest = ads.readADC_SingleEnded(0);
    r1_latest = ads.readADC_SingleEnded(1);
    r2_latest = ads.readADC_SingleEnded(2);
    r3_latest = ads.readADC_SingleEnded(3);    
#endif 
}