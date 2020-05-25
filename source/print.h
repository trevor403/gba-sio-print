#include "sio.h"

void sio_dprintf(char *str, ...);

#define dprintf	sio_dprintf

#define __DOUTBUFSIZE	256
