#include <stdarg.h>
#include <stdio.h>
#include "print.h"

char __outstr[__DOUTBUFSIZE];

void sio_dprintf(char *str, ...) {
	va_list args;
	char *string = __outstr;

	va_start(args, str);
	vsprintf(__outstr,str,args);
	va_end(args);

	while (*string)
		sendNormal32(*string++);

    sendNormal32(0x00000000);
}