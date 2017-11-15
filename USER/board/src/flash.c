
#include  "flash.h"

uint32_t Data_Start = (uint32_t)0x08032000;   //划出 200k 的 FLASH 空间存储数据
uint32_t Data_End = (uint32_t)0x08064000;
uint32_t Data_Temp = (uint32_t)0x08032000;

static FLASH_Status PutData_into_Flash(uint32_t addr,uint16_t *buf,uint32_t len)
{
	uint32_t i;  
    uint16_t TempData16;//十六位临时数据变量
    FLASH_Status status = FLASH_COMPLETE;  
    if(len%2==0)     // 数据长度的奇偶判断
    {  
        for(i=0;i<len;)                      
        {                 
  
            TempData16=(uint16_t)buf[i];     //人为的将数据转成小端模式，便于数的读取 
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
            TempData16=(uint16_t)buf[i];     //人为的将数据转成小端模式
            TempData16&=buf[i];    
            if(i+2<len)                 //长度为奇数的时候，高位补00
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
    uint32_t staAddr;       //要写入扇区的起始地址        
    uint32_t count;        //对于扇区内的分组进行计数 
    uint16_t  buf1[PageSize]={0};    
    uint32_t i,j; 
		
    FLASH_Unlock();  
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);  
  
  
    staAddr=tmpaddr-(tmpaddr%PageSize);      
  
    count=(tmpaddr%PageSize)/Flash_DataSize;        
		//GPIOB->BSRR = 0x04;
    /*读出*/  
		
    Read_From_Flash(staAddr,buf1,PageSize);                          //从FLASH中读出数据 
    /*更改数据*/  
		
    for(i=count*Flash_DataSize,j=0;i<count*Flash_DataSize+Flash_DataSize;i++,j++)  
    {                                 
        buf1[i]=romdata[j];               
    }  
  
    /*擦除*/  
		
    status=FLASH_ErasePage(staAddr);  
  
  
    /*写入*/  
    status=PutData_into_Flash(staAddr,buf1,PageSize);  
  
    Data_Temp=tmpaddr+Flash_DataSize;                          //为下次写入更新地址  
    if(Data_Temp>=Data_End)      // Data_Start，Data_End即为定义的全局变量，
		{  
        Data_Temp=Data_Start;        
    }  
    FLASH_Lock();  
    return  status; 
}

