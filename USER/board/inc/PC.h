#ifndef __PC_H__
#define __PC_H__

#include "uart.h"

#define DataType_recv  0xaa;   //�ж������Ƿ�������λ������
#define DataType_Send  0xbb;   //�ϴ���PC������ʹ�������ͷ

#define PID_ACK  0x07;   //Ӧ���
#define PID_DATA 0x02;   //���ݰ�
#define PID_CMD  0x01;   //�����

#define CMD_ADD  0x01;   //���ָ��
#define CMD_DEL  0x02;   //ɾ��ָ��

#define MATCH_OK 0x00;   //ָ��ƥ��ɹ�
#define MATCH_ERR 0x01;  //ָ��ƥ��ʧ��


typedef struct pc_message_pack{
	uint8_t Head;						//�����ͷ
	uint8_t PID;						//����ʶ   0x01(cmd_package),0x02(data_package),0x07(ack_package),0x08(end_package)
	uint8_t Length[2];			//������ = DATA_LENGTH + 2
	uint8_t *Data;          //������
}PC_PACK;

void Send_Match_OK(uint8_t num);
uint8_t Data_Length(uint8_t para_num, ...);
uint8_t Send_PACK(uint8_t *pack, uint8_t num);
void ASCK_DEl(uint8_t asck);
void ASCK_ADD(uint8_t asck,uint8_t num);


#endif
