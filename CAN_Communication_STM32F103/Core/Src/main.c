#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdio.h>

void Clock_Config(void);
void Error_Handler(char* msg);
void UART_Config(void);
void Start_LED(void);
void CAN1_Config(void);
void CAN1_TX(void);
void CAN1_FilterConfig(void);
void LED_Init(void);
void LED_Toggle(uint8_t received_msg);
void CAN_Send(void);

UART_HandleTypeDef UART_Cnfg;
CAN_HandleTypeDef CAN_Cnfg;
uint8_t msg[250];

int main(void)
{

	uint32_t clk_value;

	HAL_Init();

	Clock_Config();

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	clk_value = HAL_RCC_GetPCLK1Freq();

	UART_Config();

	sprintf((char*)msg, "PCLK1: %ld\n\r", clk_value);
	if(HAL_UART_Transmit(&UART_Cnfg, msg, sizeof(msg), HAL_MAX_DELAY) != HAL_OK)
	{
		Error_Handler("ERROR: UART Transmit\r\n");
	}

	LED_Init();

	CAN1_Config();

	CAN1_FilterConfig();

	if(HAL_CAN_ActivateNotification(&CAN_Cnfg, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_BUSOFF) != HAL_OK)
	{
		Error_Handler("ERROR: CAN IT ACTIVE\r\n");
	}

	if(HAL_CAN_Start(&CAN_Cnfg) != HAL_OK)
	{
		Error_Handler("ERROR: CAN START\r\n");
	}

	// CAN1_TX();

	Start_LED();

	while(1)
	{
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		HAL_Delay(400);
	}
	return 0;
}

void Clock_Config(void)
{
	RCC_OscInitTypeDef Osc_Config;
	RCC_ClkInitTypeDef Clk_Config;

	memset(&Osc_Config, 0 ,sizeof(Osc_Config));


	Osc_Config.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	Osc_Config.PLL.PLLMUL = RCC_PLL_MUL9;
	Osc_Config.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	Osc_Config.HSEState = RCC_HSE_ON;
	Osc_Config.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	Osc_Config.HSIState = RCC_HSI_ON;
	Osc_Config.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	Osc_Config.PLL.PLLState = RCC_PLL_ON;

	if(HAL_RCC_OscConfig(&Osc_Config) != HAL_OK)
	{
		Error_Handler("ERROR: Clock Congig \r\n");
	}

	Clk_Config.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 |RCC_CLOCKTYPE_PCLK2;
	Clk_Config.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	Clk_Config.AHBCLKDivider = RCC_SYSCLK_DIV1;
	Clk_Config.APB1CLKDivider = RCC_HCLK_DIV2;
	Clk_Config.APB2CLKDivider = RCC_HCLK_DIV1;

	if(HAL_RCC_ClockConfig(&Clk_Config, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler("ERROR: Clock Congig \r\n");
	}

	__HAL_RCC_HSI_DISABLE();
}

void UART_Config(void)
{
	UART_Cnfg.Instance = USART2;
	UART_Cnfg.Init.BaudRate = 115200;
	UART_Cnfg.Init.WordLength = UART_WORDLENGTH_8B;
	UART_Cnfg.Init.StopBits = UART_STOPBITS_1;
	UART_Cnfg.Init.Parity = UART_PARITY_NONE;
	UART_Cnfg.Init.Mode = UART_MODE_TX;
	UART_Cnfg.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UART_Cnfg.Init.OverSampling = UART_OVERSAMPLING_16;

	if(HAL_UART_Init(&UART_Cnfg) != HAL_OK)
	{
		Error_Handler("ERROR: UART Init \r\n");
	}
}

void CAN_Send(void)
{
	uint8_t data[2] = {3, 0};
	CAN_TxHeaderTypeDef CAN1_tx;

	uint32_t TxMaibox;

	CAN1_tx.StdId = 0x651;
	CAN1_tx.ExtId = 0x1FFFFFFF;
	CAN1_tx.IDE = CAN_ID_STD;
	CAN1_tx.RTR = CAN_RTR_DATA;
	CAN1_tx.DLC = 2;
	CAN1_tx.TransmitGlobalTime = DISABLE;

	if(HAL_CAN_AddTxMessage(&CAN_Cnfg, &CAN1_tx, data, &TxMaibox) != HAL_OK)
	{
		Error_Handler("ERROR: CAN TX \r\n");
	}
}

void LED_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef GPIOHandle = {0};

	GPIOHandle.Pin = GPIO_PIN_3;
	GPIOHandle.Mode = GPIO_MODE_OUTPUT_PP;
	GPIOHandle.Pull = GPIO_NOPULL;
	GPIOHandle.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIOHandle);

    GPIOHandle.Pin = GPIO_PIN_4;
    HAL_GPIO_Init(GPIOA, &GPIOHandle);

    GPIOHandle.Pin = GPIO_PIN_5;
    HAL_GPIO_Init(GPIOA, &GPIOHandle);

    GPIOHandle.Pin = GPIO_PIN_6;
    HAL_GPIO_Init(GPIOA, &GPIOHandle);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, 0);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0);
}

