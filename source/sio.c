#include "sio.h"

// adapted from 
// https://github.com/Arawn-Davies/bricks-os/blob/92b4a2d77f9d748057657382ed9d32eebec9f090/kernel/arch/arm/gbands/gbaSerial.cpp

static bool bInitialized_ = false;
static bool bModeSet_ = false;
static bool bConnected_ = false;
static volatile bool bReceived_ = false;
static bool bMaster_;
static ESerialMode eMode_;
#ifdef USE_INTERRUPTS
static uint32_t rcvData_;
#endif // USE_INTERRUPTS

#ifdef USE_INTERRUPTS
void sio_isr() {
  // Receive data
  switch(eMode_)
  {
    case SIO_8BIT_MODE:
      rcvData_ = REG_SIODATA8;
      bReceived_ = true;
      break;
    case SIO_32BIT_MODE:
      rcvData_ = REG_SIODATA32;
      bReceived_ = true;
      break;
    default:
      ;
  };

  return;
}
#endif // USE_INTERRUPTS

int sio_init() {
  if(bInitialized_ == false) {
#ifdef USE_INTERRUPTS
    irqSet(IRQ_SERIAL, &sio_isr);
	irqEnable(IRQ_SERIAL);
#endif

    bInitialized_ = true;
  }

  return 0;
}

int setMode(ESerialMode mode) {
  bModeSet_     = false;
  bConnected_   = false;
  eMode_        = mode;

  if(bInitialized_ == false)
    return -1;

  switch(eMode_) {
    case SIO_8BIT_MODE:
    case SIO_32BIT_MODE:
      bMaster_ = false;
      // Initialize in connectNormal
      break;
    default:
      return -1;
  };

  bModeSet_ = true;

  return 0;
}

int connectNormal(bool master) {
  if(((eMode_ != SIO_8BIT_MODE) && (eMode_ != SIO_32BIT_MODE)) || (bModeSet_ == false))
    return -1;

  bMaster_ = master;
  if(bMaster_ == true)
  {
    REG_RCNT    = 0;
#ifdef USE_INTERRUPTS
    REG_SIOCNT  = eMode_ | SIO_IRQ_ENABLE | SIO_NORMAL_CLK_256KHZ;
#else
    REG_SIOCNT  = eMode_ | SIO_NORMAL_CLK_256KHZ;
#endif // USE_INTERRUPTS
    REG_SIOCNT |= SIO_START;
  }
  else
  {
    REG_RCNT    = 0;
#ifdef USE_INTERRUPTS
    REG_SIOCNT  = eMode_ | SIO_IRQ_ENABLE | SIO_NORMAL_CLK_EXTERNAL;
#else
    REG_SIOCNT  = eMode_ | SIO_NORMAL_CLK_EXTERNAL;
#endif // USE_INTERRUPTS
    REG_SIOCNT |= SIO_START;
  }

  bConnected_ = true;

  return 0;
}

uint8_t sendNormal8(uint8_t data) {
  uint8_t reply = 0;
  volatile uint32_t timeout;

  if((eMode_ != SIO_8BIT_MODE) && (bConnected_ == true))
    return -1;

  if(bMaster_ == true) {
    // Place data to be sent
    REG_SIODATA8 = data;

    // Wait for slave to be ready (SI == 0)
    for(timeout = TIMEOUT_COUNT; (timeout != 0) && (REG_SIOCNT & SIO_NORMAL_SI); timeout--); \

    // Set start flag
    REG_SIOCNT |= SIO_START;

    // Wait for, and get reply
    GET_REPLY_NORMAL8();
  } else {
    // Place data to be sent
    REG_SIODATA8 = data;

    // Set start flag & clear SO
    REG_SIOCNT = (REG_SIOCNT | SIO_START) & ~SIO_NORMAL_SO;

    // Wait for, and get reply
    GET_REPLY_NORMAL8();

    // Set SO
    REG_SIOCNT |= SIO_NORMAL_SO;
  }

  // Return replied data
  return reply;
}

// -----------------------------------------------------------------------------
uint32_t sendNormal32(uint32_t data) {
  uint32_t reply = 0;
  volatile uint32_t timeout;

  if((eMode_ != SIO_32BIT_MODE) && (bConnected_ == true))
    return -1;

  if(bMaster_ == true) {
    // Place data to be sent
    REG_SIODATA32 = data;

    // Wait for slave to be ready (SI == 0)
    for(timeout = TIMEOUT_COUNT; (timeout != 0) && (REG_SIOCNT & SIO_NORMAL_SI); timeout--); \

    // Set start flag
    REG_SIOCNT |= SIO_START;

    // Wait for, and get reply
    GET_REPLY_NORMAL32();
  } else {
    // Place data to be sent
    REG_SIODATA32 = data;

    // Set start flag & clear SO
    REG_SIOCNT = (REG_SIOCNT | SIO_START) & ~SIO_NORMAL_SO;

    // Wait for, and get reply
    GET_REPLY_NORMAL32();

    // Set SO
    REG_SIOCNT |= SIO_NORMAL_SO;
  }

  // Return replied data
  return reply;
}