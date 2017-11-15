#include "main.h"
#include "app.c"   //图片的要使用的像素点数组信息

#define BSRR_VAL 0x04

extern uint8_t KEY2_FLAG;
extern uint8_t KEY3_FLAG;

extern uint32_t Data_Temp;
extern uint32_t Data_Start;
extern uint32_t Data_End;

extern const unsigned char gImage_app[32768];

__IO uint8_t RxCounter = 0,ReceiveState = 0;
__IO uint8_t RxCounter2 = 0,ReceiveState2 = 0;

extern uint8_t RxBuffer2[];
extern uint8_t RxBuffer1[];
extern uint8_t TxBuffer1[];

uint8_t uart2_recv_buf[128];

unsigned char Show_find_man_num[128] = {0};

extern uint16_t Person_num ;
volatile static uint16_t FingerCount = 0; //用于存放指纹个数
__IO uint8_t person = 0; 

uint8_t aRxBuffer[128];
uint8_t aRxBuffer2[128];
__IO uint8_t TxCounter = 0;
__IO uint8_t TxCounter2 = 0;
uint8_t  rcvlen = 0;
uint8_t  rcvlen2 = 0;
uint8_t add_finger = 0;
uint8_t del_finger = 0;
uint8_t del_all = 0;
__IO uint16_t NUM_Finger = 0;

static uint16_t finger_flag[100] = {0};

void LCD_SHOW(void)
{
	Show_Image(gImage_app);
	//Gui_DrawFont_GBK16(25,0,RED,(uint8_t *)"指纹打卡机");
}

int Flash_Read(uint32_t addr,uint16_t *buf,uint32_t len)
{
	int i = 0;
	while(i < len){
		buf[i++] = *(uint16_t *)addr; 
		addr+=2;
	}
	return i;
}
void Flash_pageWrite(uint32_t addr,uint16_t *buff,uint32_t len)
{
	uint16_t tmp = 0;
	FLASH_Status FLASHStatus = FLASH_COMPLETE;
	for(tmp = 0;(tmp < len)&&(FLASHStatus == FLASH_COMPLETE);tmp++)
	{
		FLASHStatus = FLASH_ProgramHalfWord(addr,*buff);
		addr+=2;
		buff++;
	}
}

void Count_Init(void)
{
	uint16_t tmpbuf[100] = {0};
	FLASH_Unlock();
	Flash_Read(0x08007c00,tmpbuf,100);
	Delay_ms(200);
	FLASH_Lock();
	for(int i = 0; i<100;i++){
		if(tmpbuf[i] == 0x1)
			finger_flag[i] = 1;
		else
			finger_flag[i] = 0;
	}
	return;
}


