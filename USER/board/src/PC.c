#include "PC.h"
#include "stdarg.h"

/*���ڲ�ͬ���յ��Ĳ�ͬ��ָ��������ݣ���λ��ͬ�ı�־λ */
/*�ڳ������ѭ������ѯ�����в�ͬ����*********************/
PC_PACK sendpack;
uint8_t P_Data[4] = {0};
uint8_t UART_send_data[100];   //���ڴ��ָ����洮�ڷ���

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
	va_list valist; //���ڴ�Ų���
	va_start(valist, para_num);
	for(num=0; num < para_num; num++){
		P_Data[num] = va_arg(valist,int); //���ڶ�����������ȫ��
	}
	va_end(valist);
	return para_num;
}


uint8_t Pack_Data(uint8_t pid, uint8_t *data, uint8_t data_length)
{
	uint8_t i = 0, temp = 0;
	
	//��ͷ������˵����˭�İ�
	sendpack.Head = 0xaa;
	
	//����Ҫ���͵�������䣺 ����ͷ����Ϣ
	UART_send_data[0] = sendpack.Head;
	
	//��������
	sendpack.PID = pid;
	//����Ҫ���͵�������䣺 ��������
	UART_send_data[1] = sendpack.PID;
	
	//���ĳ�����Ϣ��
	sendpack.Length[0] = (data_length + 2) >> 8;
	sendpack.Length[1] = data_length + 2;
	//����Ҫ���͵�������䣺 �������ȣ����ڽ��շ�֪����λ�ȡ����
	UART_send_data[2] = sendpack.Length[0];
	UART_send_data[3] = sendpack.Length[1];
	
	//���а���������
	sendpack.Data = data;
	//�Դ���Ҫ�������ݵ���䣺���ʵ�ʵĶԷ���Ҫ����Ϣ
	for(i=0; i<data_length; i++)
	{
		temp += sendpack.Data[i];
		UART_send_data[4+i] = sendpack.Data[i];
	}
	//������������ѽ���͵����ݵ��ֽ���
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