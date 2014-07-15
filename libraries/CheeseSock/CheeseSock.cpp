/* 
 * \file    CheeseSock.cpp
 * \ingroup 
 * \author  mattlkf
 * \date    2014-03-31
 *
 * A little thing to separate SenseActuateModule from the UniNet stack
 * Frames serial communications using SLIP-like escapes
 * Allows apps to register different member recv fns to different ports
 * Based on Alvin's LightSock
 */

#include <CheeseSock.h>

CheeseSock* CheeseSock::instance_ = NULL;

const char CheeseSock::preamble[] = "/<" , CheeseSock::postamble[] = "/>";

CheeseSock::CheeseSock(){
	read_state = discard;
	esc_state = normal;
	debug_state = none;

	for(uint8_t i = 0; i < CSOCK_MAX_PORTS; i++){
		csapp[i] = NULL;
		fp[i] = NULL;
	}
}

#ifdef __linux__
void CheeseSock::init(const char * tty, uint32_t baudrate){
	sp = new SerialPort_LibSerial(tty);
	
    if(!(baudrate == 115200 || baudrate == 57600)){
		printf("Please choose baudrate 57600 or 115200\n");
		baudrate = 57600;
	}

	sp->begin(baudrate);

	printf("Listening on serial %s at %d\n", tty, baudrate);
	return;
}
#else
void CheeseSock::init(){
	sp = &gSerial0;

	return;
}
#endif

void CheeseSock::registerApp(CheeseSockApp * app){
	// default: register the recv() function of the app to the port.
	registerAppFn(app->getPort(), app, &CheeseSockApp::recv);
	return;
}

void CheeseSock::registerAppFn(uint8_t port, CheeseSockApp * app, CheeseSockApp::RecvFn fn){
	if(port < 0 || port >= CSOCK_MAX_PORTS) return; //port does not exist
	if(csapp[port] != NULL || fp[port] != NULL) return; //port is occupied

	csapp[port] = app;
	fp[port] = fn;
	return;
}

void CheeseSock::handleFrame(const uint8_t * str, const uint8_t len){
	if(len < 1) return;

	uint8_t port = str[0];

	uint16_t src = 0; //Matt: temporary. Do CheeseSockApps have a notion of source?
	(csapp[port]->*fp[port])(src, (const char*) str+1, len-1);
	return;
}

void CheeseSock::run(){
//	gSerial0.write("hi\n");
	readloop();
	return;
}

// type are for compatibility with NetworkApp
int8_t CheeseSock::send(CheeseSockApp * app, const char * msg, int len){
	if(len > 255) return -1; // we don't send such long messages!

	send(app->getPort(), (const uint8_t *) msg, (uint8_t) len);
	return 0;
}

void CheeseSock::send(uint8_t port, const uint8_t * msg, uint8_t len){
	uint16_t csum = checksum(msg, len);

	sp->write(postamble);
	sp->write(preamble);
	escapify((const uint8_t *) &csum, 2, '/'); 
	escapify((const uint8_t *) &port, 1, '/');
	escapify(msg, len, '/');
	sp->write(postamble);

	sp->write('\n');
	return;
}

uint16_t CheeseSock::checksum(const uint8_t * str, const uint8_t len){
	return (uint16_t) 90*256+90;
}

void CheeseSock::escapify(const uint8_t * str, const uint8_t len, const uint8_t esc){
	for(uint8_t i = 0; i < len; i++){
		if(str[i] == esc) sp->write(esc); // "escaping the escape"
		sp->write(str[i]);
	}
}


// The functions below are taken almost wholesale from DIP; minor name changes made

/********** frame start and end functions *****/

void CheeseSock::begin_slip(){
	read_state = save;
	len = 0;
}

void CheeseSock::end_slip(){
	read_state = discard;
	if(len < 2) return;
	//Todo: only pass the frame to handleFrame if it passes the checksum
	handleFrame(buf + 2, len - 2);
	len = 0;
}

/********** buffering *********************/
void CheeseSock::addtobuf(uint8_t ch){
	buf[len++] = ch;
	if(len == MAX_BUF){
		len = 0; //empty the full buffer
		//Todo: raise an error on overflow
	}
}

/********** new, SLIP-based read loop *****/

void CheeseSock::readloop(){
	if(!sp->available()) return;

	uint8_t ch = sp->read();

	if(debug_state == all) printf("%c", ch);

	if(esc_state == normal){
		if(ch == '/') esc_state = escape;
		else if(read_state == save) addtobuf(ch);
	}
	else { //previous char was an escape char
		if(ch == '/') addtobuf(ch);
		else if(read_state == discard && ch == '<') begin_slip();
		else if(read_state == save    && ch == '>') end_slip();

		if(read_state == save || ch != '/') esc_state = normal;
	}

}