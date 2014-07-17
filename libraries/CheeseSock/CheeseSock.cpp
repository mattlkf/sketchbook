/* 
 * \file    CheeseSock.h
 * \ingroup 
 * \author  mattlkf
 * \date    2014-03-29, 2014-07-15
 *
 * *Much* littler than the CheeseSock I wrote for SnS
 */

#include <CheeseSock.h>
#include <Arduino.h>

const char CheeseSock::preamble[] = "/<" , CheeseSock::postamble[] = "/>";

CheeseSock::CheeseSock(SoftwareSerial * ser){
	read_state = discard;
	esc_state = normal;
	debug_state = none;

	for(uint8_t i = 0; i < CSOCK_MAX_PORTS; i++){
		fp[i] = NULL;
	}

	sp = ser;
}

void CheeseSock::registerFn(uint8_t port, void (*fn)(const uint8_t *, uint8_t)){
	if(port < 0 || port >= CSOCK_MAX_PORTS) return; //port does not exist
	if(fp[port] != NULL) return; //port is occupied

	fp[port] = fn;
	return;
}

void CheeseSock::handleFrame(const uint8_t * str, const uint8_t len){
	if(len < 1) return;

	uint8_t port = str[0];

	fp[port](str + 1, len-1);
	return;
}

void CheeseSock::run(){
	readloop();
	return;
}

void CheeseSock::send(uint8_t port, const char * str){
	send(port, (const uint8_t *) str, strlen(str));
}

void CheeseSock::send(uint8_t port, const uint8_t * msg, uint8_t len){
	sp->write(postamble);
	sp->write(preamble);
	escapify((const uint8_t *) &port, 1, '/');
	escapify(msg, len, '/');
	sp->write(postamble);

	sp->write('\n');
	return;
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
	handleFrame(buf, len);
	len = 0;
}

/********** buffering *********************/
void CheeseSock::addtobuf(uint8_t ch){
	buf[len++] = ch;
	if(len == CSOCK_MAX_BUF){
		len = 0; //empty the full buffer
		Serial.println("Overflew");
		//Todo: raise an error on overflow
	}
}

/********** new, SLIP-based read loop *****/

void CheeseSock::readloop(){
	if(!sp->available()) return;

	uint8_t ch = sp->read();

	if(debug_state == all) Serial.write(ch);

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