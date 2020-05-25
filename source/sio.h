#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_sio.h>
#include <stdlib.h>

// #define USE_INTERRUPTS

#define TIMEOUT_COUNT 10000
#define WAIT_62_5_MS()  for(volatile uint32_t tmout = 0; tmout < 50000; tmout++) // FIXME: How much is this?
#define WAIT_36_US()    for(volatile uint32_t tmout = 0; tmout <   500; tmout++) // FIXME: How much is this?

enum ESerialClock
{
    SIO_NORMAL_CLK_EXTERNAL = (0<<0)
  , SIO_NORMAL_CLK_256KHZ =   (1<<0)
  , SIO_NORMAL_CLK_2MHZ =     (3<<0)
};
// Baud Rate in MULTI & UART mode
enum EBaudRate
{
    SIO_9600_BPS =            (0<<0)
  , SIO_38400_BPS =           (1<<0)
  , SIO_57600_BPS =           (2<<0)
  , SIO_115200_BPS =          (3<<0)
};
// Communication mode
typedef unsigned int ESerialMode;


#define SIO_8BIT_MODE (0<<12) // Normal 8-bit communication mode
#define SIO_32BIT_MODE (1<<12)  // Normal 32-bit communication mode

#define SIO_START             (1<<7) // Start transfer
#define SIO_BUSY              (1<<7) // Transfer in progress
#define SIO_IRQ_ENABLE        (1<<14)
// NORMAL mode specific
#define SIO_NORMAL_SI         (1<<2)
#define SIO_NORMAL_SO         (1<<3)


#ifdef USE_INTERRUPTS
  #define GET_REPLY() \
    for(timeout = TIMEOUT_COUNT; (timeout != 0) && (bReceived_ == false); timeout--); \
    if(timeout != 0) \
    { \
      reply = rcvData_; \
      bReceived_ = false; \
    }
  #define GET_REPLY_NORMAL8()   GET_REPLY()
  #define GET_REPLY_NORMAL32()  GET_REPLY()
  #define GET_REPLY_MULTI() \
    for(timeout = TIMEOUT_COUNT; (timeout != 0) && (bReceived_ == false); timeout--); \
    if(timeout != 0) \
    { \
      *slave1 = rcvData_; \
      bReceived_ = false; \
    }
#else
  #define GET_REPLY_NORMAL8() \
    for(timeout = TIMEOUT_COUNT; (timeout != 0) && (REG_SIOCNT & SIO_START); timeout--); \
    if(timeout != 0) \
      reply = REG_SIODATA8
  #define GET_REPLY_NORMAL32() \
    for(timeout = TIMEOUT_COUNT; (timeout != 0) && (REG_SIOCNT & SIO_START); timeout--); \
    if(timeout != 0) \
      reply = REG_SIODATA32
  #define GET_REPLY_MULTI() \
    for(timeout = TIMEOUT_COUNT; (timeout != 0) && (REG_SIOCNT & SIO_START); timeout--); \
    if(timeout != 0) \
    { \
      *slave0 = REG_SIOMULTI0; \
      *slave1 = REG_SIOMULTI1; \
      *slave2 = REG_SIOMULTI2; \
      *slave3 = REG_SIOMULTI3; \
    }
#endif // USE_INTERRUPTS

int sio_init();
int setMode(ESerialMode mode);
uint8_t sendNormal8(uint8_t data);
uint32_t sendNormal32(uint32_t data);
