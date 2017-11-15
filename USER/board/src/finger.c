/*  本文件主要封装对指纹识别模块的操作函数  */

#include "finger.h"
#include "uart.h"
#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "stdarg.h"

extern uint8_t uart2_recv_buf[128];
extern uint8_t UART_SEND_FLAG;
extern __IO uint8_t ReceiveState2;
extern uint8_t  rcvlen2;

_SYS_CMD_PACK FPM_CMD;       //系统指令实例
uint8_t FPM_Data[128] = {0}; //用于存放指令数据域的值
uint8_t UART_FPM_CMD[256];   //用于存放指令，方面串口发送
uint8_t fcmd_length;
uint16_t Person_num = 0;     //用于标记指纹


/*=======================================================*/
/*         以下函数：   蜂鸣器   提示音函数             */
/*=======================================================*/
//使用简单的延时函数来控制蜂鸣器发声时间
void Delay(int time) 
{
	int i,j;
	for(i=0;i<time*10;i++)
	{
		for(j=0;j<100;j++);
	}
}

void TIP_SUCCESS(void) //成功识别到指纹  短声1次
{
	GPIOA->BRR = GPIO_Pin_0;
	Delay(250);
	GPIOA->BSRR = GPIO_Pin_0;
}

void TIP_FAILED(void) //指纹识别失败  长声1次
{
	GPIOA->BRR = GPIO_Pin_0;
	Delay(500);
	GPIOA->BSRR = GPIO_Pin_0;
}

void TIP_WRITE_SUCCESS(void) //指纹录入成功 短声 2次
{
	GPIOA->BRR = GPIO_Pin_0;
	Delay(250);
	GPIOA->BSRR = GPIO_Pin_0;
	Delay(1000);
	GPIOA->BRR = GPIO_Pin_0;
	Delay(250);
	GPIOA->BSRR = GPIO_Pin_0;
}

void TIP_WRITE_FAILED(void) //指纹录入失败   短声1次 长声1次
{
	GPIOA->BRR = GPIO_Pin_0;
	Delay(250);
	GPIOA->BSRR = GPIO_Pin_0;
	Delay(1000);
	GPIOA->BRR = GPIO_Pin_0;
	Delay(500);
	GPIOA->BSRR = GPIO_Pin_0;
}

void TIP_DELATE_ALL(void)  //删除所有指纹模板 长声1次  短声1次
{
	GPIOA->BRR = GPIO_Pin_0;
	Delay(500);
	GPIOA->BSRR = GPIO_Pin_0;	
	Delay(1000);
	GPIOA->BRR = GPIO_Pin_0;
	Delay(250);
	GPIOA->BSRR = GPIO_Pin_0;	
}

/*==========================================================*/
/*                指纹模块的操作函数                       */
/*==========================================================*/

void SHOW_CMD(uint8_t *ch, uint8_t size)
{
	uint8_t i;
	for(i=0; i<size; i++)
	{
		printf("%02x ",ch[i]);
	}
	printf("\n");
}

//extern uint8_t UART_SEND_FLAG;

//使用 USART2 给指纹模块发送命令（数据包）
uint8_t  Send_Command(uint8_t *command, uint8_t num)
{
	USART2_SendData(command,num);
	Delay(600);
	return 0;
}

