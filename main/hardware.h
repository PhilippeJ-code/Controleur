// hardware.h
//
#pragma once

#include "i2c.h"
#include "gt911.h"
#include "rgbPanel.h"
#include "hardLvgl.h"
#include "sdCard.h"
#include "nvsFlash.h"

extern void ioExpanderInit();
extern void usbInit();
extern void usbLoop();

extern bool usbIsConnected();
extern void usbSend(uint8_t data[], size_t size);
extern bool usbReceive(uint8_t *data, size_t data_len);

extern void usbSetNbrOk(uint32_t pNbrOk);
extern uint32_t usbGetNbrOk();

void hardwareInit();

