#ifndef __FLASH_H__
#define __FLASH_H__

#include "stm32f0xx.h"
#include "stdint.h"


#define  Flash_DataSize  1024

static FLASH_Status PutData_into_Flash(uint32_t addr,uint16_t *buf,uint32_t len);
void Read_From_Flash(uint32_t addr,uint16_t *buf,uint32_t  len);
FLASH_Status Write_to_Flash(uint32_t tmpaddr,uint16_t *romdata,uint32_t len,uint32_t dataStaAddr,uint32_t dataEndAddr);


#endif