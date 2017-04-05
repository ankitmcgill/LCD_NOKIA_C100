/*************************************************
* NOKIA C100 TFT LIBRARY
* ARM VERSION
*
* 132 x 162 TFT LCD COLOR
* EACH PIXEL IS R(5) - G(6) - B(5)
*
* NOTE: BECAUSE THE NOKIA LCD MODULES WE GET FROM CHINA ARE
*				OF CHEAP QUALITY, LINES / PIXELS DRAWN ON THE LCD
*				EDGES (ROW = 0 OR 161, COL = 0 OR 131) MAY NOT SHOW
*				PROPERLY AS THE LCD BEZEL MIGHT BE COVERING THE EDGES
* 
* MARCH 27 2017
* ANKIT BHATNAGAR
* ANKIT.BHATNAGARINDIA@GMAIL.COM
* ***********************************************/

#include "LCD_NOKIA_C100.h"
#include "ARM_STM32_USART.h"
#include "ARM_STM32_SYSTICK.h"

//FUNCTION POINTER TO SPI SEND FUNCTION
static void (*LCD_NOKIA_C100_SPI_SEND_FUNCTION_POINTER)(uint16_t, uint8_t) = ARM_STM32_USART_Send_Char_Reversed_Usart1;

//FUNCTION POINTER FOR BLOCKING DELAY - US
static void (*LCD_NOKIA_C100_DELAY_US)(uint32_t) = ARM_STM32_SYSTICK_Delay_Us;

void LCD_NOKIA_C100_Set_Pins(void)
{
	//SET THE /RESET PIN GPIO FOR THE NOKIA
	//LCD

	GPIO_InitTypeDef gpio;
	gpio.GPIO_Pin = LCD_NOKIA_C100_RST_PIN_NUM;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Speed = GPIO_Speed_10MHz;
	
	GPIO_Init(LCD_NOKIA_C100_RST_PIN_PORT, &gpio);
	
	//SET RESET LINE TO HIGH
	LCD_NOKIA_C100_RST_HIGH;
}

void LCD_NOKIA_C100_Init(void)
{
	LCD_NOKIA_C100_RST_LOW;
	//DELAY FOR 200us
	(*LCD_NOKIA_C100_DELAY_US)(200);
	LCD_NOKIA_C100_RST_HIGH;
	//DELAY FOR 10MS
	(*LCD_NOKIA_C100_DELAY_US)(10000);

	//SOFTWARE RESET
	LCD_NOKIA_C100_Send_Command(LCD_NOKIA_C100_COMMAND_SWRESET);
	(*LCD_NOKIA_C100_DELAY_US)(5000);
	
	//ROTATE DISPLAY IF LANDSCAPE MODE SELECTED
	#if defined(LCD_NOKIA_C100_MODE_LANDSCAPE)
		LCD_NOKIA_C100_Send_Command(LCD_NOKIA_C100_COMMAND_MADCTR);
		LCD_NOKIA_C100_Send_Data(0xA0);
		(*LCD_NOKIA_C100_DELAY_US)(5000);
	#endif
	
	//SLEEP OUT + BOOSTER ON
	LCD_NOKIA_C100_Send_Command(LCD_NOKIA_C100_COMMAND_SLPOUT);
	(*LCD_NOKIA_C100_DELAY_US)(6000);

	//PARTIAL DISPLAY OFF
	LCD_NOKIA_C100_Send_Command(LCD_NOKIA_C100_COMMAND_NORON);
	(*LCD_NOKIA_C100_DELAY_US)(6000);

	//INTERFACE PIXEL FORMAT = RGB (5 6 5)
	LCD_NOKIA_C100_Send_Command(LCD_NOKIA_C100_COMMAND_COLMOD);
	LCD_NOKIA_C100_Send_Data(0x05);

	//DISPLAY ON
	LCD_NOKIA_C100_Send_Command(LCD_NOKIA_C100_COMMAND_DISPON);

	//SET GAMMA PROFILE
	LCD_NOKIA_C100_Send_Command(LCD_NOKIA_C100_COMMAND_GAMSET);
	LCD_NOKIA_C100_Send_Data(0x08);
}

