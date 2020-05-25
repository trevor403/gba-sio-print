#include <gba_console.h>
#include <gba_video.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "print.h"

int main(void) {
    irqInit();
    irqEnable(IRQ_VBLANK);

    consoleDemoInit();

    iprintf("\x1b[10;8HTesting...\n");
    iprintf("\x1b[11;8HHello world!\n");

    sio_init();
    setMode(SIO_32BIT_MODE);

    WAIT_62_5_MS();
    sendNormal32(0xCAFEFACE);
    WAIT_62_5_MS();
    sendNormal32(0xCAFEFACE);
    WAIT_62_5_MS();

    for (uint32_t frame = 0; ; frame++) {
        VBlankIntrWait();
        dprintf("Hello world %ld\n", frame);
    }
}


