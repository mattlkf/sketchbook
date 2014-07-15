#include <stdint.h>

struct dataPkt{
	uint16_t srcId;
	uint32_t seqId;

	uint8_t hour, min, sec;
	uint8_t year, mth, day;
	float latitude, longitude;
};

void printPkt(dataPkt * pkt);