void LCD_NOKIA_C100_Send_Command(uint8_t command)
{
	(*LCD_NOKIA_C100_SPI_SEND_FUNCTION_POINTER)(command, 9);
}

void LCD_NOKIA_C100_Send_Data(uint8_t data)
{
	(*LCD_NOKIA_C100_SPI_SEND_FUNCTION_POINTER)((0x0100 | data), 9);
}

void LCD_NOKIA_C100_Set_Xy_Area(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end)
{
	//SET THE LCD WRITING AREA (FOR RAMWR COMMAND) TO THE SPECIFIED X,Y BOUNDS

	//CASET
	LCD_NOKIA_C100_Send_Command(LCD_NOKIA_C100_COMMAND_CASET);
	LCD_NOKIA_C100_Send_Data(0x00);
	LCD_NOKIA_C100_Send_Data(x_start);
	LCD_NOKIA_C100_Send_Data(0x00);
	LCD_NOKIA_C100_Send_Data(x_end);

	//RASET
	LCD_NOKIA_C100_Send_Command(LCD_NOKIA_C100_COMMAND_RASET);
	LCD_NOKIA_C100_Send_Data(0x00);
	LCD_NOKIA_C100_Send_Data(y_start);
	LCD_NOKIA_C100_Send_Data(0x00);
	LCD_NOKIA_C100_Send_Data(y_end);
}

void LCD_NOKIA_C100_Clear_Screen(uint16_t color)
{
	uint8_t h = (color>>8);
	uint8_t l = (color);

	//SET DRAWING AREA TO THE WHOLE SCREEN
	LCD_NOKIA_C100_Set_Xy_Area(0, 0, LCD_NOKIA_C100_COL_MAX, LCD_NOKIA_C100_ROW_MAX);

	//RAMWR
	LCD_NOKIA_C100_Send_Command(LCD_NOKIA_C100_COMMAND_RAMWR);

	for(uint16_t i=0; i<((LCD_NOKIA_C100_ROW_MAX+1) * (LCD_NOKIA_C100_COL_MAX+1)); i++)
	{
		LCD_NOKIA_C100_Send_Data(h);
		LCD_NOKIA_C100_Send_Data(l);
	}
	
	//RESET CURSOR BACK TO 0,0
	//SET DRAWING AREA TO THE WHOLE SCREEN
	LCD_NOKIA_C100_Set_Xy_Area(0, 0, LCD_NOKIA_C100_COL_MAX, LCD_NOKIA_C100_ROW_MAX);
}

void LCD_NOKIA_C100_Draw_Pixel(uint8_t x, uint8_t y, uint16_t color)
{
	//SET THE SPECIFIED PIXEL WITH THE SPECIFIED COLOR
	
	//SET DRAWING AREA
	LCD_NOKIA_C100_Set_Xy_Area(x, y, x + 1, y + 1);
	
	//RAMWR
	LCD_NOKIA_C100_Send_Command(LCD_NOKIA_C100_COMMAND_RAMWR);
	
	uint8_t h = (color>>8);
	uint8_t l = (color);
	
	LCD_NOKIA_C100_Send_Data(h);
	LCD_NOKIA_C100_Send_Data(l);
}

void LCD_NOKIA_C100_Draw_Line_Horizontal(uint8_t x_start, uint8_t x_end, uint8_t y, uint8_t thickness, uint16_t color)
{
	//DRAW A HORIZONTAL LINE STARTING AT X_START, Y WITH THE THICKNESS SPECIFIED IN THE BOTTOM (X) DIRECTION

	//SET DRAWING AREA
	LCD_NOKIA_C100_Set_Xy_Area(x_start, y, x_end, y + thickness - 1);
	
	//RAMWR
	LCD_NOKIA_C100_Send_Command(LCD_NOKIA_C100_COMMAND_RAMWR);
	
	uint8_t h = (color>>8);
	uint8_t l = (color);
	
	for(uint16_t i=0; i<(x_end - x_start + 1)*(thickness); i++)
	{
		LCD_NOKIA_C100_Send_Data(h);
		LCD_NOKIA_C100_Send_Data(l);
	}
}