//获取要发送的包长度（大小）
//同时将数据封包
uint8_t Get_Pack_Length(uint8_t pid, uint8_t *data, uint8_t data_length)
{
	uint8_t i = 0, temp = 0;
	
	//包头，用来说明是谁的包
	FPM_CMD.Head[0] = 0xEF;
	FPM_CMD.Head[1] = 0x01;
	//串口要发送的数据填充： 填充包头的信息
	UART_FPM_CMD[0] = FPM_CMD.Head[0];
	UART_FPM_CMD[1] = FPM_CMD.Head[1];
	
	//地址，表明模块的地址
	FPM_CMD.Addr[0] = 0xFF;
	FPM_CMD.Addr[1] = 0xFF;
	FPM_CMD.Addr[2] = 0xFF;
	FPM_CMD.Addr[3] = 0xFF;
	//串口要发送的数据填充： 填充地址信息
	UART_FPM_CMD[2] = FPM_CMD.Addr[0];
	UART_FPM_CMD[3] = FPM_CMD.Addr[1];
	UART_FPM_CMD[4] = FPM_CMD.Addr[2];
	UART_FPM_CMD[5] = FPM_CMD.Addr[3];
	
	//包的类型
	FPM_CMD.PID = pid;
	//串口要发送的数据填充： 填充包类型
	UART_FPM_CMD[6] = FPM_CMD.PID;
	
	//包的长度信息：
	FPM_CMD.Length[0] = (data_length + 2) >> 8;
	FPM_CMD.Length[1] = data_length + 2;
	//串口要发送的数据填充： 填充包长度，便于接收方知道如何获取数据
	UART_FPM_CMD[7] = FPM_CMD.Length[0];
	UART_FPM_CMD[8] = FPM_CMD.Length[1];
	
	//包中包含的数据
	FPM_CMD.Data = data;
	//对串口要发送数据的填充：填充实际的对方需要的信息
	for(i=0; i<data_length; i++)
	{
		temp += FPM_CMD.Data[i];
		UART_FPM_CMD[9+i] = FPM_CMD.Data[i];
	}
	
	//此处在进行数据和校验；便于接受方通过此段数据判断接收是否出错
	FPM_CMD.SUM[0] = (pid + temp + data_length + 2) >> 8;
	FPM_CMD.SUM[1] = (pid + temp + data_length + 2);
	//串口要发送数据的填充：填充数据和校验码
	UART_FPM_CMD[9+data_length] = FPM_CMD.SUM[0];
	UART_FPM_CMD[10+data_length] = FPM_CMD.SUM[1];
	
	//返回整个串口呀发送的数据的字节数
	return 11+data_length;
}



//本函数主要用来封装数据
/*这部分主要填充指令，以及指令需要的一些附加参数 */
/* 可变参数  ...    引用标准C 的 stdarg.h 后即可使用 */
uint8_t Get_Data_Length(uint8_t para_num, ...)
{
	uint8_t num = 0;
	va_list valist; //用于存放参数
	va_start(valist, para_num);
	for(num=0; num < para_num; num++){
		FPM_Data[num] = va_arg(valist,int); //将第二个参数存入全局
	}
	va_end(valist);
	return para_num;
}


/*读取指纹信息，并且保存 image 到内部的 buffer 中 */
uint8_t Finger_Search_AND_Save_Image(void)
{
	//首先清空 USART2 的接收数据缓冲区
	/*   注意： 不是内部的buffer寄存器 ，是自己定义的一块内存区域  */
	memset(uart2_recv_buf,0,sizeof(uart2_recv_buf));
	
	//调用数据的封包函数，对发送给指纹模块的数据封包，同时返回 USART 要发送的字节数
	fcmd_length = Get_Pack_Length(0x01,FPM_Data,Get_Data_Length(1,GENIMG)); 
	
	//使用 USART 发送数据
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//延时不能太少
	//uint8_t hh = 0xa;
	//USART1_SendData(&hh,1);
	//USART1_SendData(UART_FPM_CMD,fcmd_length);
	//USART1_SendData(&hh,1);
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//延时不能太少
	
	
	//USART1_SendData(&hh,1);
	while(ReceiveState2 != 1);
	{
		ReceiveState2 = 0;
		//USART1_SendData(uart2_recv_buf,rcvlen2);
	}
	//USART1_SendData(&hh,1);
	//while(ReceiveState2 != 1);
	if((uart2_recv_buf[9] == 0x00) && (uart2_recv_buf[6] == 0x07)) //表示指纹录入成功
	{
		//指纹探测成功
		//USART1_String("1:指纹探测成功!\n");
		return 0;
	}
	else if(uart2_recv_buf[9] == 0x01)
	{
		//收到的数据包有错
		//USART1_String("1:收到的数据包有错!\n");
		return 1;
	}
	else if(uart2_recv_buf[9] == 0x02)
	{
		//没有检测到手指
		//USART1_String("1:没有检测到手指!\n");
		return 2;
	}
	else if(uart2_recv_buf[9] == 0x03)
	{
		//手指录入不成功
		//USART1_String("1:手指录入不成功!\n");
		return 3;
	}
	else
	{
		return 4;
	}
}

