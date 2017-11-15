/*  ���ܣ�
*					��Ҫ������ɰ�����Ҫʹ�õ���GPIO�˿ڵ�����  
*		˵����
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
	//������Ҫ��ʼ���Ľṹ��
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//ʹ�ܶ�Ӧ��ʱ��(GPIO������AHB������)
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	
	//GPIOA���ŵ��������ģʽ�Լ���ϸ����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 |GPIO_Pin_4 | \
																GPIO_Pin_5 |GPIO_Pin_6 |GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	//��ʼ��GPIOA
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//GPIOB���ŵ��������ģʽ�Լ���ϸ����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	
	//��ʼ��GPIOB
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
#ifdef debug
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
	//ʹ��APB���ߵ�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	//�����ж����ӵ��Ǹ�������
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource0);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource1);
	//�����жϵĴ�����ʽ
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
	
	//���� m0 ������ NVID 
	NVIC_InitTypeDef NVIC_init;
	NVIC_init.NVIC_IRQChannel = EXTI0_1_IRQn;
	NVIC_init.NVIC_IRQChannelPriority = 0x00;
	NVIC_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_init);
	
}