void LCD_NOKIA_C100_Draw_Line_Vertical(uint8_t y_start, uint8_t y_end, uint8_t x, uint8_t thickness, uint16_t color)
{
	//DRAW A VERTICAL LINE STARTING AT X, Y_START WITH THE THICKNESS SPECIFIED IN THE RIGHT (Y) DIRECTION
	
	//SET DRAWING AREA
	LCD_NOKIA_C100_Set_Xy_Area(x, y_start, x + thickness - 1, y_end);
	
	//RAMWR
	LCD_NOKIA_C100_Send_Command(LCD_NOKIA_C100_COMMAND_RAMWR);
	
	uint8_t h = (color>>8);
	uint8_t l = (color);
	
	for(uint16_t i=0; i<(y_end - y_start + 1)*(thickness); i++)
	{
		LCD_NOKIA_C100_Send_Data(h);
		LCD_NOKIA_C100_Send_Data(l);
	}
}

void LCD_NOKIA_C100_Draw_Line(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint16_t color)
{
	//DRAW GENERIC LINE BETWEEN ANY 2 POINTS USING BRESENHAM ALGORITHM
	
	int16_t dx = x_end - x_start;
	int16_t dy = y_end - y_start;
	
	if(dx < 0)
	{
		dx = dx * -1;
	}
	if(dy < 0)
	{
		dy = dy * -1;
	}
	
	uint8_t x = x_start;
	uint8_t y = y_start;
	
	int16_t e = 0;
	
	if(dy < dx)
	{
		//ITERATE OVER X
		
		for(x=x_start; x<=x_end; x++)
		{
			LCD_NOKIA_C100_Draw_Pixel(x, y, color);
			e += dy;
			if((e << 1) >= dx)
			{
				y++;
				e -= dx;
			}
		}
	}
	else
	{
		//ITERATE OVER Y
		
		for(y=y_start; y<=y_end; y++)
		{
			LCD_NOKIA_C100_Draw_Pixel(x, y, color);
			e += dx;
			if((e << 1) >= dy)
			{
				x++;
				e -= dy;
			}
		}
	}
}

void LCD_NOKIA_C100_Draw_Rectangle_Outline(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint8_t border_thickness, uint16_t color)
{
	//DRAW A RECTANGLE OUTLINE BETWEEN THE GIVEN X AND Y BOUNDS AND SPECIFIED THICKNESS
	//WITH THE SPECIFIED COLOR. THE THICKNESS OF THE RECTANGLE IS INWARDS
	
	LCD_NOKIA_C100_Draw_Line_Vertical(y_start, y_end, x_start, border_thickness, color);
	LCD_NOKIA_C100_Draw_Line_Vertical(y_start, y_end, (x_end - border_thickness + 1), border_thickness, color);
	LCD_NOKIA_C100_Draw_Line_Horizontal(x_start, x_end, y_start, border_thickness, color);
	LCD_NOKIA_C100_Draw_Line_Horizontal(x_start, x_end, (y_end - border_thickness + 1), border_thickness, color);
}

void LCD_NOKIA_C100_Draw_Rectangle_Filled(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end, uint8_t border_thickness, uint16_t color, uint16_t fill_color)
{
	//DRAW A RECTANGLE OUTLINE BETWEEN THE GIVEN X AND Y BOUNDS AND SPECIFIED THICKNESS
	//WITH THE SPECIFIED COLOR. THE THICKNESS OF THE RECTANGLE IS INWARDS. FILL THE RECTANGLE WITH SPECIFIED COLOR
	
	LCD_NOKIA_C100_Draw_Rectangle_Outline(x_start, y_start, x_end, y_end, border_thickness, color);
	
	LCD_NOKIA_C100_Set_Xy_Area(x_start + border_thickness, y_start + border_thickness, x_end - border_thickness, y_end - border_thickness);
	
	//RAMWR
	LCD_NOKIA_C100_Send_Command(LCD_NOKIA_C100_COMMAND_RAMWR);
	
	uint8_t h = (fill_color>>8);
	uint8_t l = (fill_color);
	
	for(uint16_t i=0; i<(y_end - y_start - (2 * border_thickness) + 1)*(x_end - x_start - (2 * border_thickness) + 1); i++)
	{
		LCD_NOKIA_C100_Send_Data(h);
		LCD_NOKIA_C100_Send_Data(l);
	}
}

