#include "stm32f1xx_hal.h"

extern CAN_HandleTypeDef CAN_Cnfg;

void SysTick_Handler(void)
{
	HAL_IncTick();
	//HAL_SYSTICK_IRQHandler();
}

void USB_HP_CAN1_TX_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&CAN_Cnfg);
}
void USB_LP_CAN1_RX0_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&CAN_Cnfg);
}
void CAN1_RX1_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&CAN_Cnfg);
}
void CAN1_SCE_IRQHandler(void)
{
	HAL_CAN_IRQHandler(&CAN_Cnfg);
}
