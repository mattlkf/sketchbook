/* 
 * \file    CheeseSock.h
 * \ingroup 
 * \author  mattlkf
 * \date    2014-03-29
 *
 * A little thing to separate SenseActuateModule from the UniNet stack
 * Frames serial communications using SLIP-like escapes
 * Allows apps to register different member recv fns to different ports
 * Based on Alvin's LightSock
 */

#ifndef _CHEESESOCK_H
#define	_CHEESESOCK_H

#define CSOCK_MAX_PORTS 16
#define MAX_BUF 256

#include "CheeseSockApp.h"

#ifdef __linux__
 	#include <serialport_libserial.h>
#else
	#include <serialport.h>
#endif

#include <node.h>

class CheeseSock
{

public:
	// Serial stuff: on Linux, you can choose the tty_dev to use.
	#ifdef __linux__
		void init(const char * tty, uint32_t baudrate);
		void printAll(){ debug_state = all; }
	#else
		void init();
	#endif
	
	static CheeseSock* getInstance() { return instance_ ? instance_ : ( instance_ = new CheeseSock()); }

	void run();
	
	void registerApp(CheeseSockApp * app);
	void registerAppFn(uint8_t port, CheeseSockApp * app, CheeseSockApp::RecvFn);

	int8_t send(CheeseSockApp * app, const char * msg, int len);
	void send(uint8_t port, const uint8_t * msg, uint8_t len);

private:
	// Serial stuff: on Linux, we use the LibSerial-based version.
	#ifdef __linux__
		SerialPort_LibSerial * sp;
	#else
		SerialPort * sp;
	#endif

	static CheeseSock* instance_;

	// App and recv function registry
	CheeseSockApp * csapp[CSOCK_MAX_PORTS];
	CheeseSockApp::RecvFn fp[CSOCK_MAX_PORTS];

	CheeseSock();

	enum { none, all } debug_state;
	enum { discard, save } read_state;
	enum { escape, normal } esc_state;
	static const char preamble[], postamble[];
	uint8_t buf[MAX_BUF];
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
