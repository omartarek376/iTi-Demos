#include "MCAL/MRCC/MRCC_interface.h"
#include "MCAL/MGPIO/MGPIO_interface.h"
#include "HAL/HLED/HLED_interface.h"
#include "HAL/HSWITCH/HSWITCH_interface.h"
#include "MCAL/MSYSTICK/MSYSTICK_interface.h"
#include "SERVICES/SCHED/SCHED_interface.h"
#include "HAL/HLCD/HLCD_interface.h"
#include "MCAL/MUSART/MUSART_interface.h"
#include "MCAL/MNVIC/MNVIC_interface.h"

static void DummyCB (void)
{

}

int main(void)
{ 
  RCC_enuEnableAHB1Peripheral(AHB1_GPIOA);
  RCC_enuEnableAHB1Peripheral(AHB1_GPIOB);
  RCC_enuEnableAHB1Peripheral(AHB1_GPIOC);
  RCC_enuEnableAPB2Peripheral(APB2_USART1);

  MGPIO_PIN_config_t USART_Tx_Pin_Config = {
    .GPIOPort = GPIO_PORTB,                        // Assuming USART Tx pin is connected to GPIO Port A
    .GPIOPin = GPIO_PIN6,                          // Assuming USART Tx pin is connected to pin 9
    .GPIOMode = GPIO_AF,                           // Alternative function mode
    .GPIO_OUTPUTTYPE = GPIO_PUSHPULL,              // Push-pull output type
    .GPIO_INPUTTYPE = GPIO_NO_PULLUPPULLDOWN,      // No pull-up/pull-down
    .GPIOSpeed = GPIO_HIGH_SPEED,                  // High speed
    .GPIOAlternative = GPIO_AF_USART_1_2           // USART alternative function
};

MGPIO_PIN_config_t USART_Rx_Pin_Config = {
    .GPIOPort = GPIO_PORTB,             // Assuming USART Rx pin is connected to GPIO Port A
    .GPIOPin = GPIO_PIN7,              // Assuming USART Rx pin is connected to pin 10
    .GPIOMode = GPIO_AF,                // Alternative function mode
    .GPIO_OUTPUTTYPE = GPIO_PUSHPULL,   // Push-pull output type
    .GPIO_INPUTTYPE = GPIO_NO_PULLUPPULLDOWN,  // No pull-up/pull-down
    .GPIOSpeed = GPIO_HIGH_SPEED,       // High speed
    .GPIOAlternative = GPIO_AF_USART_1_2  // USART alternative function
};
MGPIO_enuSetPinConfig(&USART_Tx_Pin_Config);
MGPIO_enuSetPinConfig(&USART_Rx_Pin_Config);

MUSART_enuInit();
NVIC_EnableIRQ(IRQ_USART1);
   

  HLED_vLedInit();
  LCD_enuInitAsync(DummyCB);
  HSWITCH_vSwitchInit();
  SCHED_init();
  SCHED_start();   

 
   
    return 0 ;
}