void LCD_NOKIA_C100_Draw_Circle_Outline(uint8_t x_centre, uint8_t y_centre, uint8_t radius, uint8_t thickness, uint16_t color)
{
	//DRAW CIRCLE OF SPECIFIED THICKNESS AT SPECIFIED CENTRE WITH SPECIFIED RADIUS
	
	int8_t x, y;

	for(y=(-radius - thickness); y<=(radius + thickness); y++)
	{
		for(x=(-radius - thickness); x<=(radius + thickness); x++)
		{
			if((x*x)+(y*y) < (radius*radius))
			{
				//DO NOTHING
			}
			else if((x*x)+(y*y) < ((radius + thickness)*(radius + thickness)))
			{
				LCD_NOKIA_C100_Draw_Pixel(x_centre + x, y_centre + y, color);
			}
		}
	}
}

void LCD_NOKIA_C100_Draw_Circle_Filled(uint8_t x_centre, uint8_t y_centre, uint8_t radius, uint8_t thickness, uint16_t color, uint16_t fill_color)
{
	//DRAW CIRCLE OF SPECIFIED THICKNESS AT SPECIFIED CENTRE WITH SPECIFIED RADIUS AND FILL WITH SPECIFIED COLOR
	
	int8_t x, y;

	for(y=(-radius - thickness); y<=(radius + thickness); y++)
	{
		for(x=(-radius - thickness); x<=(radius + thickness); x++)
		{
			if((x*x)+(y*y) < (radius*radius))
			{
				LCD_NOKIA_C100_Draw_Pixel(x_centre + x, y_centre + y, fill_color);
			}
			else if((x*x)+(y*y) < ((radius + thickness)*(radius + thickness)))
			{
				LCD_NOKIA_C100_Draw_Pixel(x_centre + x, y_centre + y, color);
			}
		}
	}
}

void LCD_NOKIA_C100_Draw_Triangle_Outline(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3, uint16_t color)
{
	//DRAW UNFILLED TRIANGLE BETWEEN THE 3 SPECIFIED (X,Y) POINTS
	
	LCD_NOKIA_C100_Draw_Line(x1, y1, x2, y2, color);
	LCD_NOKIA_C100_Draw_Line(x2, y2, x3, y3, color);
	LCD_NOKIA_C100_Draw_Line(x3, y3, x1, y1, color);
}

void LCD_NOKIA_C100_Draw_Bitmap(uint8_t x_start, uint8_t y_start, uint8_t width, uint8_t height, uint32_t data_len, uint8_t* bitmap_buffer)
{
	//DRAW THE BITMAP BUFFER IMAGE STARTING FROM THE SPECIFIED TOP LEFT (X,Y) CORDINATE AND WITH SPECIFIED WIDTH AND HEIGHT
	
	//SET X-Y DRAWING AREA
	LCD_NOKIA_C100_Set_Xy_Area(x_start, y_start, x_start + width - 1, y_start + height - 1);
	
	//RAMWR
	LCD_NOKIA_C100_Send_Command(LCD_NOKIA_C100_COMMAND_RAMWR);
	
	for(uint32_t i=0; i<data_len; i++)
	{
		LCD_NOKIA_C100_Send_Data(bitmap_buffer[i]);
	}
}

