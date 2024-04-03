#include "HAL/LED.h"
#include "MCAL/RCC.h"
#include "MCAL/USART.h"
#include "MCAL/GPIO.h"
#include "MCAL/NVIC.h"
#include "MCAL/STM32F401CC_NVIC.h"


USART_RXBuffer rx_buff = 
{
	.Channel = USART1,
	.Data = NULL,
	.Size = 1,
	.Index = 0
};

USART_TXBuffer tx_buff = 
{
	.Channel = USART1,
	.Data = "Hello darkness my old friend",
	.Size = 28
};

USART_TXBuffer tx_buff2 = 
{
	.Channel = USART1,
	.Data = "Bye my love",
	.Size = 11
};

void recieve_callback(void)
{
	if (rx_buff.Data == 'y')
		{
			USART_SendBufferZeroCopy(&tx_buff);
		}
		else if (rx_buff.Data == 'n')
		{
			USART_SendBufferZeroCopy(&tx_buff2);
		}
}

int main()
{
	RCC_Enable_AHB1_Peripheral(AHB1_GPIOC_ENABLE,STATE_ON);
	RCC_Enable_AHB1_Peripheral(AHB1_GPIOA_ENABLE,STATE_ON);
	RCC_Enable_APB2_Peripheral(APB2_USART1_ENABLE,STATE_ON);
	NVIC_EnableIRQ(IRQ_USART1_INTERRUPT);
	

	USART_Config usart_config = {
        .Channel = USART1,
        .Oversampling = OVERSAMPLING_8,
        .USART_Enable = ENABLE,
        .WordLength = WORDLENGTH_8,
        .ParityControl = PARITY_NONE,
        .TransComplete_Int = ENABLE,
        .TransEnable = ENABLE,
		.ReceiveEnable = ENABLE,
		.ReceiveDateRegisterEmpty_Int = ENABLE,
        .BaudRate = 9600
    };

	GPIO_CONFIG_T TX_PIN = 
	{
		.Mode = GPIO_MODE_AF_PP,
		.Speed = GPIO_SPEED_HIGH,
		.Port = GPIO_PORT_A,
		.Pin = GPIO_PIN_9,
		.Alternative = GPIO_ALTERNATIVE_USART1_2
	};

	GPIO_CONFIG_T RX_PIN = 
	{
		.Mode = GPIO_MODE_AF_PP_PU,
		.Speed = GPIO_SPEED_HIGH,
		.Port = GPIO_PORT_A,
		.Pin = GPIO_PIN_10,
		.Alternative = GPIO_ALTERNATIVE_USART1_2
	};


	GPIO_InitPin(&TX_PIN);
	GPIO_InitPin(&RX_PIN);
	led_init();
	USART_Init(&usart_config);
	USART_ReceiveBuffer(&rx_buff);
	USART_RegisterCallBackFunction(USART1,RECEIVE,recieve_callback);

	while (1)
	{
	}
	
}