/*获取指纹图像的特征信息，保存到 charbuffer1 或者 charbuffer2 */
uint8_t Finger_Feature_AND_Save_charbuffer1or2(uint8_t area_num)
{
	memset(uart2_recv_buf,0,sizeof(uart2_recv_buf));
	
	fcmd_length = Get_Pack_Length(0x01,FPM_Data,Get_Data_Length(2,IMG2TZ,area_num));
	
	//USART1_SendData(&hh,1);
	//USART1_SendData(UART_FPM_CMD,fcmd_length);
	//USART1_SendData(&hh,1);
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//延时不能太少
	
	//USART1_SendData(&hh,1);
	while(ReceiveState2 != 1);
	{
		ReceiveState2 = 0;
		//USART1_SendData(uart2_recv_buf,rcvlen2);
	}
	//USART1_SendData(&hh,1);
	//while(ReceiveState2 != 1);
	if((uart2_recv_buf[9] == 0x00) && (uart2_recv_buf[6] == 0x07))//表示生成特征成功
	{
		//生成特征成功
		//USART1_String("2:生成特征成功!\n");
		return 0;		
	}
	else if(uart2_recv_buf[9] == 0x01)
	{
		//收到的数据包有错
		//USART1_String("2:收到的数据包有错!\n");
		return 1;
	}
	else if(uart2_recv_buf[9] == 0x06)
	{
		//不能生成特征
		//USART1_String("2:不能生成特征!\n");
		return 2;
	}
	else if(uart2_recv_buf[9] == 0x07)
	{
		//特征点太少不能生成特征
		//USART1_String("2:特征点太少不能生成特征!\n");
		return 3;		
	}
	else if(uart2_recv_buf[9] == 0x15)
	{
		//图像缓冲区没有图像不能生成特征
		//USART1_String("2:图像缓冲区没有图像不能生成特征!\n");
		return 4;		
	}
	else
	{
		//其他原因
		//USART1_String("2:其他原因!\n");
		return 5;
	}
}

/*合并两个 charbuffer 中的特征点 成为一个指纹模板 */
uint8_t Combine_Finger_Feature(void)
{
  memset(uart2_recv_buf,0,sizeof(uart2_recv_buf));
	fcmd_length = Get_Pack_Length(0x01,FPM_Data,Get_Data_Length(1,REGMODEL)); 

	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//延时不能太少
	
	//uint8_t hh = 0xa;
	//USART1_SendData(&hh,1);
	//USART1_SendData(UART_FPM_CMD,fcmd_length);
	//USART1_SendData(&hh,1);
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//延时不能太少
	
	//USART1_SendData(&hh,1);
	while(ReceiveState2 != 1);
	{
		ReceiveState2 = 0;
		//USART1_SendData(uart2_recv_buf,rcvlen2);
	}
	//USART1_SendData(&hh,1);
	//while(ReceiveState2 != 1);
	if((uart2_recv_buf[9] == 0x00) && (uart2_recv_buf[6] == 0x07))//表示特征合并成功
	{
		//合并成功
		//USART1_String("33:合并成功!\n");
		return 0;				
	}
	else if(uart2_recv_buf[9] == 0x01)
	{
		//数据收包有错
		//USART1_String("33:数据收包有错!\n");
		return 1;
	}
	else if(uart2_recv_buf[9] == 0x0a)
	{
		//两个模板不属于一个人
		//USART1_String("33:两个模板不属于一个人!\n");
		return 2;
	}
	else
	{
		//USART1_String("33:其他原因!\n");
		return 3;
	}
}

/*将指纹模板存放到内部的 flash 中 */
uint8_t Save_Finger_To_Flash(uint8_t BufferID, uint16_t PageID)
{
	memset(uart2_recv_buf,0,sizeof(uart2_recv_buf));
	fcmd_length = Get_Pack_Length(0x01,FPM_Data,Get_Data_Length(4,STORE,BufferID,(PageID>>8),PageID)); 

	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//延时不能太少
	
	//uint8_t hh = 0xa;
	//USART1_SendData(&hh,1);
	//USART1_SendData(UART_FPM_CMD,fcmd_length);
	//USART1_SendData(&hh,1);
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//延时不能太少
	
	//USART1_SendData(&hh,1);
	while(ReceiveState2 != 1);
	{
		ReceiveState2 = 0;
		//USART1_SendData(uart2_recv_buf,rcvlen2);
	}
	//USART1_SendData(&hh,1);
	//while(ReceiveState2 != 1);
	if((uart2_recv_buf[9] == 0x00) && (uart2_recv_buf[6] == 0x07))//表示特征合并成功
	{
		//模板存放成功
		//USART1_String("4:模板存放成功!\n");
		return 0;
	}		
	else if(uart2_recv_buf[9] == 0x01)
	{
		//收包错误
		//USART1_String("4:收包错误!\n");
		return 1;
	}
	else if(uart2_recv_buf[9] == 0x0B)
	{
		//页码超标
		//USART1_String("4:页码超标!\n");
		return 2;
	}
	else if(uart2_recv_buf[9] == 0x18)
	{
		//写 flash 出错
		//USART1_String("4:写 flash 出错!\n");
		return 3;
	}
	else
	{
		//其他
		//USART1_String("4:其他原因!\n");
		return 4;
	}
}

