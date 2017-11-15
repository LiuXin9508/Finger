/*  功能：
*					主要用来完成板子需要使用到的GPIO端口的配置  
*		说明：
*					PA0 <==> tc4-1
*					PA1 <==>	RST_LOW
*					PA4 <==>	SS
*					PA5 <==>	SCK
*					PA6 <==>	MISO
*					PA7 <==>	MOSI
*			====================
*					PB0 <==> key3
*					PB1 <==> key2
*/

#include "gpio.h"
#define debug

uint8_t KEY2_FLAG = 0;
uint8_t KEY3_FLAG = 0;

void Board_GpioInit(void)
{
	//定义需要初始化的结构体
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//使能对应的时钟(GPIO挂载在AHB总线上)
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	
	//GPIOA引脚的输入输出模式以及详细配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 |GPIO_Pin_4 | \
																GPIO_Pin_5 |GPIO_Pin_6 |GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	//初始化GPIOA
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//GPIOB引脚的输入输出模式以及详细配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	
	//初始化GPIOB
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
#ifdef debug
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
	//使能APB总线的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	//配置中断连接到那个触发线
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource0);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource1);
	//配置中断的触发方式
	EXTI_InitTypeDef exit_gpio;
	exit_gpio.EXTI_Line = EXTI_Line0;
	exit_gpio.EXTI_Mode = EXTI_Mode_Interrupt;
	exit_gpio.EXTI_Trigger = EXTI_Trigger_Falling;
	exit_gpio.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exit_gpio);
	
	exit_gpio.EXTI_Line = EXTI_Line1;
	exit_gpio.EXTI_Mode = EXTI_Mode_Interrupt;
	exit_gpio.EXTI_Trigger = EXTI_Trigger_Falling;
	exit_gpio.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exit_gpio);
	
	//配置 m0 核心中 NVID 
	NVIC_InitTypeDef NVIC_init;
	NVIC_init.NVIC_IRQChannel = EXTI0_1_IRQn;
	NVIC_init.NVIC_IRQChannelPriority = 0x00;
	NVIC_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_init);
	
}
