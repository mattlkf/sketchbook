

#ifndef SERIAL_H
#define SERIAL_H

void serial_fill();
char serial_nextchar();
int serial_available();
int serial_init(const char * tty, const char * baud);
void serial_close();

#endif
