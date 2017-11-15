/**
  ******************************************************************************
  * @file    Project/STM32F0xx_StdPeriph_Templates/stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.5.0
  * @date    05-December-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
#include "gpio.h"
#include "uart.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 
extern uint8_t KEY2_FLAG;
extern uint8_t KEY3_FLAG;
extern uint8_t del_all;
void EXTI0_1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_1_IRQn 0 */

  /* USER CODE END EXTI0_1_IRQn 0 */
  //KEY2_FLAG = 1;
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
   {
		 KEY3_FLAG = 1;
		 del_all = 1;
     /* Clear the EXTI line 0 pending bit */
     EXTI_ClearITPendingBit(EXTI_Line0);
   }
	 if(EXTI_GetITStatus(EXTI_Line1) != RESET)
   {
		 KEY2_FLAG = 1;
     /* Clear the EXTI line 0 pending bit */
     EXTI_ClearITPendingBit(EXTI_Line1);
   }
	//EXTI_ClearITPendingBit(EXTI_Line0);
  /* USER CODE BEGIN EXTI0_1_IRQn 1 */

  /* USER CODE END EXTI0_1_IRQn 1 */
}
extern uint8_t aRxBuffer[128];
extern uint8_t  rcvlen;
extern __IO uint8_t RxCounter, ReceiveState;
extern __IO uint8_t person; 
void USART1_IRQHandler(void)
{
	 if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
   {
		 //清除挂起的中断标志位
		 USART_ClearITPendingBit(USART1,USART_IT_RXNE);
     /* Read one byte from the receive data register */
     aRxBuffer[RxCounter++] = USART_ReceiveData(USART1);
		 rcvlen = RxCounter; 
		 /*if(aRxBuffer[0] == 0){
			 RxCounter = 0;
		 }*/
   }
	 if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
   {
		 USART_ClearITPendingBit(USART1,USART_IT_IDLE);
     ReceiveState = 1;
     RxCounter = 0;
   }
}

extern uint8_t aRxBuffer2[BUFFER_SIZE];
extern __IO uint8_t RxCounter2, ReceiveState2;
extern uint8_t  rcvlen2;
extern uint8_t uart2_recv_buf[128];

void USART2_IRQHandler(void)
{
	 if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
   {
		 //清除挂起的中断标志位
		 USART_ClearITPendingBit(USART2,USART_IT_RXNE);
     /* Read one byte from the receive data register */
     uart2_recv_buf[RxCounter2++] = USART_ReceiveData(USART2);
		 rcvlen2 = RxCounter2; 
		 /*if(uart2_recv_buf[0] == 0){
			 RxCounter = 0;
		 }*/
   }
	 if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
   {
		 USART_ClearITPendingBit(USART2,USART_IT_IDLE);
     ReceiveState2 = 1;
     RxCounter2 = 0;
   }
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
