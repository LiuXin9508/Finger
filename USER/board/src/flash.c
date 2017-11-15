
#include  "flash.h"

uint32_t Data_Start = (uint32_t)0x08032000;   //���� 200k �� FLASH �ռ�洢����
uint32_t Data_End = (uint32_t)0x08064000;
uint32_t Data_Temp = (uint32_t)0x08032000;

static FLASH_Status PutData_into_Flash(uint32_t addr,uint16_t *buf,uint32_t len)
{
	uint32_t i;  
    uint16_t TempData16;//ʮ��λ��ʱ���ݱ���
    FLASH_Status status = FLASH_COMPLETE;  
    if(len%2==0)     // ���ݳ��ȵ���ż�ж�
    {  
        for(i=0;i<len;)                      
        {                 
  
            TempData16=(uint16_t)buf[i];     //��Ϊ�Ľ�����ת��С��ģʽ���������Ķ�ȡ 
            TempData16= TempData16|0xFF00;  
            TempData16= ((((uint16_t)buf[i+1])<<8)|0x00FF)&TempData16;   
  
            if(status == FLASH_COMPLETE)  
            {              
                status=FLASH_ProgramHalfWord(addr,TempData16);  
                addr+=2;                   
            }  
            else   
            {                  
                break;             
            }     
            i=i+2;        
        }     
    }  
    else  
    {             
        for(i=0;i<len;)                      
        {  
            TempData16=(uint16_t)buf[i];     //��Ϊ�Ľ�����ת��С��ģʽ
            TempData16&=buf[i];    
            if(i+2<len)                 //����Ϊ������ʱ�򣬸�λ��00
            {  
                TempData16= TempData16|0xFF00;  
                TempData16= ((((uint16_t)buf[i+1])<<8)|0x00FF)&TempData16;              
  
            }  
            else  
                TempData16&=0x00FF;                 
  
            if(status == FLASH_COMPLETE)  
            {              
                status=FLASH_ProgramHalfWord(addr,TempData16);  
                addr+=2;                   
            }  
            else   
            {                  
                break;             
            }     
            i=i+2;        
        }     
    }  
    return  status; 
}


void Read_From_Flash(uint32_t addr,uint16_t *buf,uint32_t  len)
{
	uint32_t address=addr;               
  while(len--)  
  {  
    *buf=*(uint16_t*)address;  
    buf++;  
    address+=2;		
  } 
	GPIOB->BSRR = 0x04;			
  return;
}


#define PageSize  1024  

FLASH_Status Write_to_Flash(uint32_t tmpaddr,uint16_t *romdata,uint32_t len,uint32_t dataStaAddr,uint32_t dataEndAddr)
{
		FLASH_Status  status;  
    uint32_t staAddr;       //Ҫд����������ʼ��ַ        
    uint32_t count;        //���������ڵķ�����м��� 
    uint16_t  buf1[PageSize]={0};    
    uint32_t i,j; 
		
    FLASH_Unlock();  
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);  
  
  
    staAddr=tmpaddr-(tmpaddr%PageSize);      
  
    count=(tmpaddr%PageSize)/Flash_DataSize;        
		//GPIOB->BSRR = 0x04;
    /*����*/  
		
    Read_From_Flash(staAddr,buf1,PageSize);                          //��FLASH�ж������� 
    /*��������*/  
		
    for(i=count*Flash_DataSize,j=0;i<count*Flash_DataSize+Flash_DataSize;i++,j++)  
    {                                 
        buf1[i]=romdata[j];               
    }  
  
    /*����*/  
		
    status=FLASH_ErasePage(staAddr);  
  
  
    /*д��*/  
    status=PutData_into_Flash(staAddr,buf1,PageSize);  
  
    Data_Temp=tmpaddr+Flash_DataSize;                          //Ϊ�´�д����µ�ַ  
    if(Data_Temp>=Data_End)      // Data_Start��Data_End��Ϊ�����ȫ�ֱ�����
		{  
        Data_Temp=Data_Start;        
    }  
    FLASH_Lock();  
    return  status; 
}

