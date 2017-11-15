/*  ���ļ���Ҫ��װ��ָ��ʶ��ģ��Ĳ�������  */

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

_SYS_CMD_PACK FPM_CMD;       //ϵͳָ��ʵ��
uint8_t FPM_Data[128] = {0}; //���ڴ��ָ���������ֵ
uint8_t UART_FPM_CMD[256];   //���ڴ��ָ����洮�ڷ���
uint8_t fcmd_length;
uint16_t Person_num = 0;     //���ڱ��ָ��


/*=======================================================*/
/*         ���º�����   ������   ��ʾ������             */
/*=======================================================*/
//ʹ�ü򵥵���ʱ���������Ʒ���������ʱ��
void Delay(int time) 
{
	int i,j;
	for(i=0;i<time*10;i++)
	{
		for(j=0;j<100;j++);
	}
}

void TIP_SUCCESS(void) //�ɹ�ʶ��ָ��  ����1��
{
	GPIOA->BRR = GPIO_Pin_0;
	Delay(250);
	GPIOA->BSRR = GPIO_Pin_0;
}

void TIP_FAILED(void) //ָ��ʶ��ʧ��  ����1��
{
	GPIOA->BRR = GPIO_Pin_0;
	Delay(500);
	GPIOA->BSRR = GPIO_Pin_0;
}

void TIP_WRITE_SUCCESS(void) //ָ��¼��ɹ� ���� 2��
{
	GPIOA->BRR = GPIO_Pin_0;
	Delay(250);
	GPIOA->BSRR = GPIO_Pin_0;
	Delay(1000);
	GPIOA->BRR = GPIO_Pin_0;
	Delay(250);
	GPIOA->BSRR = GPIO_Pin_0;
}

void TIP_WRITE_FAILED(void) //ָ��¼��ʧ��   ����1�� ����1��
{
	GPIOA->BRR = GPIO_Pin_0;
	Delay(250);
	GPIOA->BSRR = GPIO_Pin_0;
	Delay(1000);
	GPIOA->BRR = GPIO_Pin_0;
	Delay(500);
	GPIOA->BSRR = GPIO_Pin_0;
}

void TIP_DELATE_ALL(void)  //ɾ������ָ��ģ�� ����1��  ����1��
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
/*                ָ��ģ��Ĳ�������                       */
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

//ʹ�� USART2 ��ָ��ģ�鷢��������ݰ���
uint8_t  Send_Command(uint8_t *command, uint8_t num)
{
	USART2_SendData(command,num);
	Delay(600);
	return 0;
}

//��ȡҪ���͵İ����ȣ���С��
//ͬʱ�����ݷ��
uint8_t Get_Pack_Length(uint8_t pid, uint8_t *data, uint8_t data_length)
{
	uint8_t i = 0, temp = 0;
	
	//��ͷ������˵����˭�İ�
	FPM_CMD.Head[0] = 0xEF;
	FPM_CMD.Head[1] = 0x01;
	//����Ҫ���͵�������䣺 ����ͷ����Ϣ
	UART_FPM_CMD[0] = FPM_CMD.Head[0];
	UART_FPM_CMD[1] = FPM_CMD.Head[1];
	
	//��ַ������ģ��ĵ�ַ
	FPM_CMD.Addr[0] = 0xFF;
	FPM_CMD.Addr[1] = 0xFF;
	FPM_CMD.Addr[2] = 0xFF;
	FPM_CMD.Addr[3] = 0xFF;
	//����Ҫ���͵�������䣺 ����ַ��Ϣ
	UART_FPM_CMD[2] = FPM_CMD.Addr[0];
	UART_FPM_CMD[3] = FPM_CMD.Addr[1];
	UART_FPM_CMD[4] = FPM_CMD.Addr[2];
	UART_FPM_CMD[5] = FPM_CMD.Addr[3];
	
	//��������
	FPM_CMD.PID = pid;
	//����Ҫ���͵�������䣺 ��������
	UART_FPM_CMD[6] = FPM_CMD.PID;
	
	//���ĳ�����Ϣ��
	FPM_CMD.Length[0] = (data_length + 2) >> 8;
	FPM_CMD.Length[1] = data_length + 2;
	//����Ҫ���͵�������䣺 �������ȣ����ڽ��շ�֪����λ�ȡ����
	UART_FPM_CMD[7] = FPM_CMD.Length[0];
	UART_FPM_CMD[8] = FPM_CMD.Length[1];
	
	//���а���������
	FPM_CMD.Data = data;
	//�Դ���Ҫ�������ݵ���䣺���ʵ�ʵĶԷ���Ҫ����Ϣ
	for(i=0; i<data_length; i++)
	{
		temp += FPM_CMD.Data[i];
		UART_FPM_CMD[9+i] = FPM_CMD.Data[i];
	}
	
	//�˴��ڽ������ݺ�У�飻���ڽ��ܷ�ͨ���˶������жϽ����Ƿ����
	FPM_CMD.SUM[0] = (pid + temp + data_length + 2) >> 8;
	FPM_CMD.SUM[1] = (pid + temp + data_length + 2);
	//����Ҫ�������ݵ���䣺������ݺ�У����
	UART_FPM_CMD[9+data_length] = FPM_CMD.SUM[0];
	UART_FPM_CMD[10+data_length] = FPM_CMD.SUM[1];
	
	//������������ѽ���͵����ݵ��ֽ���
	return 11+data_length;
}



