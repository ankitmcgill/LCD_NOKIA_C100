/*************************************************
* NOKIA C100 TFT LIBRARY
*
* OCTOBER 30 2016
* ANKIT BHATNAGAR
* ANKIT.BHATNAGARINDIA@GMAIL.COM
* ***********************************************/

#ifndef _LCD_NOKIA_C100_H_
#define _LCD_NOKIA_C100_H_

#include <osapi.h>
#include "user_interface.h"
#include <ets_sys.h>
#include "gpio.h"
#include "ESP8266_SPI.h"

//LCD COMMAND DEFINES
#define LCD_NOKIA_C100_COMMAND_NOP		0x00
#define LCD_NOKIA_C100_COMMAND_SWRESET	0x01
#define LCD_NOKIA_C100_COMMAND_SLPIN	0x10
#define LCD_NOKIA_C100_COMMAND_SLPOUT	0x11
#define LCD_NOKIA_C100_COMMAND_PTLON	0x12
#define LCD_NOKIA_C100_COMMAND_NORON	0x13
#define LCD_NOKIA_C100_COMMAND_INVOFF	0x20
#define LCD_NOKIA_C100_COMMAND_GAMSET	0x26
#define LCD_NOKIA_C100_COMMAND_DISPOFF	0x28
#define LCD_NOKIA_C100_COMMAND_DISPON	0x29
#define LCD_NOKIA_C100_COMMAND_CASET	0x2A
#define LCD_NOKIA_C100_COMMAND_RASET	0x2B
#define LCD_NOKIA_C100_COMMAND_RAMWR	0x2C
#define LCD_NOKIA_C100_COMMAND_RBGSET	0x2D
#define LCD_NOKIA_C100_COMMAND_IDMOFF	0x38
#define LCD_NOKIA_C100_COMMAND_IDMON	0x39

//16-bit color definitions
//565 RGB FORMAT
//http://www.rinkydinkelectronics.com/calc_rgb565.php
//http://cloford.com/resources/colours/namedcol.htm
#define LCD_NOKIA_C100_COLOR_WHITE     0xFFFF
#define LCD_NOKIA_C100_COLOR_BLACK     0x0000
#define LCD_NOKIA_C100_COLOR_RED       0xF800
#define LCD_NOKIA_C100_COLOR_GREEN     0x07E0
#define LCD_NOKIA_C100_COLOR_BLUE      0x001F
#define LCD_NOKIA_C100_COLOR_CYAN      0x07FF
#define LCD_NOKIA_C100_COLOR_MAGENTA   0xF81F
#define LCD_NOKIA_C100_COLOR_YELLOW    0xFFE0
#define LCD_NOKIA_C100_COLOR_ORANGE    0xFD20

#define LCD_NOKIA_C100_RST_LOW  GPIO_OUTPUT_SET(4, 0)
#define LCD_NOKIA_C100_RST_HIGH GPIO_OUTPUT_SET(4, 1)

//FUNCTION POINTER TO SPI SEND FUNCTION
void (*LCD_NOKIA_C100_SPI_SEND_FUNCTION_POINTER)(uint8_t, uint8_t, uint32_t, uint32_t);

void LCD_NOKIA_C100_set_pins(void);
void LCD_NOKIA_C100_init(void);
void LCD_NOKIA_C100_send_command(uint8_t command);
void LCD_NOKIA_C100_send_data(uint8_t data);

void LCD_NOKIA_C100_reset_xy(void);
void LCD_NOKIA_C100_clear_screen(uint16_t color);

void LCD_NOKIA_C100_draw_ebu_test_pattern(void);

#endif
