/*   本文件主要用来封装对 Board 的串口使用接口函数   */

#include "uart.h"

extern uint8_t uart2_recv_buf[BUFFER_SIZE];
__IO uint8_t UART_SEND_FLAG = 0;
__IO uint8_t UART_RECV_FLAG = 0;

uint8_t TxBuffer1[] = "Communication between USART1-USART2 using DMA2";
extern __IO uint8_t RxCounter, ReceiveState;
extern __IO uint8_t person; 
extern uint8_t aRxBuffer[128];
extern uint8_t add_finger;
extern uint8_t del_finger;
extern uint8_t del_all;

#define TXBUFFERSIZE          (countof(TxBuffer1) - 1)
#define RXBUFFERSIZE          TXBUFFERSIZE


uint8_t RxBuffer2 [RXBUFFERSIZE] = {0};

void User_USART_Init(void)
{
	 USART_InitTypeDef uart1,uart2;
	 
	 uart1.USART_BaudRate = 115200;    //波特率 115200
   uart1.USART_WordLength = USART_WordLength_8b;   // 8 bits
   uart1.USART_StopBits = USART_StopBits_1;   //  1 个停止位
   uart1.USART_Parity = USART_Parity_No;			// 没有奇偶校验
   uart1.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //
   uart1.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;   // 模式选择
	
	 uart2.USART_BaudRate = 57600;    //波特率 57600
   uart2.USART_WordLength = USART_WordLength_8b;   // 8 bits
   uart2.USART_StopBits = USART_StopBits_1;   //  1 个停止位
   uart2.USART_Parity = USART_Parity_No;			// 没有奇偶校验
   uart2.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //
   uart2.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;   // 模式选择
	
	 //时钟的设置
	 /* Enable GPIO clock */
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	 //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
   
   /* Enable 8xUSARTs Clock */
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);  
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	 /* Enable Syscfg */
   //RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	 
   
   /* USART1 Pins configuration ************************************************/
	 GPIO_InitTypeDef GPIO_InitStructure;
   /* Connect pin to Periph */
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1); 
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);    
   /* Configure pins as AF pushpull */
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
   GPIO_Init(GPIOA, &GPIO_InitStructure);  
	 USART_Init(USART1, &uart1);
	 
	 
   /* USART2 Pins configuration ************************************************/  
   /* Connect pin to Periph */
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
   GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);    
   
   /* Configure pins as AF pushpull */
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	 
   GPIO_Init(GPIOA, &GPIO_InitStructure);
	 USART_Init(USART2, &uart2);
	  
	
	
	 /*------------------------------- DMA---------------------------------------*/ 
	 //DMA_InitTypeDef dma;
	 	 
   /* Common DMA configuration */
   //dma.DMA_BufferSize = 128;  //指定通道中数据缓冲区的大小
   //dma.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //指定外围数据的大小
   //dma.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;   //指定内存数据的大小
   //dma.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //指定外围地址寄存器是否递增
   //dma.DMA_MemoryInc = DMA_MemoryInc_Enable;      //指定内存寄存器是否递增
   //dma.DMA_Mode = DMA_Mode_Normal;            //操作模式
   //dma.DMA_M2M = DMA_M2M_Disable;        //指定是否使能内存到内存的传输
   
   /* DMA1 Channel1 configuration */
   //dma.DMA_MemoryBaseAddr = (uint32_t)&uart2_recv_buf;  //指定内存的基地址
   //dma.DMA_DIR = DMA_DIR_PeripheralDST;    //指定外围设备是作为源还是目标
   //dma.DMA_Priority = DMA_Priority_Low;    //指定优先级
   //dma.DMA_PeripheralBaseAddr = USART2_RDR_Address;  //指定外围的基地址

	 //DMA_Init(DMA1_Channel1, &dma);
   
   /* Clear DMA2 TC flags */
   //DMA_ClearFlag(DMA1_FLAG_TC1);
	 //USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
	 
	 /* Enable the DMA2 channels */
   //DMA_Cmd(DMA1_Channel2, ENABLE);
	
	/* Enable USARTs Receive interrupts */
	//单字节传输完成中断
	 USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	//数据帧传输完成中断
	 USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	 
	 USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	 USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	
	 NVIC_InitTypeDef NVIC_InitStructure;  
  /* USART1 IRQ Channel configuration */
   NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPriority = 0x01;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
   
   /* USART2 IRQ Channel configuration */
   NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPriority = 0x01;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
	 
	    
   /* Enable the USARTs */
	 USART_Cmd(USART1, ENABLE);
	 USART_Cmd(USART2, ENABLE);
	 
	 //USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	 //USART_ClearITPendingBit(USART1,USART_IT_IDLE);
}

void USART1_String(uint8_t *Data)
{
		uint8_t TxCounter = 0;
		uint8_t *buffptr = Data;
		/* Send one byte from USART1 to USARTx */
		while(TxCounter <= strlen((char *)Data))
		{ 
			USART_SendData(USART1,buffptr[TxCounter++]);
			/* Loop until USART1 DR register is empty */ 
			while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
			{}
			//Delay(50);
		}
}

void USART2_String(uint8_t *Data)
{
		uint8_t TxCounter = 0;
		uint8_t *buffptr = Data;
		/* Send one byte from USART1 to USARTx */
		while(TxCounter <= strlen((char *)Data))
		{ 
			USART_SendData(USART2,buffptr[TxCounter++]);
			/* Loop until USART1 DR register is empty */ 
			while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
			{}
		}
}

void USART1_SendData(uint8_t *Data,uint8_t length)
{
		uint8_t TxCounter = 0;
		uint8_t *buffptr = Data;
		/* Send one byte from USART1 to USARTx */
		while(TxCounter < length)
		{ 
			USART_SendData(USART1,buffptr[TxCounter++]);
			/* Loop until USART1 DR register is empty */ 
			while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
			{}
		}
}

void USART2_SendData(uint8_t *Command,uint8_t length)
{
		uint8_t TxCounter = 0;
		uint8_t *buffptr = Command;
		/* Send one byte from USART1 to USARTx */
		while(TxCounter < length)
		{ 
			USART_SendData(USART2,buffptr[TxCounter++]);
			/* Loop until USART1 DR register is empty */ 
			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
			{}
		}
}

uint8_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
   while (BufferLength--)
   {
     if (*pBuffer1 != *pBuffer2)
     {
       return 0;
     }
     pBuffer1++;
     pBuffer2++;
   }
   return 1;
}

void USART1_RECV_Handle(void)
{
	if(ReceiveState == 1){
		/*uint8_t buff[20] = {0};
		sprintf((char *)buff,"%s!",aRxBuffer);
		Gui_DrawFont_GBK16(1,80,RED,GRAY2,buff);
		while(1);*/
		ReceiveState = 0;
		if(aRxBuffer[1] == 0x01 && aRxBuffer[4] == 0x01)
			add_finger = 1;
		else if(aRxBuffer[4] == 0x02){
			del_finger = 1;
			person = aRxBuffer[5];
		}else if(aRxBuffer[4] == 0x03){
			del_all = 1;
			del_finger = 1;
		}
	}
} 