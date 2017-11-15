/*  本文件主要要来封装对 LCD 显示屏的操作  */
/********************************************   
*板子的接口定义：
*					PA0 <==> tc4-1
*					PA1 <==>	RST_LOW
*					PA4 <==>	SS
*					PA5 <==>	SCK
*					PA6 <==>	MISO
*					PA7 <==>	MOSI
*********************************************/

#include "LCD.h"
#include "GPIO.h"
#include "font_lcd.h"
//#include "font.h"

#include "uart.h"

/*一个简单的延时函数*/
void Delay_ms(int time)
{
	int i,j;
	for(i=0;i<time*10;i++)
	{
		for(j=0;j<100;j++)
		{
		
		}
	}
}

void SPI_WriteData(uint8_t Data)
{
	unsigned char i;
	for(i = 8;i>0;i--){
		/* 模拟 SPI 的数据发送 */
		/*  PA_7 <--> MOSI */
		if(Data & 0x80){
			GPIOA->BSRR = GPIO_Pin_7;
		}else {
			GPIOA->BRR = GPIO_Pin_7;
		}
		/*使用IO 口来模拟时钟*/
		/* PA_5 <--> SCK */
		GPIOA->BSRR = GPIO_Pin_5;
		GPIOA->BRR = GPIO_Pin_5;
		Data <<=1;
	}
}


void LCD_WriteData_16Bit(uint16_t Data)
{
	GPIOA->BRR = GPIO_Pin_4;
	GPIOA->BSRR = GPIO_Pin_6;
	SPI_WriteData(Data >> 8);   //写入高八位数据
	SPI_WriteData(Data);	//写入低八位数据
	GPIOA->BSRR = GPIO_Pin_4;
}


//向液晶屏写入一个8位指令
void LCD_WriteCmd(uint8_t cmd)
{
	// SPI 写命令时序开始
	GPIOA->BRR = GPIO_Pin_4;
	GPIOA->BRR = GPIO_Pin_6;
	SPI_WriteData(cmd);
	GPIOA->BSRR = GPIO_Pin_4;
}


void LCD_WriteData(uint8_t Data)
{
	GPIOA->BRR = GPIO_Pin_4;
	GPIOA->BSRR = GPIO_Pin_6;
	SPI_WriteData(Data);
	GPIOA->BSRR = GPIO_Pin_4;
}


void LCD_WriteReg(uint8_t cmd,uint8_t Data)
{
	GPIOA->BRR = GPIO_Pin_4;
	LCD_WriteCmd(cmd);
	LCD_WriteData(Data);
	GPIOA->BSRR = GPIO_Pin_4;
}


// LCD 初始化
void LCD_Init(void)
{
	LCD_WriteCmd(0x11);  // 退出休眠
	Delay_ms(120);       //延时等待内部的时钟等初始化成功，完成自检
	
	LCD_WriteCmd(0x36);  // MX MY RGB mode
											//内存数据的访问控制
	LCD_WriteData(0xc8);	//命令的参数 ：
												// MY MX MV ML RGB MH  -  - 
	
	LCD_WriteCmd(0x3a);   //65k mode
												//像素格式设置 
	LCD_WriteData(0x05); //像素格式的参数
											 //  16bit/pix
	
	LCD_WriteCmd(0x29);  //显示
}


// LCD SET_Region
void LCD_SetRegion(uint16_t x_start,uint16_t y_start,\
										uint16_t x_end,uint16_t y_end)
{
	LCD_WriteCmd(0x2a);   //设置列地址
	LCD_WriteData(0x00);  //参数1   起始地址的高八位
	LCD_WriteData(x_start + 2); //参数2   起始地址的低八位
	LCD_WriteData(0x00);			//参数3    结束地址的高八位
	LCD_WriteData(x_end + 2);  //参数4   结束地址的低八位
	
	LCD_WriteCmd(0x2b);   //设置行地址
	LCD_WriteData(0x00);  //参数1   
	LCD_WriteData(y_start + 3); //参数2
	LCD_WriteData(0x00);      //参数3
	LCD_WriteData(y_end + 3);  //参数4
	
	LCD_WriteCmd(0x2c);    //将参数写入到寄存器
}


// LCD 设置显示起始点
void LCD_SetXY(uint16_t x,uint16_t y)
{
	LCD_SetRegion(x,y,x,y);
}


//LCD 画一个点
void GUI_DrawPoint(uint16_t x,uint16_t y,uint16_t Data)
{
	LCD_SetRegion(x,y,x + 1,y + 1);
	LCD_WriteData_16Bit(Data);
}


//读取TFT某一点的颜色
unsigned int LCD_ReadPoint(uint16_t x,uint16_t y)
{
	unsigned int Data;
	LCD_SetXY(x,y);
	
	LCD_WriteData(Data);
	return Data;
}


//LCD_CLEAR 
void LCD_Clear(uint16_t color)
{
	unsigned int i,m;
	LCD_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
	LCD_WriteCmd(0x2c);
	for(i=0;i < X_MAX_PIXEL;i++){
		for(m=0;m< Y_MAX_PIXEL;m++){
			LCD_WriteData_16Bit(color);
		}
	}
}


//水平扫描的方式，从左到右，低位在前
void Show_Image(const unsigned char *p)
{
	int i;
	int tmp;
	unsigned int picH,picL;
	//LCD_Clear(BLACK);
	LCD_SetRegion(0,0,127,127);
	LCD_WriteCmd(0x2c);
	for(i=0;i<128*128;i++){
		picH = *(p+i*2);
		tmp = picH;
		picL = *(p+i*2+1);
		tmp = picL;
		LCD_WriteData_16Bit(picH<<8|picL);
	}
}



//
void Gui_DrawFont_GBK16(uint16_t x, uint16_t y,uint16_t fc, uint8_t *s)
{
	unsigned char i,j;
	unsigned short k,x0;
	x0=x;

	while(*s) 
	{	
		if((*s) < 128)     //判断是ascii 表中的内容还是其他的汉字
		{
			k=*s;
			if (k == 13)    //回车符号的处理
			{
				x=x0;
				y+=16;
			}
			else            //正常字符的处理
			{
				if (k>32) k-=32; else k=0;     //不处理打印控制字符
	
			  for(i=0;i<16;i++)
				for(j=0;j<8;j++) 
					{
				    	if(asc16[k*16+i]&(0x80>>j))
								GUI_DrawPoint(x+j,y+i,fc);
					}
				x+=8;
			}
			s++;   //下一个字符
		}
		else       //汉字的处理方式  
		{
			for (k=0;k<hz16_num;k++) 
			{
			  if ((hz16[k].Index[0]==*(s))&&(hz16[k].Index[1]==*(s+1)))
			  { 
				    for(i=0;i<16;i++)
				    {
						for(j=0;j<8;j++) 
							{
						    if(hz16[k].Msk[i*2]&(0x80>>j))
										GUI_DrawPoint(x+j,y+i,fc);
							}
						for(j=0;j<8;j++) 
							{
								if(hz16[k].Msk[i*2+1]&(0x80>>j))	
										GUI_DrawPoint(x+j+8,y+i,fc);
							}
				    }
				}
			 }
			s+=2;x+=16;  //一个汉字占两个字节  一个汉字的点阵为 16*16 
		} 
		
	}
}
