// io_expander.cpp
//
#include "ESP_IOExpander_Library.h"

#define I2C_MASTER_NUM 0

#define TP_RST 1
#define LCD_BL 2
#define LCD_RST 3
#define SD_CS 4
#define USB_SEL 5

ESP_IOExpander *expander = NULL;

extern "C" void ioExpanderInit()
{
    expander = new ESP_IOExpander_CH422G((i2c_port_t)I2C_MASTER_NUM, (uint8_t)ESP_IO_EXPANDER_I2C_CH422G_ADDRESS_000);
    expander->init();
    expander->begin();
    expander->multiPinMode(TP_RST | LCD_BL | LCD_RST | SD_CS | USB_SEL, OUTPUT);
    expander->multiDigitalWrite(TP_RST | LCD_BL | LCD_RST, HIGH);

    // When USB_SEL is HIGH, it enables FSUSB42UMX chip and gpio19,gpio20 wired CAN_TX CAN_RX, and then dont use USB Function
    expander->digitalWrite(USB_SEL, LOW);
    // use extend GPIO for SD card
    expander->digitalWrite(SD_CS, LOW);

}