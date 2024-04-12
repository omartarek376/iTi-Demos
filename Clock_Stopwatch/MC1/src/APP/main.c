#include "MCAL/USART.h"
#include "MCAL/GPIO.h"
#include "MCAL/SYSTICK.h"
#include "HAL/Switch.h"
#include "HAL/LCD.h"
#include "MCAL/RCC.h"
#include "SERVICE/SCHED.h"
#include "MCAL/NVIC.h"
#include "STM32f401CC_peripherals_interrupt.h"


void lcd_done(void)
{

}


int main()
{
	RCC_Control_AHB1Peripherals(RCC_PERI_AHB1ENR_GPIOA,Status_Enable);
	RCC_Control_AHB1Peripherals(RCC_PERI_AHB1ENR_GPIOC,Status_Enable);
	LCD_enuInitAsync(lcd_done);	
	Switch_init();
	NVIC_EnableIRQ(USART1_IRQ);
	//USART_Init();
	SCHED_Init();
	SCHED_Start();	

	while (1)
	{
	}
	
}


