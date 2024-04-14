#include "HAL/HLED/HLED_interface.h"
#include "HAL/HSWITCH/HSWITCH_interface.h"
#include "MCAl/MUSART/MUSART_interface.h"

u8 recived [1] ={0} ;

static void Dummycb (void)
{

}
void reciverunnable (void)
{  
    MUSART_enuRecieveBufferAsync(USART_1,recived,1,Dummycb);

    if (recived[0] == 1)
    {
       HLED_vSetLedStatus(Led_Start,LED_STATUS_ON);
      // HLED_vToggle(Led_Start);
       recived[0] = 0 ;
    }
    else
    {
         HLED_vSetLedStatus(Led_Start,LED_STATUS_OFF);
    }
   
}