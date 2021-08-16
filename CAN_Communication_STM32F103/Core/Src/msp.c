#include "stm32f1xx_hal.h"

void HAL_MspInit(void)
{
	/* Configure priority grouping */
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	/* Enable required system exceptions */
	SCB->SHCSR |= (7 << 16); // UsageFault, MemFault, BusFault enabled

	  __HAL_RCC_AFIO_CLK_ENABLE();
	  __HAL_RCC_PWR_CLK_ENABLE();
	  // __HAL_AFIO_REMAP_SWJ_DISABLE();
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef GPIO_Cnfg;

	// Enable related peripheral clocks
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART2_CLK_ENABLE();

	// Enanle NVIC
	HAL_NVIC_SetPriority(USART2_IRQn, 2, 0);
	HAL_NVIC_EnableIRQ(USART2_IRQn);

	// Configure GPIOs
	GPIO_Cnfg.Mode = GPIO_MODE_AF_PP;
	GPIO_Cnfg.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_Cnfg.Pull = GPIO_NOPULL;
	GPIO_Cnfg.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOA, &GPIO_Cnfg);
}

void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan)
{
	// Enable CAN peripheral clock, GPIO clock
	__HAL_RCC_CAN1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	// Enable IRQ and set priority
	HAL_NVIC_SetPriority(CAN1_RX1_IRQn, 1, 0);
	HAL_NVIC_SetPriority(CAN1_SCE_IRQn, 1, 0);
	HAL_NVIC_SetPriority(USB_HP_CAN1_TX_IRQn, 1, 0);
	HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 1, 0);

	HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
	HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
	HAL_NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
	HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);

	// Configure GPIO pins for CAN  /* PA11 -> CAN1_RX, PA12 ->CAN1_TX */
	GPIO_InitTypeDef GPIOHandle = {0};

	GPIOHandle.Pin = GPIO_PIN_11;
	GPIOHandle.Mode = GPIO_MODE_INPUT;
	GPIOHandle.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIOHandle);

    GPIOHandle.Pin = GPIO_PIN_12;
    GPIOHandle.Mode = GPIO_MODE_AF_PP;
    GPIOHandle.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIOHandle);

}
