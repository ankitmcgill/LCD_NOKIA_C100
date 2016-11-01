/*************************************************
* NOKIA C100 TFT LIBRARY
*
* OCTOBER 30 2016
* ANKIT BHATNAGAR
* ANKIT.BHATNAGARINDIA@GMAIL.COM
* ***********************************************/

#include "LCD_NOKIA_C100.h"

//FUNCTION POINTER TO SPI SEND FUNCTION
void (*LCD_NOKIA_C100_SPI_SEND_FUNCTION_POINTER)(uint8_t, uint8_t, uint32_t, uint32_t) = &ESP8266_SPI_send;

void LCD_NOKIA_C100_init(void)
{
	//SET GPIO4 AS OUTPUT FOR RST LINE
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
	GPIO_OUTPUT_SET(4, 1);

	LCD_NOKIA_C100_RST_LOW;
	//DELAY FOR 200us
	os_delay_us(200);
	LCD_NOKIA_C100_RST_HIGH;
	//DELAY FOR 10MS
	os_delay_us(10000);

	//SOFTWARE RESET
	LCD_NOKIA_C100_send_command(0x01);
	os_delay_us(5000);

	//SLEEP OUT + BOOSTER ON
	LCD_NOKIA_C100_send_command(0x11);
	os_delay_us(6000);

	//PARTIAL DISPLAY OFF
	LCD_NOKIA_C100_send_command(0x13);

	os_delay_us(6000);

	//INTERFACE PIXEL FORMAT = RGB (5 6 5)
	LCD_NOKIA_C100_send_command(0x3A);
	LCD_NOKIA_C100_send_data(0x05);

	//DISPLAY ON
	LCD_NOKIA_C100_send_command(0x29);

	//SET GAMMA PROFILE
	LCD_NOKIA_C100_send_command(0x26);
	LCD_NOKIA_C100_send_data(0x08);
}

void LCD_NOKIA_C100_send_command(uint8_t command)
{
	(*LCD_NOKIA_C100_SPI_SEND_FUNCTION_POINTER)(1, 8, 0, command);
}

void LCD_NOKIA_C100_send_data(uint8_t data)
{
	(*LCD_NOKIA_C100_SPI_SEND_FUNCTION_POINTER)(1, 8, 1, data);
}

void LCD_NOKIA_C100_reset_xy(void)
{

}

void LCD_NOKIA_C100_clear_screen(uint16_t color)
{
	uint8_t h = (color>>8);
	uint8_t l = (color);

	//CASET (COLS = 0 TO 131)
	LCD_NOKIA_C100_send_command(0x2A);
	LCD_NOKIA_C100_send_data(0x00);
	LCD_NOKIA_C100_send_data(0x00);
	LCD_NOKIA_C100_send_data(0x00);
	LCD_NOKIA_C100_send_data(131);

	//RASET (ROWS = 0 TO 161)
	LCD_NOKIA_C100_send_command(0x2B);
	LCD_NOKIA_C100_send_data(0x00);
	LCD_NOKIA_C100_send_data(0x00);
	LCD_NOKIA_C100_send_data(0x00);
	LCD_NOKIA_C100_send_data(161);

	//RAMWR
	LCD_NOKIA_C100_send_command(0x2C);

	uint16_t i;
	for(i=0; i<(132*162); i++)
	{
		LCD_NOKIA_C100_send_data(h);
		LCD_NOKIA_C100_send_data(l);
	}
}
