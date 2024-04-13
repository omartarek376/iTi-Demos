#include "HAL/HLED/HLED_interface.h"
#include "HAL/HSWITCH/HSWITCH_interface.h"
#include "MCAl/MUSART/MUSART_interface.h"

u8 recived [1] ={2} ;

static void Dummycb (void)
{

}

void reciverunnable (void)
{  
    u8 static counter =0;
    if (counter == 0 || counter == 4)
    {

    
    MUSART_enuSendBufferAsync(USART_1,recived,1,Dummycb);

    if (recived[0] == 1)
    {
        HLED_vSetLedStatus(Led_Start,LED_STATUS_ON);
    }
    else if (recived[0] == 0)
    {
         HLED_vSetLedStatus(Led_Start,LED_STATUS_OFF);
    }
    else
    {
        
    }
    }
    counter ++;
 
}