/*************************************************
* NOKIA C100 TFT LIBRARY
* ARM VERSION
*
* 132 x 162 TFT LCD COLOR
* EACH PIXEL IS R(5) - G(6) - B(5)
* 
* MARCH 27 2017
* ANKIT BHATNAGAR
* ANKIT.BHATNAGARINDIA@GMAIL.COM
* ***********************************************/

#ifndef _LCD_NOKIA_C100_H_
#define _LCD_NOKIA_C100_H_

#include <stdint.h>
#if defined(STM32F072)
	#include "stm32f07x.h"
	#include "stm32f07x_gpio.h"
#endif
#if defined(STM32F103)
	#include "stm32f10x.h"
	#include "stm32f10x_gpio.h"
#endif

#define LCD_NOKIA_C100_RST_PIN_PORT	GPIOA
#define LCD_NOKIA_C100_RST_PIN_NUM	GPIO_Pin_0

#define LCD_NOKIA_C100_RST_LOW  GPIO_ResetBits(LCD_NOKIA_C100_RST_PIN_PORT, LCD_NOKIA_C100_RST_PIN_NUM)
#define LCD_NOKIA_C100_RST_HIGH GPIO_SetBits(LCD_NOKIA_C100_RST_PIN_PORT, LCD_NOKIA_C100_RST_PIN_NUM)

#define LCD_NOKIA_C100_ROW_MAX	131
#define LCD_NOKIA_C100_COL_MAX	161

//LCD COMMAND DEFINES
#define LCD_NOKIA_C100_COMMAND_NOP			0x00
#define LCD_NOKIA_C100_COMMAND_SWRESET	0x01
#define LCD_NOKIA_C100_COMMAND_SLPIN		0x10
#define LCD_NOKIA_C100_COMMAND_SLPOUT		0x11
#define LCD_NOKIA_C100_COMMAND_PTLON		0x12
#define LCD_NOKIA_C100_COMMAND_NORON		0x13
#define LCD_NOKIA_C100_COMMAND_INVOFF		0x20
#define LCD_NOKIA_C100_COMMAND_GAMSET		0x26
#define LCD_NOKIA_C100_COMMAND_DISPOFF	0x28
#define LCD_NOKIA_C100_COMMAND_DISPON		0x29
#define LCD_NOKIA_C100_COMMAND_CASET		0x2A
#define LCD_NOKIA_C100_COMMAND_RASET		0x2B
#define LCD_NOKIA_C100_COMMAND_RAMWR		0x2C
#define LCD_NOKIA_C100_COMMAND_RBGSET		0x2D
#define LCD_NOKIA_C100_COMMAND_MADCTR		0x36
#define LCD_NOKIA_C100_COMMAND_IDMOFF		0x38
#define LCD_NOKIA_C100_COMMAND_IDMON		0x39
#define LCD_NOKIA_C100_COMMAND_COLMOD		0x3A

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

//FUNCTION POINTER TO SPI SEND FUNCTION
static void (*LCD_NOKIA_C100_SPI_SEND_FUNCTION_POINTER)(uint16_t, uint8_t);

//FUNCTION POINTER FOR BLOCKING DELAY - US
static void (*LCD_NOKIA_C100_DELAY_US)(uint32_t);

void LCD_NOKIA_C100_Set_Pins(void);
void LCD_NOKIA_C100_Init(void);
void LCD_NOKIA_C100_Send_Command(uint8_t command);
void LCD_NOKIA_C100_Send_Data(uint8_t data);

void LCD_NOKIA_C100_Set_Xy_Area(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end);
void LCD_NOKIA_C100_Clear_Screen(uint16_t color);

void LCD_NOKIA_C100_Draw_Line_Horizontal(uint8_t x_start, uint8_t x_end, uint8_t y, uint8_t thickness, uint16_t color);
void LCD_NOKIA_C100_Draw_Line_Vertical(uint8_t x, uint8_t y_start, uint8_t y_end, uint8_t thickness, uint16_t color);

void LCD_NOKIA_C100_Draw_Rectangle_Outline(uint8_t x_start, uint8_t x_end, uint8_t y_start, uint8_t y_end, uint8_t border_size, uint16_t color);
void LCD_NOKIA_C100_Draw_Rectangle_Filled(uint8_t x_start, uint8_t x_end, uint8_t y_start, uint8_t y_end, uint16_t color);

void LCD_NOKIA_C100_Draw_Bitmap(uint8_t x_start, uint8_t x_end, uint8_t y_start, uint8_t y_end, uint32_t bitmap_flash_address, uint32_t bitmap_size_bytes);
void LCD_NOKIA_C100_Draw_Text(uint8_t x_start, uint8_t y_start, const uint8_t* font_map, const uint16_t (*font_descriptor_map)[3], uint8_t font_width, uint8_t font_height, char* str, uint8_t len, uint16_t color, uint16_t bgcolor);



#endif
