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

void LCD_NOKIA_C100_set_pins(void)
{
	//SET THE /RESET PIN GPIO FOR THE NOKIA
	//LCD

	//SET GPIO4 AS OUTPUT FOR RST LINE
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
	LCD_NOKIA_C100_RST_HIGH;
}

void LCD_NOKIA_C100_init(void)
{
	LCD_NOKIA_C100_RST_LOW;
	//DELAY FOR 200us
	os_delay_us(200);
	LCD_NOKIA_C100_RST_HIGH;
	//DELAY FOR 10MS
	os_delay_us(10000);

	//SOFTWARE RESET
	LCD_NOKIA_C100_send_command(LCD_NOKIA_C100_COMMAND_SWRESET);
	os_delay_us(5000);

	//SLEEP OUT + BOOSTER ON
	LCD_NOKIA_C100_send_command(LCD_NOKIA_C100_COMMAND_SLPOUT);
	os_delay_us(6000);

	//PARTIAL DISPLAY OFF
	LCD_NOKIA_C100_send_command(LCD_NOKIA_C100_COMMAND_NORON);

	os_delay_us(6000);

	//INTERFACE PIXEL FORMAT = RGB (5 6 5)
	LCD_NOKIA_C100_send_command(LCD_NOKIA_C100_COMMAND_COLMOD);
	LCD_NOKIA_C100_send_data(0x05);

	//DISPLAY ON
	LCD_NOKIA_C100_send_command(LCD_NOKIA_C100_COMMAND_DISPON);

	//SET GAMMA PROFILE
	LCD_NOKIA_C100_send_command(LCD_NOKIA_C100_COMMAND_GAMSET);
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

void LCD_NOKIA_C100_set_xy_area(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end)
{
	//SET THE LCD WRITING AREA (FOR RAMWR COMMAND) TO THE
	//SPECIFIED X,Y BOUNDS

	//CASET (COLS = 0 TO 131)
	LCD_NOKIA_C100_send_command(0x2A);
	LCD_NOKIA_C100_send_data(0x00);
	LCD_NOKIA_C100_send_data(x_start);
	LCD_NOKIA_C100_send_data(0x00);
	LCD_NOKIA_C100_send_data(x_end);

	//RASET (ROWS = 0 TO 161)
	LCD_NOKIA_C100_send_command(0x2B);
	LCD_NOKIA_C100_send_data(0x00);
	LCD_NOKIA_C100_send_data(y_start);
	LCD_NOKIA_C100_send_data(0x00);
	LCD_NOKIA_C100_send_data(y_end);
}

void LCD_NOKIA_C100_clear_screen(uint16_t color)
{
	uint8_t h = (color>>8);
	uint8_t l = (color);

	//SET DRAWING AREA TO THE WHOLE SCREEN
	LCD_NOKIA_C100_set_xy_area(0, 0, 131, 161);

	//RAMWR
	LCD_NOKIA_C100_send_command(LCD_NOKIA_C100_COMMAND_RAMWR);

	uint16_t i;
	for(i=0; i<(132*162); i++)
	{
		LCD_NOKIA_C100_send_data(h);
		LCD_NOKIA_C100_send_data(l);
	}
}

void LCD_NOKIA_C100_draw_bitmap(uint8_t x_start, uint8_t x_end, uint8_t y_start, uint8_t y_end, uint32_t bitmap_flash_address, uint32_t bitmap_size_bytes)
{
	//DRAW THE BITMAP IMAGE FROM THE SPECIFIED ARRAY (AND GIVEN SIZE)
	//BETWEEN THE SPECIFIED X AND Y CONSTRAINTS

	uint32_t buffer[256];
	uint8_t p1, p2, p3, p4;
	uint32_t num_1kb_blocks;
	uint32_t num_remaining_bytes;

	num_1kb_blocks = bitmap_size_bytes / 1024;
	if(num_1kb_blocks == 0)
	{
		//SIZE LESS THAN 1024 BYTES
		num_remaining_bytes = bitmap_size_bytes;
	}
	else
	{
		num_remaining_bytes = bitmap_size_bytes % 1024;
	}

	LCD_NOKIA_C100_clear_screen(LCD_NOKIA_C100_COLOR_BLACK);

	//SET DRAWING AREA TO THE WHOLE SCREEN
	LCD_NOKIA_C100_set_xy_area(0, 0, 131, 161);

	//RAMWR
	LCD_NOKIA_C100_send_command(LCD_NOKIA_C100_COMMAND_RAMWR);

	uint32_t i=0;
	uint32_t read_address = bitmap_flash_address;
	while(num_1kb_blocks != 0)
	{
		ESP8266_FLASH_read(read_address, buffer, 1024);
		for(i=0; i<256; i++)
		{
			p1 = (uint8_t)(buffer[i] >> 24);
			p2 = (uint8_t)((buffer[i] & 0x00FF0000) >> 16);
			p3 = (uint8_t)((buffer[i] & 0x0000FF00) >> 24);
			p4 = (uint8_t)buffer[i];

			LCD_NOKIA_C100_send_data(p1);
			LCD_NOKIA_C100_send_data(p2);
			LCD_NOKIA_C100_send_data(p3);
			LCD_NOKIA_C100_send_data(p4);
		}
		num_1kb_blocks--;
		read_address += 1024;
	}
	//READ THE REMAINING BYTES (LESS THAN 1024)
	ESP8266_FLASH_read(read_address, buffer, 1024);
	for(i=0; i<(num_remaining_bytes/4); i++)
	{
		p1 = (uint8_t)(buffer[i] >> 24);
		p2 = (uint8_t)((buffer[i] & 0x00FF0000) >> 16);
		p3 = (uint8_t)((buffer[i] & 0x0000FF00) >> 24);
		p4 = (uint8_t)buffer[i];

		LCD_NOKIA_C100_send_data(p1);
		LCD_NOKIA_C100_send_data(p2);
		LCD_NOKIA_C100_send_data(p3);
		LCD_NOKIA_C100_send_data(p4);
	}
}

void LCD_NOKIA_C100_draw_text(uint8_t x_start, uint8_t y_start, const uint8_t* font_map, const uint16_t (*font_descriptor_map)[3], uint8_t font_width, uint8_t font_height, char* str, uint8_t len, uint16_t color, uint16_t bgcolor)
{
	//DRAW THE SPECIFIED STRING IN THE SPECIFIED FONT AT THE
	//SPECIFIED TOP LEFT STARTING LOCATION
	//FONT_MAP    : POINTER TO FONT MAPPING ARRAY (MAKE SURE TO ADD ALL SYMBOLS IN THE DOCT FACTORY
	//FONT_DESCRIPTOR_MAP : CONTAINS THE WIDTH, HEIGHT AND OFFSET OF CHARACTERS IN THE MAIN FONT MAP ARRAY
	//FONT_WIDTH  : FONT CHARACTER WIDTH IN BYTES
	//FONT_HEIGHT : FONT CHARACTER HEIGHT IN BYTES
	//LEN         : THE LENGTH OF THE STRING TO BE DRAWN
	//COLOR		  : THE COLOR TO DRAW THE STRING IN

	uint8_t h_color = (color>>8);
	uint8_t l_color = (color);
	uint8_t h_bgcolor = (bgcolor >> 8);
	uint8_t l_bgcolor = (bgcolor);

	uint8_t i, current_byte;
	uint16_t j;
	uint32_t char_start_location;
	uint16_t char_byte_len = font_width * font_height;

	for(i=0; i<len; i++)
	{
		//SET THE CHARACTER BOUNDS
		LCD_NOKIA_C100_set_xy_area(x_start, y_start, x_start + (font_width * 8)-1, y_start + font_descriptor_map[str[i]-32][1]);
		//GET CHARACTER PARAMETERS
		char_start_location = font_descriptor_map[str[i]-32][2];
		char_byte_len = font_width * font_height;
		//RAMWR
		LCD_NOKIA_C100_send_command(LCD_NOKIA_C100_COMMAND_RAMWR);

		for(j=0; j<char_byte_len; j++)
		{
			current_byte = font_map[char_start_location + j];

			//BIT 7
			if(current_byte & 0b10000000)
			{
				LCD_NOKIA_C100_send_data(h_color);
				LCD_NOKIA_C100_send_data(l_color);
			}
			else
			{
				LCD_NOKIA_C100_send_data(h_bgcolor);
				LCD_NOKIA_C100_send_data(l_bgcolor);
			}

			//BIT 6
			if(current_byte & 0b01000000)
			{
				LCD_NOKIA_C100_send_data(h_color);
				LCD_NOKIA_C100_send_data(l_color);
			}
			else
			{
				LCD_NOKIA_C100_send_data(h_bgcolor);
				LCD_NOKIA_C100_send_data(l_bgcolor);
			}

			//BIT 5
			if(current_byte & 0b00100000)
			{
				LCD_NOKIA_C100_send_data(h_color);
				LCD_NOKIA_C100_send_data(l_color);
			}
			else
			{
				LCD_NOKIA_C100_send_data(h_bgcolor);
				LCD_NOKIA_C100_send_data(l_bgcolor);
			}

			//BIT 4
			if(current_byte & 0b00010000)
			{
				LCD_NOKIA_C100_send_data(h_color);
				LCD_NOKIA_C100_send_data(l_color);
			}
			else
			{
				LCD_NOKIA_C100_send_data(h_bgcolor);
				LCD_NOKIA_C100_send_data(l_bgcolor);
			}

			//BIT 3
			if(current_byte & 0b00001000)
			{
				LCD_NOKIA_C100_send_data(h_color);
				LCD_NOKIA_C100_send_data(l_color);
			}
			else
			{
				LCD_NOKIA_C100_send_data(h_bgcolor);
				LCD_NOKIA_C100_send_data(l_bgcolor);
			}

			//BIT 2
			if(current_byte & 0b00000100)
			{
				LCD_NOKIA_C100_send_data(h_color);
				LCD_NOKIA_C100_send_data(l_color);
			}
			else
			{
				LCD_NOKIA_C100_send_data(h_bgcolor);
				LCD_NOKIA_C100_send_data(l_bgcolor);
			}

			//BIT 1
			if(current_byte & 0b00000010)
			{
				LCD_NOKIA_C100_send_data(h_color);
				LCD_NOKIA_C100_send_data(l_color);
			}
			else
			{
				LCD_NOKIA_C100_send_data(h_bgcolor);
				LCD_NOKIA_C100_send_data(l_bgcolor);
			}
			//BIT 0
			if(current_byte & 0b00000001)
			{
				LCD_NOKIA_C100_send_data(h_color);
				LCD_NOKIA_C100_send_data(l_color);
			}
			else
			{
				LCD_NOKIA_C100_send_data(h_bgcolor);
				LCD_NOKIA_C100_send_data(l_bgcolor);
			}

		}
		x_start += font_descriptor_map[str[i]-32][0];
	}
}

void LCD_NOKIA_C100_draw_filled_box(uint8_t x_start, uint8_t x_end, uint8_t y_start, uint8_t y_end, uint16_t color)
{
	//DRAW A BOX BETWEEN THE GIVEN X AND Y BOUNDS AND
	//FILL IT WITH THE SUPPLIED COLOR

	uint8_t h_color = (color>>8);
	uint8_t l_color = (color);
	uint16_t pixel_count = (x_end - x_start + 1) * (y_end - y_start + 1);
	uint16_t i;

	LCD_NOKIA_C100_set_xy_area(x_start, y_start, x_end, y_end);

	//RAMWR
	LCD_NOKIA_C100_send_command(LCD_NOKIA_C100_COMMAND_RAMWR);

	for(i=0; i<pixel_count; i++)
	{
		LCD_NOKIA_C100_send_data(h_color);
		LCD_NOKIA_C100_send_data(l_color);
	}
}

void LCD_NOKIA_C100_draw_outline_box(uint8_t x_start, uint8_t x_end, uint8_t y_start, uint8_t y_end, uint8_t border_size, uint16_t color)
{
	//DRAW A BOX OUTLINE BETWEEN THE GIVEN X AND Y BOUNDS AND
	//AND OUTLINE WITH THE SPECIFIED COLOR

	uint8_t h_color = (color>>8);
	uint8_t l_color = (color);
	uint16_t pixel_count = (x_end - x_start + 1) * (y_end - y_start + 1);
	uint16_t i;

	LCD_NOKIA_C100_set_xy_area(x_start, y_start, x_end, y_end);

	//RAMWR
	LCD_NOKIA_C100_send_command(LCD_NOKIA_C100_COMMAND_RAMWR);

	for(i=0; i<pixel_count; i++)
	{
		LCD_NOKIA_C100_send_data(h_color);
		LCD_NOKIA_C100_send_data(l_color);
	}

	pixel_count = ((x_end - border_size) - (x_start + border_size) + 1) * ((y_end - border_size) - (y_start + border_size) + 1);

	LCD_NOKIA_C100_set_xy_area(x_start + border_size, y_start + border_size, x_end - border_size, y_end - border_size);

	//RAMWR
	LCD_NOKIA_C100_send_command(LCD_NOKIA_C100_COMMAND_RAMWR);

	for(i=0; i<pixel_count; i++)
	{
		LCD_NOKIA_C100_send_data(0x00);
		LCD_NOKIA_C100_send_data(0x00);
	}
}