void CAN1_Config(void)
{
	CAN_Cnfg.Instance = CAN1;
	CAN_Cnfg.Init.Prescaler = 2;
	CAN_Cnfg.Init.TimeSeg1 = CAN_BS1_15TQ;
	CAN_Cnfg.Init.TimeSeg2 = CAN_BS2_2TQ;
	CAN_Cnfg.Init.Mode = CAN_MODE_NORMAL;
	CAN_Cnfg.Init.SyncJumpWidth = CAN_SJW_1TQ;
	CAN_Cnfg.Init.TimeTriggeredMode = DISABLE;
	CAN_Cnfg.Init.AutoBusOff = DISABLE;
	CAN_Cnfg.Init.AutoWakeUp = DISABLE;
	CAN_Cnfg.Init.AutoRetransmission = ENABLE;
	CAN_Cnfg.Init.ReceiveFifoLocked = DISABLE;
	CAN_Cnfg.Init.TransmitFifoPriority =  DISABLE;

	if(HAL_CAN_Init(&CAN_Cnfg) != HAL_OK)
	{
		Error_Handler("ERROR: CAN Init \r\n");
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

	if(HAL_CAN_AddTxMessage(&CAN_Cnfg, &CAN1_tx, message, &TxMaibox) != HAL_OK)
	{
		Error_Handler("ERROR: CAN TX \r\n");
	}
}

void Start_LED(void)
{
	__HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_Cnfg;

	// Configure GPIOs
	GPIO_Cnfg.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_Cnfg.Pin = GPIO_PIN_13;
	GPIO_Cnfg.Pull = GPIO_NOPULL;
	GPIO_Cnfg.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOC, &GPIO_Cnfg);
}

void LED_Toggle(uint8_t received_msg)
{
	switch(received_msg)
	{
	case 0:
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_3);
		break;
	case 1:
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);
		break;
	case 2:
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		break;
	case 3:
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_6);
		break;
	default:
		;
	}
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
	sprintf((char*)msg, "Transmit Done: Mbox0 \r\n");
	if(HAL_UART_Transmit(&UART_Cnfg, msg, sizeof(msg), HAL_MAX_DELAY) != HAL_OK)
	{
		Error_Handler("ERROR: UART Transmit\r\n");
	}
}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{
	sprintf((char*)msg, "Transmit Done: Mbox1\n\r");
	if(HAL_UART_Transmit(&UART_Cnfg, msg, sizeof(msg), HAL_MAX_DELAY) != HAL_OK)
	{
		Error_Handler("ERROR: UART Transmit\r\n");
	}
}

void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
	sprintf((char*)msg, "Transmit Done: Mbox2\n\r");
	if(HAL_UART_Transmit(&UART_Cnfg, msg, sizeof(msg), HAL_MAX_DELAY) != HAL_OK)
	{
		Error_Handler("ERROR: UART Transmit\r\n");
	}
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef CAN1_rx;
	static uint8_t received_msg[8];

	if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN1_rx, received_msg) != HAL_OK)
	{
		Error_Handler("ERROR: Can get rx\r\n");
	}

	if( (CAN1_rx.StdId == 0x65D) && (CAN1_rx.RTR == CAN_RTR_DATA) )
	{
		// DATA Frame
		LED_Toggle(received_msg[0]);
	}
	if( (CAN1_rx.StdId == 0x651) && (CAN1_rx.RTR == CAN_RTR_REMOTE) )
	{
		// REMOTE Frame
		CAN_Send();
	}
	if(CAN1_rx.StdId == 0x651 && CAN1_rx.RTR == CAN_RTR_DATA)
	{
		sprintf(msg, "Received CAN message that requested(MUC1): %d \n\n\r", received_msg[0] << 8 | received_msg[1]);
		HAL_UART_Transmit(&UART_Cnfg, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	}

	sprintf(msg, "Received CAN message(MCU2): %d \n\n\r", received_msg[0]);
	HAL_UART_Transmit(&UART_Cnfg, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
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

	if(HAL_CAN_ConfigFilter(&CAN_Cnfg, &CanFilter) != HAL_OK)
	{
		Error_Handler("ERROR: Can Filter Config\r\n");
	}
}

void Error_Handler(char* msg)
{
	if(HAL_UART_Transmit(&UART_Cnfg, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY) != HAL_OK)
	{
	}
	while(1);
}