//��������Ҫ������װ����
/*�ⲿ����Ҫ���ָ��Լ�ָ����Ҫ��һЩ���Ӳ��� */
/* �ɱ����  ...    ���ñ�׼C �� stdarg.h �󼴿�ʹ�� */
uint8_t Get_Data_Length(uint8_t para_num, ...)
{
	uint8_t num = 0;
	va_list valist; //���ڴ�Ų���
	va_start(valist, para_num);
	for(num=0; num < para_num; num++){
		FPM_Data[num] = va_arg(valist,int); //���ڶ�����������ȫ��
	}
	va_end(valist);
	return para_num;
}


/*��ȡָ����Ϣ�����ұ��� image ���ڲ��� buffer �� */
uint8_t Finger_Search_AND_Save_Image(void)
{
	//������� USART2 �Ľ������ݻ�����
	/*   ע�⣺ �����ڲ���buffer�Ĵ��� �����Լ������һ���ڴ�����  */
	memset(uart2_recv_buf,0,sizeof(uart2_recv_buf));
	
	//�������ݵķ���������Է��͸�ָ��ģ������ݷ����ͬʱ���� USART Ҫ���͵��ֽ���
	fcmd_length = Get_Pack_Length(0x01,FPM_Data,Get_Data_Length(1,GENIMG)); 
	
	//ʹ�� USART ��������
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//��ʱ����̫��
	//uint8_t hh = 0xa;
	//USART1_SendData(&hh,1);
	//USART1_SendData(UART_FPM_CMD,fcmd_length);
	//USART1_SendData(&hh,1);
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//��ʱ����̫��
	
	
	//USART1_SendData(&hh,1);
	while(ReceiveState2 != 1);
	{
		ReceiveState2 = 0;
		//USART1_SendData(uart2_recv_buf,rcvlen2);
	}
	//USART1_SendData(&hh,1);
	//while(ReceiveState2 != 1);
	if((uart2_recv_buf[9] == 0x00) && (uart2_recv_buf[6] == 0x07)) //��ʾָ��¼��ɹ�
	{
		//ָ��̽��ɹ�
		//USART1_String("1:ָ��̽��ɹ�!\n");
		return 0;
	}
	else if(uart2_recv_buf[9] == 0x01)
	{
		//�յ������ݰ��д�
		//USART1_String("1:�յ������ݰ��д�!\n");
		return 1;
	}
	else if(uart2_recv_buf[9] == 0x02)
	{
		//û�м�⵽��ָ
		//USART1_String("1:û�м�⵽��ָ!\n");
		return 2;
	}
	else if(uart2_recv_buf[9] == 0x03)
	{
		//��ָ¼�벻�ɹ�
		//USART1_String("1:��ָ¼�벻�ɹ�!\n");
		return 3;
	}
	else
	{
		return 4;
	}
}

