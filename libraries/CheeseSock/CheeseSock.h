/* 
 * \file    CheeseSock.h
 * \ingroup 
 * \author  mattlkf
 * \date    2014-03-29, 2014-07-15
 *
 * *Much* littler than the CheeseSock I wrote for SnS
 */

#ifndef _CHEESESOCK_H
#define	_CHEESESOCK_H

#ifndef CSOCK_MAX_PORTS
	#define CSOCK_MAX_PORTS 16
#endif

#ifndef CSOCK_MAX_BUF
	#define CSOCK_MAX_BUF 128
#endif

#include "SoftwareSerial.h"

class CheeseSock
{

public:
	CheeseSock(SoftwareSerial *);

	void begin(uint16_t baud){
		if(!(baud == 9600 || baud == 57600)){
			return;
		}

		sp->begin(baud);
	}

	void begin(uint16_t baud, uint8_t debug){
		if(debug == 0)
			debug_state = none;
		else 
			debug_state = all;

		begin(baud);
	}

	void run();

	void registerFn(uint8_t port, void(*)(const uint8_t *, uint8_t));

	void send(uint8_t port, const uint8_t * msg, uint8_t len);
	void send(uint8_t port, const char * msg);

private:
	SoftwareSerial * sp;

	// App and recv function registry
	void (*fp[CSOCK_MAX_PORTS])(const uint8_t *, uint8_t);

	enum { none, all } debug_state;
	enum { discard, save } read_state;
	enum { escape, normal } esc_state;

	static const char preamble[], postamble[];

	uint8_t buf[CSOCK_MAX_BUF];
	uint8_t len;

	void escapify(const uint8_t * str, const uint8_t len, const uint8_t esc);
	void handleFrame(const uint8_t * str, const uint8_t len);

	void begin_slip();
	void end_slip();
	void addtobuf(uint8_t ch);
	void readloop();

	uint16_t checksum(const uint8_t * str, const uint8_t len);

};

#endif
