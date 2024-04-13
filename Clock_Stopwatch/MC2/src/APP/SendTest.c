#include "HAL/HLED/HLED_interface.h"
#include "HAL/HSWITCH/HSWITCH_interface.h"
#include "MCAl/MUSART/MUSART_interface.h"

u8 switchstate [1] = {1}; 
static u8 counter = 0;
static void Dummycb (void)
{

}

void sendrunnable (void)
{
    if (counter == 2)
    {
         //switchstate[0] = HSWITCH_u32GetSwitchState(MODE_SWITCH);
    
         MUSART_enuSendBufferAsync(USART_1,switchstate,1,Dummycb);
    }
    else
    {

    }
   counter ++;

}