/*��ȡָ��ͼ���������Ϣ�����浽 charbuffer1 ���� charbuffer2 */
uint8_t Finger_Feature_AND_Save_charbuffer1or2(uint8_t area_num)
{
	memset(uart2_recv_buf,0,sizeof(uart2_recv_buf));
	
	fcmd_length = Get_Pack_Length(0x01,FPM_Data,Get_Data_Length(2,IMG2TZ,area_num));
	
	//USART1_SendData(&hh,1);
	//USART1_SendData(UART_FPM_CMD,fcmd_length);
	//USART1_SendData(&hh,1);
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//��ʱ����̫��
	
	//USART1_SendData(&hh,1);
	while(ReceiveState2 != 1);
	{
		ReceiveState2 = 0;
		//USART1_SendData(uart2_recv_buf,rcvlen2);
	}
	//USART1_SendData(&hh,1);
	//while(ReceiveState2 != 1);
	if((uart2_recv_buf[9] == 0x00) && (uart2_recv_buf[6] == 0x07))//��ʾ���������ɹ�
	{
		//���������ɹ�
		//USART1_String("2:���������ɹ�!\n");
		return 0;		
	}
	else if(uart2_recv_buf[9] == 0x01)
	{
		//�յ������ݰ��д�
		//USART1_String("2:�յ������ݰ��д�!\n");
		return 1;
	}
	else if(uart2_recv_buf[9] == 0x06)
	{
		//������������
		//USART1_String("2:������������!\n");
		return 2;
	}
	else if(uart2_recv_buf[9] == 0x07)
	{
		//������̫�ٲ�����������
		//USART1_String("2:������̫�ٲ�����������!\n");
		return 3;		
	}
	else if(uart2_recv_buf[9] == 0x15)
	{
		//ͼ�񻺳���û��ͼ������������
		//USART1_String("2:ͼ�񻺳���û��ͼ������������!\n");
		return 4;		
	}
	else
	{
		//����ԭ��
		//USART1_String("2:����ԭ��!\n");
		return 5;
	}
}

/*�ϲ����� charbuffer �е������� ��Ϊһ��ָ��ģ�� */
uint8_t Combine_Finger_Feature(void)
{
  memset(uart2_recv_buf,0,sizeof(uart2_recv_buf));
	fcmd_length = Get_Pack_Length(0x01,FPM_Data,Get_Data_Length(1,REGMODEL)); 

	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//��ʱ����̫��
	
	//uint8_t hh = 0xa;
	//USART1_SendData(&hh,1);
	//USART1_SendData(UART_FPM_CMD,fcmd_length);
	//USART1_SendData(&hh,1);
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//��ʱ����̫��
	
	//USART1_SendData(&hh,1);
	while(ReceiveState2 != 1);
	{
		ReceiveState2 = 0;
		//USART1_SendData(uart2_recv_buf,rcvlen2);
	}
	//USART1_SendData(&hh,1);
	//while(ReceiveState2 != 1);
	if((uart2_recv_buf[9] == 0x00) && (uart2_recv_buf[6] == 0x07))//��ʾ�����ϲ��ɹ�
	{
		//�ϲ��ɹ�
		//USART1_String("33:�ϲ��ɹ�!\n");
		return 0;				
	}
	else if(uart2_recv_buf[9] == 0x01)
	{
		//�����հ��д�
		//USART1_String("33:�����հ��д�!\n");
		return 1;
	}
	else if(uart2_recv_buf[9] == 0x0a)
	{
		//����ģ�岻����һ����
		//USART1_String("33:����ģ�岻����һ����!\n");
		return 2;
	}
	else
	{
		//USART1_String("33:����ԭ��!\n");
		return 3;
	}
}

