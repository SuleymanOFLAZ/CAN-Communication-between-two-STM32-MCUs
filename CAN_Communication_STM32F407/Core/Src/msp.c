/*
 * msp.c
 *
 *  Created on: Jul 18, 2021
 *      Author: suley
 */
#include "stm32f4xx_hal.h"

void HAL_MspInit(void)
{
// We will do low level processor specific initializations
	// 1. Set up the priority grouping of the arm cortex mx processor
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	// 2. Enable the required sysrem exception of the arm cortex mx processor
	SCB->SHCSR |= (0x7 << 16); /* UsageFault, MemFault, BusFault exceptions are enabled */

	// 3. Configure the priority of the system exceptions.
	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef GPIOHandle;

// Here, we will do the low level initialization of the UART Peripheral
	// 1. Enable the clock for USART1 Peripheral
	__HAL_RCC_USART1_CLK_ENABLE();

	// 2. Do the pi muxing configurations
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIOHandle.Alternate = GPIO_AF7_USART1;
	GPIOHandle.Mode = GPIO_MODE_AF_PP;
	GPIOHandle.Pull = GPIO_PULLUP;
	GPIOHandle.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIOHandle.Pin = GPIO_PIN_6;	// PB6 - USART1 TX
	HAL_GPIO_Init(GPIOB, &GPIOHandle);

	GPIOHandle.Pin = GPIO_PIN_7;	// PB7 - USART1 RX
	HAL_GPIO_Init(GPIOB, &GPIOHandle);

	// 3. Enable the IRQ and set up the priority
	HAL_NVIC_EnableIRQ(USART1_IRQn);
	HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
}

void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan)
{
	// Enable CAN peripheral clock, GPIO clock
	__HAL_RCC_CAN1_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	// Enable IRQ and set priority
	HAL_NVIC_SetPriority(CAN1_TX_IRQn, 3, 0);
	HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 3, 0);
	HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 3, 0);
	HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 3, 0);

	HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
	HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
	HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
	HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);

	// Configure GPIO pins for CAN  /* PD0 -> CAN1_RX, PD1 ->CAN1_TX */  /* AF9 */
	GPIO_InitTypeDef GPIOHandle;
	GPIOHandle.Alternate = GPIO_AF9_CAN1;
	GPIOHandle.Mode = GPIO_MODE_AF_PP;
	GPIOHandle.Pull = GPIO_NOPULL;
	GPIOHandle.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIOHandle.Pin = GPIO_PIN_0;
	HAL_GPIO_Init(GPIOD, &GPIOHandle);

	GPIOHandle.Pin = GPIO_PIN_1;
	HAL_GPIO_Init(GPIOD, &GPIOHandle);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
	// 1. Enable peripheral clock
	__HAL_RCC_TIM6_CLK_ENABLE();

	// Configure the IRQ
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 6, 0);
	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);

	// GPIO
	__HAL_RCC_GPIOC_CLK_ENABLE();

	/*
	GPIO_InitTypeDef GPIOHandle;
	GPIOHandle.Alternate = 0;
	GPIOHandle.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOHandle.Pull = GPIO_NOPULL;
	GPIOHandle.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIOHandle.Pin = GPIO_PIN_1;
	HAL_GPIO_Init(GPIOC, &GPIOHandle);
	*/
}
