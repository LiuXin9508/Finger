/*  ���ļ���ҪҪ����װ�� LCD ��ʾ���Ĳ���  */
/********************************************   
*���ӵĽӿڶ��壺
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

/*һ���򵥵���ʱ����*/
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
		/* ģ�� SPI �����ݷ��� */
		/*  PA_7 <--> MOSI */
		if(Data & 0x80){
			GPIOA->BSRR = GPIO_Pin_7;
		}else {
			GPIOA->BRR = GPIO_Pin_7;
		}
		/*ʹ��IO ����ģ��ʱ��*/
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
	SPI_WriteData(Data >> 8);   //д��߰�λ����
	SPI_WriteData(Data);	//д��Ͱ�λ����
	GPIOA->BSRR = GPIO_Pin_4;
}


//��Һ����д��һ��8λָ��
void LCD_WriteCmd(uint8_t cmd)
{
	// SPI д����ʱ��ʼ
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


// LCD ��ʼ��
void LCD_Init(void)
{
	LCD_WriteCmd(0x11);  // �˳�����
	Delay_ms(120);       //��ʱ�ȴ��ڲ���ʱ�ӵȳ�ʼ���ɹ�������Լ�
	
	LCD_WriteCmd(0x36);  // MX MY RGB mode
											//�ڴ����ݵķ��ʿ���
	LCD_WriteData(0xc8);	//����Ĳ��� ��
												// MY MX MV ML RGB MH  -  - 
	
	LCD_WriteCmd(0x3a);   //65k mode
												//���ظ�ʽ���� 
	LCD_WriteData(0x05); //���ظ�ʽ�Ĳ���
											 //  16bit/pix
	
	LCD_WriteCmd(0x29);  //��ʾ
}


// LCD SET_Region
void LCD_SetRegion(uint16_t x_start,uint16_t y_start,\
										uint16_t x_end,uint16_t y_end)
{
	LCD_WriteCmd(0x2a);   //�����е�ַ
	LCD_WriteData(0x00);  //����1   ��ʼ��ַ�ĸ߰�λ
	LCD_WriteData(x_start + 2); //����2   ��ʼ��ַ�ĵͰ�λ
	LCD_WriteData(0x00);			//����3    ������ַ�ĸ߰�λ
	LCD_WriteData(x_end + 2);  //����4   ������ַ�ĵͰ�λ
	
	LCD_WriteCmd(0x2b);   //�����е�ַ
	LCD_WriteData(0x00);  //����1   
	LCD_WriteData(y_start + 3); //����2
	LCD_WriteData(0x00);      //����3
	LCD_WriteData(y_end + 3);  //����4
	
	LCD_WriteCmd(0x2c);    //������д�뵽�Ĵ���
}


// LCD ������ʾ��ʼ��
void LCD_SetXY(uint16_t x,uint16_t y)
{
	LCD_SetRegion(x,y,x,y);
}


//LCD ��һ����
void GUI_DrawPoint(uint16_t x,uint16_t y,uint16_t Data)
{
	LCD_SetRegion(x,y,x + 1,y + 1);
	LCD_WriteData_16Bit(Data);
}


//��ȡTFTĳһ�����ɫ
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


//ˮƽɨ��ķ�ʽ�������ң���λ��ǰ
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
		if((*s) < 128)     //�ж���ascii ���е����ݻ��������ĺ���
		{
			k=*s;
			if (k == 13)    //�س����ŵĴ���
			{
				x=x0;
				y+=16;
			}
			else            //�����ַ��Ĵ���
			{
				if (k>32) k-=32; else k=0;     //�������ӡ�����ַ�
	
			  for(i=0;i<16;i++)
				for(j=0;j<8;j++) 
					{
				    	if(asc16[k*16+i]&(0x80>>j))
								GUI_DrawPoint(x+j,y+i,fc);
					}
				x+=8;
			}
			s++;   //��һ���ַ�
		}
		else       //���ֵĴ���ʽ  
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
			s+=2;x+=16;  //һ������ռ�����ֽ�  һ�����ֵĵ���Ϊ 16*16 
		} 
		
	}
}