/*搜索整个指纹库，匹配*/
uint16_t Search_Fingerlib(uint8_t BufferID, uint16_t Page_start, uint16_t Page_num)
{
	uint16_t page = 0;   //存放指纹的页
	uint16_t score = 0;
  memset(uart2_recv_buf,0,sizeof(uart2_recv_buf));
	fcmd_length = Get_Pack_Length(0x01,FPM_Data,Get_Data_Length(6,SEARCH,BufferID,(Page_start>>8),Page_start,(Page_num>>8),Page_num)); 
	
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//延时不能太少
	
	//uint8_t hh = 0xa;
	//USART1_SendData(&hh,1);
	//USART1_SendData(UART_FPM_CMD,fcmd_length);
	//USART1_SendData(&hh,1);
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//延时不能太少
	
	//USART1_SendData(&hh,1);
	while(ReceiveState2 != 1);
	{
		ReceiveState2 = 0;
	//	USART1_SendData(uart2_recv_buf,rcvlen2);
	}
	//USART1_SendData(&hh,1);
	
	//while(ReceiveState2 != 1);
	if((uart2_recv_buf[9] == 0x00) && (uart2_recv_buf[6] == 0x07))//表示指纹搜索到
	{
		page = ((uart2_recv_buf[10] & 0x00FF) << 8) + (uart2_recv_buf[11]);
		score = ((uart2_recv_buf[12] & 0x00FF) << 8) + (uart2_recv_buf[13]);
		Person_num = page;
		//找到指纹
		//USART1_String("3:找到指纹!\n");
		return 0;
	}
	else if(uart2_recv_buf[9] == 0x01)
	{
		//收包有错
		//USART1_String("3:收包有错!\n");
		return 1;
	}
	else if(uart2_recv_buf[9] == 0x09)
	{
		//没有搜索到
		//USART1_String("3:没有搜索到!\n");
		return 2;
	}
	else
	{
		//其他问题
		//USART1_String("3:其他原因!\n");
		return 3;
	}
}

/* */
uint8_t Empty_The_FinLib(void)
{
  memset(uart2_recv_buf,0,sizeof(uart2_recv_buf));
	fcmd_length = Get_Pack_Length(0x01,FPM_Data,Get_Data_Length(1,EMPTY)); 
	
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//延时不能太少
	
	//uint8_t hh = 0xa;
	//USART1_SendData(&hh,1);
	//USART1_SendData(UART_FPM_CMD,fcmd_length);
	//USART1_SendData(&hh,1);
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//延时不能太少
	

	//USART1_SendData(&hh,1);
	while(ReceiveState2 != 1);
	{
		ReceiveState2 = 0;
		//USART1_SendData(uart2_recv_buf,rcvlen2);
	}
	//USART1_SendData(&hh,1);
	//while(ReceiveState2 != 1);
	if((uart2_recv_buf[9] == 0x00) && (uart2_recv_buf[6] == 0x07))//表示清空成功
	{
		//清空成功
		return 0;
	}
	else if(uart2_recv_buf[9] == 0x01)
	{
		//收包出错
		return 1;
	}
	else if(uart2_recv_buf[9] == 0x11)
	{
		//清空失败
		return 2;
	}
	else
	{
		//其他
		GPIOB->BRR = 0x04;
		return 3;
	}
}

