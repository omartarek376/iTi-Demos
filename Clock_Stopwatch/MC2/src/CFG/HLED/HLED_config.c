#include "MCAL/MGPIO/MGPIO_interface.h" 
#include "HAL/HLED/HLED_interface.h"
#include "CFG/HLED/HLED_config.h"

const Led_CFG_t LEDS[_Led_Num] =
{
    [Led_Start]=
    {
        .Led_Port=GPIO_PORTA,
        .Led_Pin=GPIO_PIN7,
        .Led_Connection=LED_CONN_FORWARD,
        .Led_InitStatus=LED_STATUS_OFF
    },
    //  [Led_GREEN]=
    // {
    //     .Led_Port=GPIO_PORTB,
    //     .Led_Pin=GPIO_PIN5,
    //     .Led_Connection=LED_CONN_FORWARD,
    //     .Led_InitStatus=LED_STATUS_OFF
    // },
    //  [Led_RED]=
    // {
    //     .Led_Port=GPIO_PORTB,
    //     .Led_Pin=GPIO_PIN6,
    //     .Led_Connection=LED_CONN_FORWARD,
    //     .Led_InitStatus=LED_STATUS_OFF
    //  },
    // [Led_YELLOW]=
    // {
    //     .Led_Port=GPIO_PORTB,
    //     .Led_Pin=GPIO_PIN7,
    //     .Led_Connection=LED_CONN_FORWARD,
    //     .Led_InitStatus=LED_STATUS_OFF
    // },
    // [Led_Switch]
    // {
    //     .Led_Port=GPIO_PORTB,
    //     .Led_Pin=GPIO_PIN1,
    //     .Led_Connection=LED_CONN_FORWARD,
    //     .Led_InitStatus=LED_STATUS_OFF
    // }
    


};