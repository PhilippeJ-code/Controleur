// hardware.c
//
#include "hardware.h"

void hardwareInit()
{
    usbInit();
    i2cInit();
    gt911Init();
    ioExpanderInit();
    rgbPanelInit();
    sdCardInit();
    hardLvglInit();
    nvsInit();
}