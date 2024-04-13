#include "MCAL/MGPIO/MGPIO_interface.h" 
#include "HAL/HLED/HLED_interface.h"
#include "CFG/HLED/HLED_config.h"

const Led_CFG_t LEDS[_Led_Num] =
{
    [Led_Start]=
    {
        .Led_Port=GPIO_PORTC,
        .Led_Pin=GPIO_PIN14,
        .Led_Connection=LED_CONN_FORWARD,
        .Led_InitStatus=LED_STATUS_OFF
    }


};