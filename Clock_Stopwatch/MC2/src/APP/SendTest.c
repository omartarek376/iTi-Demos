#include "HAL/HLED/HLED_interface.h"
#include "HAL/HSWITCH/HSWITCH_interface.h"
#include "MCAl/MUSART/MUSART_interface.h"

// u8 switchstate [1] = {2}; 
// static u8 counter = 0;


// static void Dummycb (void)
// {

// }
// USART_Req_t USARAT_Bytes = {.length = 1, .buffer = switchstate, .USART_Peri = USART_Peri_1, .CB = Dummycb};
// void sendrunnable (void)
// {
//          switchstate[0] = HSWITCH_u32GetSwitchState(MODE_SWITCH);

//          USART_TXBufferAsyncZC(USARAT_Bytes);
//          // MUSART_enuSendBufferAsync(USART_1,switchstate,1,Dummycb);
//          switchstate[0] = 0 ;
         
// }