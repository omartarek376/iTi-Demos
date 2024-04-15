#include "HAL/HLED/HLED_interface.h"
#include "HAL/HSWITCH/HSWITCH_interface.h"
#include "MCAl/MUSART/MUSART_interface.h"

// u8 recived [1] ={2} ;
// u8 x = 0 ;

// static void Dummycb (void)
// {
//    x++ ;
// }
// USART_Req_t USARATR_Bytes = {.length = 1, .buffer = recived, .USART_Peri = USART_Peri_1, .CB = Dummycb};
// void reciverunnable (void)
// {  
//  //   MUSART_enuRecieveBufferAsync(USART_1,recived,1,Dummycb);

//   USART_RXBufferAsyncZC(USARATR_Bytes);

//     if (recived[0] == 1)
//     {
//        HLED_vSetLedStatus(Led_Start,LED_STATUS_ON);
//       // HLED_vToggle(Led_Start);
//        recived[0] = 2 ;
//     }
//     else if (recived[0] == 0)
//     {
//          HLED_vSetLedStatus(Led_Start,LED_STATUS_OFF);
//          recived[0] = 2 ;
//     }
   
// }