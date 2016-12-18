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
#include "ESP8266_FLASH.h"

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
#define LCD_NOKIA_C100_COMMAND_MADCTR	0x36
#define LCD_NOKIA_C100_COMMAND_IDMOFF	0x38
#define LCD_NOKIA_C100_COMMAND_IDMON	0x39
#define LCD_NOKIA_C100_COMMAND_COLMOD	0x3A

//16-bit color definitions
//565 RGB FORMAT
//http://www.rinkydinkelectronics.com/calc_rgb565.php
//http://cloford.com/resources/colours/namedcol.htm
#define LCD_NOKIA_C100_COLOR_WHITE     	0xFFFF
#define LCD_NOKIA_C100_COLOR_BLACK     	0x0000
#define LCD_NOKIA_C100_COLOR_RED       	0xF800
#define LCD_NOKIA_C100_COLOR_GREEN     	0x07E0
#define LCD_NOKIA_C100_COLOR_BLUE      	0x001F
#define LCD_NOKIA_C100_COLOR_CYAN      	0x07FF
#define LCD_NOKIA_C100_COLOR_MAGENTA   	0xF81F
#define LCD_NOKIA_C100_COLOR_YELLOW    	0xFFE0
#define LCD_NOKIA_C100_COLOR_ORANGE   	0xFD20
#define LCD_NOKIA_C100_COLOR_LIGHT_PINK	0xFB33
#define LCD_NOKIA_C100_COLOR_TEAL_BLUE	0x03B6
#define LCD_NOKIA_C100_COLOR_LAWN_GREEN	0x7FE0

#define LCD_NOKIA_C100_RST_LOW  GPIO_OUTPUT_SET(4, 0)
#define LCD_NOKIA_C100_RST_HIGH GPIO_OUTPUT_SET(4, 1)

//FUNCTION POINTER TO SPI SEND FUNCTION
void (*LCD_NOKIA_C100_SPI_SEND_FUNCTION_POINTER)(uint8_t, uint8_t, uint32_t, uint32_t);

void LCD_NOKIA_C100_set_pins(void);
void LCD_NOKIA_C100_init(void);
void LCD_NOKIA_C100_send_command(uint8_t command);
void LCD_NOKIA_C100_send_data(uint8_t data);

void LCD_NOKIA_C100_set_xy_area(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end);
void LCD_NOKIA_C100_clear_screen(uint16_t color);

void LCD_NOKIA_C100_draw_bitmap(uint8_t x_start, uint8_t x_end, uint8_t y_start, uint8_t y_end, uint32_t bitmap_flash_address, uint32_t bitmap_size_bytes);
void LCD_NOKIA_C100_draw_text(uint8_t x_start, uint8_t y_start, const uint8_t* font_map, const uint16_t (*font_descriptor_map)[3], uint8_t font_width, uint8_t font_height, char* str, uint8_t len, uint16_t color, uint16_t bgcolor);
void LCD_NOKIA_C100_draw_filled_box(uint8_t x_start, uint8_t x_end, uint8_t y_start, uint8_t y_end, uint16_t color);
void LCD_NOKIA_C100_draw_outline_box(uint8_t x_start, uint8_t x_end, uint8_t y_start, uint8_t y_end, uint8_t border_size, uint16_t color);
void LCD_NOKIA_C100_draw_line_horizontal(uint8_t x_start, uint8_t x_end, uint8_t y, uint8_t thickness, uint16_t color);
void LCD_NOKIA_C100_draw_line_vertical(uint8_t x, uint8_t y_start, uint8_t y_end, uint8_t thickness, uint16_t color);

#endif