int main(void)
{
 /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f0xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
      system_stm32f0xx.c file
    */
	uint8_t time_flag = 0;
	Board_GpioInit();
	LCD_Init();
	User_USART_Init();
	LCD_Clear(GRAY2);
	
	
	
	
	LCD_SHOW();
	//GPIOB->BSRR = BSRR_VAL;
	TIP_SUCCESS();
	
	while(Get_NUM()!=0);
	FingerCount = NUM_Finger;
	
	sprintf((char *)Show_find_man_num,"GET NUM %d!",FingerCount);
	Gui_DrawFont_GBK16(15,80,GRAY2,Show_find_man_num);
	
	RCC_HSICmd(ENABLE);
	FLASH_SetLatency(FLASH_Latency_1);
	FLASH_PrefetchBufferCmd(ENABLE);
	
	Count_Init();
	/*uint16_t buf1[5] = {0,1,1,0,1};
	FLASH_Unlock();
	FLASH_ErasePage(0x08007400);
	Delay_ms(100);
	Flash_pageWrite(0x08007400,buf1,5);
	FLASH_Lock();*/
	GPIOB->BSRR = BSRR_VAL;
	
	/*while(1)
	{
		USART1_SendData("A",1);
		//Write_to_Flash(Data_Temp,buf,512,Data_Start,Data_End);
		
		
		
		uint16_t buf1[10] = {0};
		FLASH_Unlock();
		Flash_Read(0x08007400,buf1,5);
		Delay_ms(300);
		FLASH_Lock();
	
		USART1_SendData((uint8_t *)&buf1[0],1);
		USART1_SendData((uint8_t *)&buf1[1],1);
		USART1_SendData((uint8_t *)&buf1[2],1);
		USART1_SendData((uint8_t *)&buf1[3],1);
		USART1_SendData((uint8_t *)&buf1[4],1);
		if(buf1[0] == 0x08)
			GPIOB->BSRR = BSRR_VAL;
		
	}*/
	while(1)
	{
		USART1_RECV_Handle();
		//以下代码是指纹模块 使用 USART 通信的测试代码 
		if(KEY2_FLAG == 1 || add_finger == 1)		//按下KEY2，开始录入指纹
		{
			KEY2_FLAG = 0;
			add_finger = 0;
buffer1:		
			LCD_SHOW();
			Gui_DrawFont_GBK16(15,80,RED,(uint8_t *)"Press finger!");
			while(Finger_Search_AND_Save_Image() != 0);
			while(Finger_Feature_AND_Save_charbuffer1or2(0x01) != 0){goto buffer1;}
			
buffer2:			
			LCD_SHOW();
			Gui_DrawFont_GBK16(15,80,RED,(uint8_t *)"Press again!");
			while(Finger_Search_AND_Save_Image() != 0);
			LCD_SHOW();
			Gui_DrawFont_GBK16(15,80,RED,(uint8_t *)"Save_Image !");
			while(Finger_Feature_AND_Save_charbuffer1or2(0x02) != 0){goto buffer2;}
			LCD_SHOW();
			Gui_DrawFont_GBK16(15,80,RED,(uint8_t *)"Save_charbuff!");
			while(1){
				if(Combine_Finger_Feature() != 0){
					time_flag++;
					while(time_flag == 4){
						time_flag = 0;
						goto buffer1;
					}
				}else{
					LCD_SHOW();
					Gui_DrawFont_GBK16(15,80,RED,(uint8_t *)"Combine_Finger!");
					for(int i = 0 ;i < 100;i++)
					{
						if(finger_flag[i]== 0){
							FingerCount = i;
							break;
						}
					}
					if(Save_Finger_To_Flash(0x01,FingerCount) == 0)
					{
						finger_flag[FingerCount] = 1;
						FLASH_Unlock();
						FLASH_ErasePage(0x08007c00);
						Delay_ms(100);
						Flash_pageWrite(0x08007c00,finger_flag,100);
						Delay_ms(100);
						FLASH_Lock();
						
						//FingerCount++;
						LCD_SHOW();
						Gui_DrawFont_GBK16(15,80,RED,(uint8_t *)"Finger In OK!");
						TIP_WRITE_SUCCESS();
						//回复应打包
						ASCK_ADD(0x00,FingerCount);
					}
					else
					{
						TIP_FAILED();
					}
					time_flag = 0;
					break;
				}
			}		
		}
		else if(KEY3_FLAG == 1 || del_finger == 1)
		{
			KEY3_FLAG = 0;
			del_finger = 0;
			//USART1_String("KEY3 Interrupt !\n");
			if(del_all == 1){
				del_all = 0;
				while(Empty_The_FinLib() != 0);
				LCD_SHOW();
				Gui_DrawFont_GBK16(15,80,RED,(uint8_t *)"Delate All !");
				TIP_DELATE_ALL();
				
				FingerCount = 0;
				for(int i =0 ;i<100;i++)
					finger_flag[i] = 0;
				FLASH_Unlock();
				FLASH_ErasePage(0x08007c00);
				Delay_ms(100);
				Flash_pageWrite(0x08007c00,finger_flag,100);
				Delay_ms(100);
				FLASH_Lock();
				
			}else{
				//与PC机交互 回送应答包
				while(Del_finger(person)!= 0);
				
				finger_flag[person] = 0;
				FLASH_Unlock();
				FLASH_ErasePage(0x08007c00);
				Delay_ms(100);
				Flash_pageWrite(0x08007c00,finger_flag,100);
				Delay_ms(100);
				FLASH_Lock();
				
				LCD_SHOW();
				Gui_DrawFont_GBK16(15,80,RED,(uint8_t *)"Delate One !");
				ASCK_DEl(0x00);
				TIP_DELATE_ALL();
			}
		}
		else  //扫描指纹，比对
		{	
			if(Finger_Search_AND_Save_Image() == 0){
				if(Finger_Feature_AND_Save_charbuffer1or2(0x01) == 0){
					if(Search_Fingerlib(0x01,0x0000,0x039E) == 0){
						LCD_SHOW();
						sprintf((char *)Show_find_man_num,"Find man %d!",Person_num);
						Gui_DrawFont_GBK16(15,80,RED,Show_find_man_num);
						TIP_SUCCESS();
						
						//指纹识别成功，上传数据包给 PC 机
						Send_Match_OK(Person_num);
					}
				  else
				  {
						LCD_SHOW();
						Gui_DrawFont_GBK16(15,80,RED,(uint8_t *)"Can`t Find!");
						TIP_FAILED();
					}					
				}
			}
		}
		Delay_ms(1000);
		LCD_SHOW();
	}
}

#ifdef  USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file: pointer to the source file name
* @param  line: assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
 
  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
* @}
*/

/**
* @}
*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