/*��ָ��ģ���ŵ��ڲ��� flash �� */
uint8_t Save_Finger_To_Flash(uint8_t BufferID, uint16_t PageID)
{
	memset(uart2_recv_buf,0,sizeof(uart2_recv_buf));
	fcmd_length = Get_Pack_Length(0x01,FPM_Data,Get_Data_Length(4,STORE,BufferID,(PageID>>8),PageID)); 

	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//��ʱ����̫��
	
	//uint8_t hh = 0xa;
	//USART1_SendData(&hh,1);
	//USART1_SendData(UART_FPM_CMD,fcmd_length);
	//USART1_SendData(&hh,1);
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//��ʱ����̫��
	
	//USART1_SendData(&hh,1);
	while(ReceiveState2 != 1);
	{
		ReceiveState2 = 0;
		//USART1_SendData(uart2_recv_buf,rcvlen2);
	}
	//USART1_SendData(&hh,1);
	//while(ReceiveState2 != 1);
	if((uart2_recv_buf[9] == 0x00) && (uart2_recv_buf[6] == 0x07))//��ʾ�����ϲ��ɹ�
	{
		//ģ���ųɹ�
		//USART1_String("4:ģ���ųɹ�!\n");
		return 0;
	}		
	else if(uart2_recv_buf[9] == 0x01)
	{
		//�հ�����
		//USART1_String("4:�հ�����!\n");
		return 1;
	}
	else if(uart2_recv_buf[9] == 0x0B)
	{
		//ҳ�볬��
		//USART1_String("4:ҳ�볬��!\n");
		return 2;
	}
	else if(uart2_recv_buf[9] == 0x18)
	{
		//д flash ����
		//USART1_String("4:д flash ����!\n");
		return 3;
	}
	else
	{
		//����
		//USART1_String("4:����ԭ��!\n");
		return 4;
	}
}

/*��������ָ�ƿ⣬ƥ��*/
uint16_t Search_Fingerlib(uint8_t BufferID, uint16_t Page_start, uint16_t Page_num)
{
	uint16_t page = 0;   //���ָ�Ƶ�ҳ
	uint16_t score = 0;
  memset(uart2_recv_buf,0,sizeof(uart2_recv_buf));
	fcmd_length = Get_Pack_Length(0x01,FPM_Data,Get_Data_Length(6,SEARCH,BufferID,(Page_start>>8),Page_start,(Page_num>>8),Page_num)); 
	
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//��ʱ����̫��
	
	//uint8_t hh = 0xa;
	//USART1_SendData(&hh,1);
	//USART1_SendData(UART_FPM_CMD,fcmd_length);
	//USART1_SendData(&hh,1);
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//��ʱ����̫��
	
	//USART1_SendData(&hh,1);
	while(ReceiveState2 != 1);
	{
		ReceiveState2 = 0;
	//	USART1_SendData(uart2_recv_buf,rcvlen2);
	}
	//USART1_SendData(&hh,1);
	
	//while(ReceiveState2 != 1);
	if((uart2_recv_buf[9] == 0x00) && (uart2_recv_buf[6] == 0x07))//��ʾָ��������
	{
		page = ((uart2_recv_buf[10] & 0x00FF) << 8) + (uart2_recv_buf[11]);
		score = ((uart2_recv_buf[12] & 0x00FF) << 8) + (uart2_recv_buf[13]);
		Person_num = page;
		//�ҵ�ָ��
		//USART1_String("3:�ҵ�ָ��!\n");
		return 0;
	}
	else if(uart2_recv_buf[9] == 0x01)
	{
		//�հ��д�
		//USART1_String("3:�հ��д�!\n");
		return 1;
	}
	else if(uart2_recv_buf[9] == 0x09)
	{
		//û��������
		//USART1_String("3:û��������!\n");
		return 2;
	}
	else
	{
		//��������
		//USART1_String("3:����ԭ��!\n");
		return 3;
	}
}

