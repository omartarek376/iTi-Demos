#include "MCAL/MGPIO/MGPIO_interface.h"
#include "HAL/HLED/HLED_interface.h"


extern const Led_CFG_t LEDS[];

void HLED_vLedInit(void)
{   
    u8 LOC_u8IDX = 0;
    MGPIO_PIN_config_t LED ;
    LED.GPIOMode=GPIO_OUTPUT;
    LED.GPIO_OUTPUTTYPE=GPIO_PUSHPULL;
    LED.GPIOSpeed=GPIO_HIGH_SPEED;
    LED.GPIO_INPUTTYPE=GPIO_NO_PULLUPPULLDOWN;

    for (LOC_u8IDX = 0; LOC_u8IDX <_Led_Num ;LOC_u8IDX++)
    {
        LED.GPIOPort=LEDS[LOC_u8IDX].Led_Port;
        LED.GPIOPin=LEDS[LOC_u8IDX].Led_Pin;
        MGPIO_enuSetPinConfig(&LED);
        MGPIO_enuSetPinValue(LEDS[LOC_u8IDX].Led_Port,LEDS[LOC_u8IDX].Led_Pin,LEDS[LOC_u8IDX].Led_InitStatus);
    }

}

void HLED_vSetLedStatus(u32 LED,u8 Status)
{   
    MGPIO_enuSetPinValue(LEDS[LED].Led_Port,LEDS[LED].Led_Pin,LEDS[LED].Led_Connection ^ Status );
}

void HLED_vToggle(u32 LED)
{
    u32 Led_State = 0;  
    MGPIO_getPinValue(LEDS[LED].Led_Port, LEDS[LED].Led_Pin, &Led_State);
    MGPIO_enuSetPinValue(LEDS[LED].Led_Port, LEDS[LED].Led_Pin, Led_State ^ 1);
}