void LCD_NOKIA_C100_Draw_Character(uint8_t x_start, uint8_t y_start, FONT_INFO f_info, uint8_t c, uint16_t color, uint16_t bgcolor)
{
	//DRAW THE SPECIFIED CHARACTER IN THE SPECIFIED FONT AT THE SPECIFIED TOP LEFT STARTING LOCATION
	
	uint8_t h_color = (color>>8);
	uint8_t l_color = (color);
	uint8_t h_bgcolor = (bgcolor >> 8);
	uint8_t l_bgcolor = (bgcolor);
	
	//SET THE CHARACTER BOUNDS
	LCD_NOKIA_C100_Set_Xy_Area(x_start, y_start, x_start + ((f_info.font_char_descriptors[c-32][0]) * 8)-1, y_start + f_info.font_char_descriptors[c-32][1]);
	
	//GET CHARACTER PARAMETERS
	uint32_t char_start_location = f_info.font_char_descriptors[c-32][2];
	uint16_t char_byte_len = f_info.font_char_descriptors[c-32][0] * f_info.font_char_descriptors[c-32][1];
	
	//RAMWR
	LCD_NOKIA_C100_Send_Command(LCD_NOKIA_C100_COMMAND_RAMWR);
	
	for(uint16_t j=0; j<char_byte_len; j++)
	{
		uint8_t current_byte = f_info.font_bitmap[char_start_location + j];

		//BIT 7
		if(current_byte & 0x80)
		{
			LCD_NOKIA_C100_Send_Data(h_color);
			LCD_NOKIA_C100_Send_Data(l_color);
		}
		else
		{
			LCD_NOKIA_C100_Send_Data(h_bgcolor);
			LCD_NOKIA_C100_Send_Data(l_bgcolor);
		}

		//BIT 6
		if(current_byte & 0x40)
		{
			LCD_NOKIA_C100_Send_Data(h_color);
			LCD_NOKIA_C100_Send_Data(l_color);
		}
		else
		{
			LCD_NOKIA_C100_Send_Data(h_bgcolor);
			LCD_NOKIA_C100_Send_Data(l_bgcolor);
		}

		//BIT 5
		if(current_byte & 0x20)
		{
			LCD_NOKIA_C100_Send_Data(h_color);
			LCD_NOKIA_C100_Send_Data(l_color);
		}
		else
		{
			LCD_NOKIA_C100_Send_Data(h_bgcolor);
			LCD_NOKIA_C100_Send_Data(l_bgcolor);
		}

		//BIT 4
		if(current_byte & 0x10)
		{
			LCD_NOKIA_C100_Send_Data(h_color);
			LCD_NOKIA_C100_Send_Data(l_color);
		}
		else
		{
			LCD_NOKIA_C100_Send_Data(h_bgcolor);
			LCD_NOKIA_C100_Send_Data(l_bgcolor);
		}

		//BIT 3
		if(current_byte & 0x08)
		{
			LCD_NOKIA_C100_Send_Data(h_color);
			LCD_NOKIA_C100_Send_Data(l_color);
		}
		else
		{
			LCD_NOKIA_C100_Send_Data(h_bgcolor);
			LCD_NOKIA_C100_Send_Data(l_bgcolor);
		}

		//BIT 2
		if(current_byte & 0x04)
		{
			LCD_NOKIA_C100_Send_Data(h_color);
			LCD_NOKIA_C100_Send_Data(l_color);
		}
		else
		{
			LCD_NOKIA_C100_Send_Data(h_bgcolor);
			LCD_NOKIA_C100_Send_Data(l_bgcolor);
		}

		//BIT 1
		if(current_byte & 0x02)
		{
			LCD_NOKIA_C100_Send_Data(h_color);
			LCD_NOKIA_C100_Send_Data(l_color);
		}
		else
		{
			LCD_NOKIA_C100_Send_Data(h_bgcolor);
			LCD_NOKIA_C100_Send_Data(l_bgcolor);
		}
		//BIT 0
		if(current_byte & 0x01)
		{
			LCD_NOKIA_C100_Send_Data(h_color);
			LCD_NOKIA_C100_Send_Data(l_color);
		}
		else
		{
			LCD_NOKIA_C100_Send_Data(h_bgcolor);
			LCD_NOKIA_C100_Send_Data(l_bgcolor);
		}
	}
}

void LCD_NOKIA_C100_Draw_Text_Fixedwidth(uint8_t x_start, uint8_t y_start, FONT_INFO f_info, char* str, uint8_t len, uint16_t color, uint16_t bgcolor)
{
	//DRAW THE SPECIFIED STRING IN THE SPECIFIED FONT AT THE SPECIFIED TOP LEFT STARTING LOCATION
	//FIXED WIDTH FONT
	//FINFO    		: FONT DESCRIPTIVE STRUCT CONTAINING ITS BITMAP ARRAY, DESCRIPTOR ARRAY ETC (DEFINED IN FONT.H)
	//LEN         : THE LENGTH OF THE STRING TO BE DRAWN
	//COLOR		  	: THE COLOR TO DRAW THE STRING IN
	//BGCOLOR			: BACKGROUND COLOR OF THE TEXT

	for(uint8_t i=0; i<len; i++)
	{
		LCD_NOKIA_C100_Draw_Character(x_start, y_start, f_info, str[i], color, bgcolor);
		x_start += (f_info.font_char_descriptors[str[i]-32][0] * 8);
	}
}

