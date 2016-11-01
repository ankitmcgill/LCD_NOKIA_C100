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

//16-bit color definitions
//565 RGB FORMAT
//http://www.rinkydinkelectronics.com/calc_rgb565.php
//http://cloford.com/resources/colours/namedcol.htm

#define WHITE     0xFFFF
#define BLACK     0x0000
#define RED       0xF800
#define GREEN     0x07E0
#define BLUE      0x001F
#define CYAN      0x07FF
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0
#define ORANGE    0xFD20

#define LCD_NOKIA_C100_RST_LOW  GPIO_OUTPUT_SET(4, 0)
#define LCD_NOKIA_C100_RST_HIGH GPIO_OUTPUT_SET(4, 1)

//FUNCTION POINTER TO SPI SEND FUNCTION
void (*LCD_NOKIA_C100_SPI_SEND_FUNCTION_POINTER)(uint8_t, uint8_t, uint32_t, uint32_t);

void LCD_NOKIA_C100_init(void);
void LCD_NOKIA_C100_send_command(uint8_t command);
void LCD_NOKIA_C100_send_data(uint8_t data);

void LCD_NOKIA_C100_reset_xy(void);
void LCD_NOKIA_C100_clear_screen(uint16_t color);

void LCD_NOKIA_C100_draw_ebu_test_pattern(void);


#endif
