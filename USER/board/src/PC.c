#include "PC.h"
#include "stdarg.h"

/*对于不同的收到的不同的指令或者数据，置位不同的标志位 */
/*在程序的主循环中轮询，进行不同操作*********************/
PC_PACK sendpack;
uint8_t P_Data[4] = {0};
uint8_t UART_send_data[100];   //用于存放指令，方面串口发送

void Send_Match_OK(uint8_t num)
{
	UART_send_data[0] = 0xaa;
	UART_send_data[1] = PID_DATA;
	UART_send_data[2] = 0x0;
	UART_send_data[3] = 0x4;
	UART_send_data[4] = 0x00;
	UART_send_data[5] = num;
	
	USART1_SendData(UART_send_data,6);
}

uint8_t Data_Length(uint8_t para_num, ...)
{
	uint8_t num = 0;
	va_list valist; //用于存放参数
	va_start(valist, para_num);
	for(num=0; num < para_num; num++){
		P_Data[num] = va_arg(valist,int); //将第二个参数存入全局
	}
	va_end(valist);
	return para_num;
}


uint8_t Pack_Data(uint8_t pid, uint8_t *data, uint8_t data_length)
{
	uint8_t i = 0, temp = 0;
	
	//包头，用来说明是谁的包
	sendpack.Head = 0xaa;
	
	//串口要发送的数据填充： 填充包头的信息
	UART_send_data[0] = sendpack.Head;
	
	//包的类型
	sendpack.PID = pid;
	//串口要发送的数据填充： 填充包类型
	UART_send_data[1] = sendpack.PID;
	
	//包的长度信息：
	sendpack.Length[0] = (data_length + 2) >> 8;
	sendpack.Length[1] = data_length + 2;
	//串口要发送的数据填充： 填充包长度，便于接收方知道如何获取数据
	UART_send_data[2] = sendpack.Length[0];
	UART_send_data[3] = sendpack.Length[1];
	
	//包中包含的数据
	sendpack.Data = data;
	//对串口要发送数据的填充：填充实际的对方需要的信息
	for(i=0; i<data_length; i++)
	{
		temp += sendpack.Data[i];
		UART_send_data[4+i] = sendpack.Data[i];
	}
	//返回整个串口呀发送的数据的字节数
	return 4+data_length;
}

uint8_t Send_PACK(uint8_t *pack, uint8_t num)
{
	USART1_SendData(pack,num);
	Delay(200);
	return 0;
}

void ASCK_DEl(uint8_t asck)
{
	uint8_t pack_lenth = Pack_Data(0x07,P_Data,Data_Length(1,asck));
	Send_PACK(UART_send_data, pack_lenth);
}

void ASCK_ADD(uint8_t asck,uint8_t num)
{
	uint8_t pack_lenth = Pack_Data(0x07,P_Data,Data_Length(2,asck,num));
	Send_PACK(UART_send_data, pack_lenth);
}