/* */
uint8_t Empty_The_FinLib(void)
{
  memset(uart2_recv_buf,0,sizeof(uart2_recv_buf));
	fcmd_length = Get_Pack_Length(0x01,FPM_Data,Get_Data_Length(1,EMPTY)); 
	
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//��ʱ����̫��
	
	//uint8_t hh = 0xa;
	//USART1_SendData(&hh,1);
	//USART1_SendData(UART_FPM_CMD,fcmd_length);
	//USART1_SendData(&hh,1);
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//��ʱ����̫��
	

	//USART1_SendData(&hh,1);
	while(ReceiveState2 != 1);
	{
		ReceiveState2 = 0;
		//USART1_SendData(uart2_recv_buf,rcvlen2);
	}
	//USART1_SendData(&hh,1);
	//while(ReceiveState2 != 1);
	if((uart2_recv_buf[9] == 0x00) && (uart2_recv_buf[6] == 0x07))//��ʾ��ճɹ�
	{
		//��ճɹ�
		return 0;
	}
	else if(uart2_recv_buf[9] == 0x01)
	{
		//�հ�����
		return 1;
	}
	else if(uart2_recv_buf[9] == 0x11)
	{
		//���ʧ��
		return 2;
	}
	else
	{
		//����
		GPIOB->BRR = 0x04;
		return 3;
	}
}

/*******************************************************************************
*brif:��flash���ݿ���ָ��ID�ŵ�ָ��ģ����뵽ģ�建����CharBuffer1��CharBuffer2
*para:BufferID  ������CharBuffer1��CharBuffer2��BufferID�ֱ�Ϊ0x01 �� 0x02
*     PageID    ָ��ҳID
*return:0   success   else  failed
********************************************************************************/
uint8_t Read_Finger_Form_Flash(uint8_t BufferID,uint16_t PageID)
{
  memset(uart2_recv_buf,0,sizeof(uart2_recv_buf));
	fcmd_length = Get_Pack_Length(0x01,FPM_Data,Get_Data_Length(1,LOADCHAR,BufferID,(PageID>>8),PageID)); 

	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//��ʱ����̫��
	
	
	//USART1_SendData(&hh,1);
	//USART1_SendData(UART_FPM_CMD,fcmd_length);
	//USART1_SendData(&hh,1);
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//��ʱ����̫��
	
	
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
		//��ȡģ��ɹ�
		return 0;
	}
	else if(uart2_recv_buf[9] == 0x01)
	{
		//�յ������ݰ��д�
		return 1;
	}
	else if(uart2_recv_buf[9] == 0x0c)
	{
		//�����д�����ģ����Ч
		return 2;
	}
	else if(uart2_recv_buf[9] == 0x0B)
	{
		//pageID ������Χ
		return 3;
	}
	else 
	{
		//����
		return 4;
	}
}

uint8_t Del_finger(uint8_t BufferID)
{
	memset(uart2_recv_buf,0,sizeof(uart2_recv_buf));
	fcmd_length = Get_Pack_Length(0x01,FPM_Data,Get_Data_Length(5,DELETCHAR,(BufferID>>8),BufferID,(1>>8),1)); 

	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//��ʱ����̫��
	

	//USART1_SendData(&hh,1);
	//USART1_SendData(UART_FPM_CMD,fcmd_length);
	//USART1_SendData(&hh,1);
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//��ʱ����̫��
	
	
	
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
		//ɾ���ɹ�
		return 0;
	}
	else if(uart2_recv_buf[9] == 0x01)
	{
		//�յ������ݰ��д�
		return 1;
	}
	else if(uart2_recv_buf[9] == 0x0c)
	{
		//�����д�����ģ����Ч
		return 2;
	}
	else if(uart2_recv_buf[9] == 0x0B)
	{
		//pageID ������Χ
		return 3;
	}
	else 
	{
		//����
		return 4;
	}
}

extern __IO uint16_t NUM_Finger;
uint8_t Get_NUM(void)
{
	memset(uart2_recv_buf,0,sizeof(uart2_recv_buf));
	fcmd_length = Get_Pack_Length(0x01,FPM_Data,Get_Data_Length(2,TEMPLETENUM)); 

	//while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	//Delay(600);//��ʱ����̫��
	
	
	//USART1_SendData(&hh,1);
	//USART1_SendData(UART_FPM_CMD,fcmd_length);
	//USART1_SendData(&hh,1);
	while(Send_Command(UART_FPM_CMD,fcmd_length) != 0);
	Delay(600);//��ʱ����̫��

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
		
		//����ģ�����
		return 0;
	}
	else if(uart2_recv_buf[9] == 0x01)
	{
		//�յ������ݰ��д�
		return 1;
	}
	else 
	{
		//����
		return 4;
	}
}