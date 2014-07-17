#include <stdint.h>

struct dataPkt{
	uint16_t srcId;
	uint32_t seqId;

	uint8_t hour, min, sec;
	uint8_t year, mth, day;
	float latitude, longitude;
};

struct reqPkt{
	uint32_t s[4];
	uint32_t e[4];
};

void printPkt(const dataPkt * pkt);
void printPkt(const reqPkt * pkt);