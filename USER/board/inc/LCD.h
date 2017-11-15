
#ifndef __LCD_H__
#define __LCD_H__

#include "stdint.h"



#define RED  	0xf800
#define GREEN	0x07e0
#define BLUE 	0x001f
#define WHITE	0xffff
#define BLACK	0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D   	//»ÒÉ«0 3165 00110 001011 00101
#define GRAY1   0x8410      	//»ÒÉ«1      00000 000000 00000
#define GRAY2   0x4208      	//»ÒÉ«2  1111111111011111

#define X_MAX_PIXEL	        128
#define Y_MAX_PIXEL	        128

void Delay_ms(int time);
void SPI_WriteData(uint8_t Data);
void LCD_Init(void);
void LCD_SetRegion(uint16_t x_start,uint16_t y_start,\
										uint16_t x_end,uint16_t y_end);
void LCD_SetXY(uint16_t x,uint16_t y);
void GUI_DrawPoint(uint16_t x,uint16_t y,uint16_t Data);
unsigned int LCD_ReadPoint(uint16_t x,uint16_t y);
void LCD_Clear(uint16_t color);
void Show_Image(const unsigned char *p);
void Gui_DrawFont_GBK16(uint16_t x, uint16_t y,uint16_t bc, uint8_t *s);



#endif
