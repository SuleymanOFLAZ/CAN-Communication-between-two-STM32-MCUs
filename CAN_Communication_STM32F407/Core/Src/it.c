/*
 * it.c
 *
 *  Created on: Jul 18, 2021
 *      Author: suley
 */
#include "stm32f4xx_hal.h"

extern UART_HandleTypeDef Usart1Handle;
extern CAN_HandleTypeDef CAN1Handle;
extern CAN_HandleTypeDef CAN2Handle;
extern TIM_HandleTypeDef TIM6Handle;


void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&Usart1Handle);
}

void CAN1_TX_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&CAN1Handle);
}
void CAN1_RX0_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&CAN1Handle);
}
void CAN1_RX1_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&CAN1Handle);
}
void CAN1_SCE_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&CAN1Handle);
}

void CAN2_TX_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&CAN2Handle);
}
void CAN2_RX0_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&CAN2Handle);
}
void CAN2_RX1_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&CAN2Handle);
}
void CAN2_SCE_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&CAN2Handle);
}

void TIM6_DAC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&TIM6Handle);
}