void LCD_NOKIA_C100_Draw_Text_VariableWidth(uint8_t x_start, uint8_t y_start, FONT_INFO f_info, char* str, uint8_t len, uint16_t color, uint16_t bgcolor)
{
	//DRAW THE SPECIFIED STRING IN THE SPECIFIED FONT AT THE SPECIFIED TOP LEFT STARTING LOCATION
	//VARIABLE WIDTH FONT
	//FINFO    		: FONT DESCRIPTIVE STRUCT CONTAINING ITS BITMAP ARRAY, DESCRIPTOR ARRAY ETC (DEFINED IN FONT.H)
	//LEN         : THE LENGTH OF THE STRING TO BE DRAWN
	//COLOR		  	: THE COLOR TO DRAW THE STRING IN
	//BGCOLOR			: BACKGROUND COLOR OF THE TEXT
	
}

void LCD_NOKIA_C100_Draw_EBU_Test_Pattern(void)
{
	/*
	EBU TEST PATTERN
	R	 G  B
	-  -  -
	1, 1, 1: WHITE
	1, 1, 0: YELLOW
	0, 1, 1: CYAN
	0, 1, 0: GREEN
	1, 0, 1: MAGENTA
	1, 0, 0: RED
	0, 0, 1: BLUE
	0, 0, 0: BLACK

	8 HORIZONTAL BARS
	FOR EACH BAR
	COLUMNS = LCD_NOKIA_C100_COL_MAX
	ROWS = LCD_NOKIA_C100_ROW_MAX/8
	*/
  
	uint8_t temp = 0;
  
	LCD_NOKIA_C100_Clear_Screen(LCD_NOKIA_C100_COLOR_BLACK);
	
	//RAMWR
	LCD_NOKIA_C100_Send_Command(LCD_NOKIA_C100_COMMAND_RAMWR);
  
	for(uint16_t i=0; i<((LCD_NOKIA_C100_ROW_MAX + 1)*(LCD_NOKIA_C100_COL_MAX + 1)); i++)
	{
		temp = i/((LCD_NOKIA_C100_COL_MAX + 1)*(LCD_NOKIA_C100_ROW_MAX / 8));

		switch(temp)
		{
			case 0:	//WHITE
					LCD_NOKIA_C100_Send_Data(0xFF);
					LCD_NOKIA_C100_Send_Data(0xFF);
					break;
			
			case 1:	//YELLOW
					LCD_NOKIA_C100_Send_Data(0xFF);
					LCD_NOKIA_C100_Send_Data(0xE0);
					break;
			
			case 2:	//CYAN
					LCD_NOKIA_C100_Send_Data(0x07);
					LCD_NOKIA_C100_Send_Data(0xFF);
					break;

			case 3:	//GREEN
					LCD_NOKIA_C100_Send_Data(0x07);
					LCD_NOKIA_C100_Send_Data(0xE0);
					break;

			case 4:	//MAGENTA
					LCD_NOKIA_C100_Send_Data(0xF8);
					LCD_NOKIA_C100_Send_Data(0x1F);
					break;
			
			case 5:	//RED
					LCD_NOKIA_C100_Send_Data(0xF8);
					LCD_NOKIA_C100_Send_Data(0x00);
					break;

			case 6:	//BLUE
					LCD_NOKIA_C100_Send_Data(0x00);
					LCD_NOKIA_C100_Send_Data(0x1F);
					break;

			case 7:	//BLACK
					LCD_NOKIA_C100_Send_Data(0x00);
					LCD_NOKIA_C100_Send_Data(0x00);
					break;
			
			case 8:	//BLACK
					LCD_NOKIA_C100_Send_Data(0x00);
					LCD_NOKIA_C100_Send_Data(0x00);
					break;
		}
	}
}
