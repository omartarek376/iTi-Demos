// #include "MCAl/MUSART/MUSART_interface.h"


// u8 recivedMessage[1] = {0};
// u8 receiveFlag = 0 ;

// void receiveCallback (void)
// {
//   receiveFlag = 1 ;
// }
// USART_Req_t USARATR_Bytes = {.length = 1, .buffer = recivedMessage, .USART_Peri = USART_Peri_1, .CB = receiveCallback };
// void recieveRunnable(void)
// {
//     //	MUSART_enuRecieveBufferAsync(USART_1,recivedMessage,1,receiveCallback);
//       USART_RXBufferAsyncZC(USARATR_Bytes);


// }