/*******************************************************************************
*brif:将flash数据库中指定ID号的指纹模板读入到模板缓冲区CharBuffer1或CharBuffer2
*para:BufferID  缓冲区CharBuffer1、CharBuffer2的BufferID分别为0x01 和 0x02
*     PageID    指纹页ID
*return:0   success   else  failed
********************************************************************************/
uint8_t Read_Finger_Form_Flash(uint8_t BufferID,uint16_t PageID)
{
  memset(uart2_recv_buf,0,sizeof(uart2_recv_buf));
	fcmd_length = Get_Pack_Length(0x01,FPM_Data,Get_Data_Length(1,LOADCHAR,BufferID,(PageID>>8),PageID)); 

	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//延时不能太少
	
	
	//USART1_SendData(&hh,1);
	//USART1_SendData(UART_FPM_CMD,fcmd_length);
	//USART1_SendData(&hh,1);
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//延时不能太少
	
	
	//USART1_SendData(&hh,1);
	while(ReceiveState2 != 1);
	{
		ReceiveState2 = 0;
		//USART1_SendData(uart2_recv_buf,rcvlen2);
	}
	//USART1_SendData(&hh,1);

	//while(ReceiveState2 != 1);
	if((uart2_recv_buf[9] == 0x00) && (uart2_recv_buf[6] == 0x07))
	{
		//读取模板成功
		return 0;
	}
	else if(uart2_recv_buf[9] == 0x01)
	{
		//收到的数据包有错
		return 1;
	}
	else if(uart2_recv_buf[9] == 0x0c)
	{
		//读出有错，或者模板无效
		return 2;
	}
	else if(uart2_recv_buf[9] == 0x0B)
	{
		//pageID 超过范围
		return 3;
	}
	else 
	{
		//其他
		return 4;
	}
}

uint8_t Del_finger(uint8_t BufferID)
{
	memset(uart2_recv_buf,0,sizeof(uart2_recv_buf));
	fcmd_length = Get_Pack_Length(0x01,FPM_Data,Get_Data_Length(5,DELETCHAR,(BufferID>>8),BufferID,(1>>8),1)); 

	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//延时不能太少
	

	//USART1_SendData(&hh,1);
	//USART1_SendData(UART_FPM_CMD,fcmd_length);
	//USART1_SendData(&hh,1);
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//延时不能太少
	
	
	
	//USART1_SendData(&hh,1);
	while(ReceiveState2 != 1);
	{
		ReceiveState2 = 0;
		//USART1_SendData(uart2_recv_buf,rcvlen2);
	}
	//USART1_SendData(&hh,1);
	
	//while(ReceiveState2 != 1);
	if((uart2_recv_buf[9] == 0x00) && (uart2_recv_buf[6] == 0x07))
	{
		//删除成功
		return 0;
	}
	else if(uart2_recv_buf[9] == 0x01)
	{
		//收到的数据包有错
		return 1;
	}
	else if(uart2_recv_buf[9] == 0x0c)
	{
		//读出有错，或者模板无效
		return 2;
	}
	else if(uart2_recv_buf[9] == 0x0B)
	{
		//pageID 超过范围
		return 3;
	}
	else 
	{
		//其他
		return 4;
	}
}

extern __IO uint16_t NUM_Finger;
uint8_t Get_NUM(void)
{
	memset(uart2_recv_buf,0,sizeof(uart2_recv_buf));
	fcmd_length = Get_Pack_Length(0x01,FPM_Data,Get_Data_Length(2,TEMPLETENUM)); 

	//while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	//Delay(600);//延时不能太少
	
	
	//USART1_SendData(&hh,1);
	//USART1_SendData(UART_FPM_CMD,fcmd_length);
	//USART1_SendData(&hh,1);
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//延时不能太少

	//USART1_SendData(&hh,1);
	while(ReceiveState2 != 1);
	{
		ReceiveState2 = 0;
		//USART1_SendData(uart2_recv_buf,rcvlen2);
	}
	//USART1_SendData(&hh,1);
	
	//while(ReceiveState2 != 1);
	if((uart2_recv_buf[9] == 0x00) && (uart2_recv_buf[6] == 0x07))
	{
		NUM_Finger = ((uart2_recv_buf[10] & 0x00FF) << 8) + (uart2_recv_buf[11]);
		
		//读到模板个数
		return 0;
	}
	else if(uart2_recv_buf[9] == 0x01)
	{
		//收到的数据包有错
		return 1;
	}
	else 
	{
		//其他
		return 4;
	}
}