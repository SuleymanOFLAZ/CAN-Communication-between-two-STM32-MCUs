/*
 * main.c
 *
 *  Created on: Jul 18, 2021
 *      Author: suley
 */


#include "stm32f4xx_hal.h"
#include "main.h"
#include <string.h>
#include <stdio.h>


void SystemClockConfig(void);
void Usart1_Handle(void);
void Error_Handler(void);
void Usart1_Config(void);
void CAN1_Init(void);
void CAN1_TX(void);
void CAN1_RX(void);
void CAN1_FilterConfig(void);
void TIM6_Init(void);
void Send_Event(void);
void Request_Event(void);
void CAN_Send(uint8_t led_no);
void CAN_Request(void);
void Print_Requested_message(uint8_t* msg);

UART_HandleTypeDef Usart1Handle;
CAN_HandleTypeDef CAN1Handle;
CAN_HandleTypeDef CAN2Handle;
TIM_HandleTypeDef TIM6Handle;

uint32_t tim6_glb_cnt = 0;

char msg[250];

int main(void)
{
	HAL_Init();
	SystemClockConfig();
	HAL_SYSTICK_Config(__HAL_RCC_GET_SYSCLK_SOURCE()/1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	Usart1_Config();

	CAN1_Init();

	CAN1_FilterConfig();

	if(HAL_CAN_ActivateNotification(&CAN1Handle, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_BUSOFF) != HAL_OK)
	{
		Error_Handler();
	}

	if(HAL_CAN_Start(&CAN1Handle) != HAL_OK)
	{
		Error_Handler();
	}

	TIM6_Init();

	// CAN1_TX();

	// CAN1_RX();

	while(1)
	{
	}

	return 0;
}

void SystemClockConfig(void)
{
	RCC_OscInitTypeDef OscInitStruct;
	memset(&OscInitStruct, 0 , sizeof(OscInitStruct));
	OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	OscInitStruct.HSEState = RCC_HSE_ON;
	OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	OscInitStruct.PLL.PLLM = 4;
	OscInitStruct.PLL.PLLN = 72;
	OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;

	if(HAL_RCC_OscConfig(&OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	RCC_ClkInitTypeDef ClkInitTypedef;

	ClkInitTypedef.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | \
			RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	ClkInitTypedef.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	ClkInitTypedef.APB1CLKDivider = RCC_HCLK_DIV2;
	ClkInitTypedef.AHBCLKDivider = RCC_SYSCLK_DIV1;
	ClkInitTypedef.APB2CLKDivider = RCC_HCLK_DIV2;

	if(HAL_RCC_ClockConfig(&ClkInitTypedef, FLASH_ACR_LATENCY_1WS) != HAL_OK)
	{
		Error_Handler();
	}

	__HAL_RCC_HSI_DISABLE();
}

void Usart1_Config(void)
{
	Usart1Handle.Instance = USART1;
	Usart1Handle.Init.BaudRate = 115200;
	Usart1Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	Usart1Handle.Init.Mode = UART_MODE_TX_RX;
	Usart1Handle.Init.OverSampling = UART_OVERSAMPLING_16 ;
	Usart1Handle.Init.Parity = UART_PARITY_NONE ;
	Usart1Handle.Init.StopBits = UART_STOPBITS_1 ;
	Usart1Handle.Init.WordLength = UART_WORDLENGTH_8B ;

	if(HAL_UART_Init(&Usart1Handle) != HAL_OK)
	{
		Error_Handler();
	}
}

void TIM6_Init(void)
{
	TIM6Handle.Instance = TIM6;
	TIM6Handle.Init.Prescaler = 6000 -1;
	// TIM6Handle.Init.AutoReloadPreload = 5000;
	TIM6Handle.Init.Period = 5000;

	if(HAL_TIM_Base_Init(&TIM6Handle) != HAL_OK)
	{
		Error_Handler();
	}

	if(HAL_TIM_Base_Start_IT(&TIM6Handle) != HAL_OK)
	{
		Error_Handler();
	}
}

void CAN1_Init(void)
{
	CAN1Handle.Instance = CAN1;
	CAN1Handle.Init.Prescaler = 2;
	CAN1Handle.Init.TimeSeg1 = CAN_BS1_15TQ;
	CAN1Handle.Init.TimeSeg2 = CAN_BS2_2TQ;
	CAN1Handle.Init.Mode = CAN_MODE_NORMAL;
	CAN1Handle.Init.SyncJumpWidth = CAN_SJW_1TQ;
	CAN1Handle.Init.TimeTriggeredMode = DISABLE;
	CAN1Handle.Init.AutoBusOff = DISABLE;
	CAN1Handle.Init.AutoWakeUp = DISABLE;
	CAN1Handle.Init.AutoRetransmission = ENABLE;
	CAN1Handle.Init.ReceiveFifoLocked = DISABLE;
	CAN1Handle.Init.TransmitFifoPriority =  DISABLE;

	if(HAL_CAN_Init(&CAN1Handle) != HAL_OK)
	{
		Error_Handler();
	}
}

void CAN1_FilterConfig(void)
{
	CAN_FilterTypeDef CanFilter;

	// Accept all frames
	CanFilter.FilterIdHigh = 0;
	CanFilter.FilterIdLow = 0;
	CanFilter.FilterMaskIdHigh = 0;
	CanFilter.FilterMaskIdLow = 0;
	CanFilter.FilterFIFOAssignment = CAN_RX_FIFO0;
	CanFilter.FilterBank = 0;
	CanFilter.FilterMode = CAN_FILTERMODE_IDMASK;
	CanFilter.FilterScale = CAN_FILTERSCALE_32BIT;
	CanFilter.FilterActivation = CAN_FILTER_ENABLE;
	CanFilter.SlaveStartFilterBank = 14;

	if(HAL_CAN_ConfigFilter(&CAN1Handle, &CanFilter) != HAL_OK)
	{
		Error_Handler();
	}
}

void CAN1_TX(void)
{
	CAN_TxHeaderTypeDef CAN1_tx;

	uint8_t message[5] = {'H', 'E', 'L', 'L', 'O'};

	uint32_t TxMaibox;

	CAN1_tx.StdId = 0x65D;
	CAN1_tx.ExtId = 0x1FFFFFFF;
	CAN1_tx.IDE = CAN_ID_STD;
	CAN1_tx.RTR = CAN_RTR_DATA;
	CAN1_tx.DLC = 5;
	CAN1_tx.TransmitGlobalTime = DISABLE;

	if(HAL_CAN_AddTxMessage(&CAN1Handle, &CAN1_tx, message, &TxMaibox) != HAL_OK)
	{
		Error_Handler();
	}
}

void CAN1_RX(void)
{
	CAN_RxHeaderTypeDef CAN1_rx;
	uint8_t received_msg[5];

	// Check that is FIFO not empty
	while(! HAL_CAN_GetRxFifoFillLevel(&CAN1Handle, CAN_RX_FIFO0));

	if(HAL_CAN_GetRxMessage(&CAN1Handle, CAN_RX_FIFO0, &CAN1_rx, received_msg) != HAL_OK)
	{
		Error_Handler();
	}

	sprintf(msg, "Received CAN message %s \r\n", received_msg);
	HAL_UART_Transmit(&Usart1Handle, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
	/*
	sprintf(msg, "Message Transmitted:M0\r\n");
	HAL_UART_Transmit(&Usart1Handle, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	*/
}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{
	/*
	sprintf(msg, "Message Transmitted:M1\r\n");
	HAL_UART_Transmit(&Usart1Handle, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	*/
}

void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
	/*
	sprintf(msg, "Message Transmitted:M2\r\n");
	HAL_UART_Transmit(&Usart1Handle, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	*/
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef CAN1_rx;
	uint8_t received_msg[8];

	if(HAL_CAN_GetRxMessage(&CAN1Handle, CAN_RX_FIFO0, &CAN1_rx, received_msg) != HAL_OK)
	{
		Error_Handler();
	}

	if(CAN1_rx.StdId == 0x65D && CAN1_rx.RTR == CAN_RTR_DATA)
	{
		// DATA Frame
		sprintf(msg, "Received CAN message(MUC1): %d \n\n\r", *(uint16_t *)received_msg);
		HAL_UART_Transmit(&Usart1Handle, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	}
	if(CAN1_rx.StdId == 0x651 && CAN1_rx.RTR == CAN_RTR_REMOTE)
	{
		// REMOTE Frame
		//CAN_Send(&received_msg);
	}
	if(CAN1_rx.StdId == 0x651 && CAN1_rx.RTR == CAN_RTR_DATA)
	{
		// Data Frame that returned from data request
		Print_Requested_message(received_msg);
	}
}

void Print_Requested_message(uint8_t* msg)
{
	sprintf(msg, "Received requested data: %d\n\r", *msg);
	HAL_UART_Transmit(&Usart1Handle, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	sprintf(msg, "!! CAN ERROR OCCURED\r\n");
	HAL_UART_Transmit(&Usart1Handle, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_1);

	if(tim6_glb_cnt == 4)
	{
		Request_Event();
	}
	Send_Event();

	tim6_glb_cnt %= 4;
	tim6_glb_cnt++;
}

void Send_Event(void)
{
	static uint8_t led_no = 0;

	CAN_Send(led_no);

	led_no++;
	led_no %= 4;
}

void Request_Event(void)
{
	CAN_Request();
}

void CAN_Send(uint8_t led_no)
{
	CAN_TxHeaderTypeDef CAN1_tx;

	uint32_t TxMaibox;

	CAN1_tx.StdId = 0x65D;
	CAN1_tx.ExtId = 0x1FFFFFFF;
	CAN1_tx.IDE = CAN_ID_STD;
	CAN1_tx.RTR = CAN_RTR_DATA;
	CAN1_tx.DLC = 1;
	CAN1_tx.TransmitGlobalTime = DISABLE;

	if(HAL_CAN_AddTxMessage(&CAN1Handle, &CAN1_tx, &led_no, &TxMaibox) != HAL_OK)
	{
		Error_Handler();
	}
}
void CAN_Request(void)
{
	CAN_TxHeaderTypeDef CAN1_tx;

	uint32_t TxMaibox;

	CAN1_tx.StdId = 0x651;
	CAN1_tx.ExtId = 0x1FFFFFFF;
	CAN1_tx.IDE = CAN_ID_STD;
	CAN1_tx.RTR = CAN_RTR_REMOTE;
	CAN1_tx.DLC = 2;
	CAN1_tx.TransmitGlobalTime = DISABLE;

	if(HAL_CAN_AddTxMessage(&CAN1Handle, &CAN1_tx, 0, &TxMaibox) != HAL_OK)
	{
		Error_Handler();
	}
}

void Error_Handler(void)
{
	while(1);
}
