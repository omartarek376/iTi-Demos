#include "HAL/HSWITCH/HSWITCH_interface.h"
#include "MCAL/MUSART/MUSART_interface.h"

#define UP_SWITCH_VALUE     0x01
#define DOWN_SWITCH_VALUE   0x02
#define RIGHT_SWITCH_VALUE  0x03
#define LEFT_SWITCH_VALUE   0x04
#define OK_SWITCH_VALUE     0x05
#define RESET_SWITCH_VALUE  0x06
#define MODE_SWITCH_VALUE   0x07
#define STOP_SWITCH_VALUE   0x08
#define START_SWITCH_VALUE  0x09
#define EDIT_SWITCH_VALUE   0x0A

u8 static readyForNewKey = 1;
u8 message[1] = {0};
static u8 switchState = 0;

                                                      
// static u8 Encryption(u8 value)                                                                          
// { 
//   u8 CheckSumBits= (~value) & 0x0F ;
//   u8 EncryptedMessage = (value << 4) | (CheckSumBits) ;
//   return EncryptedMessage;
// }


static u8 Encryption(u8 value) {
    u8 CheckSumBits = value & 0x0F; // Only consider lower 4 bits for checksum
    u8 EncryptedMessage = (value << 4) | (CheckSumBits);
    return EncryptedMessage;
}


void messageSent (void) 
{
	readyForNewKey = 1;
}
USART_Req_t USARAT_Bytes = {.length = 1, .buffer = message, .USART_Peri = USART_Peri_1, .CB = messageSent};

void switchesCheckRunnable (void) 
{   
	 u8 index = 0  ;


	if (readyForNewKey == 1) 
    {
		for (index=0 ; index < _Switch_Num ; index ++ )
		{
			switchState = HSWITCH_u32GetSwitchState(index);
			if(switchState == SWITCH_STATUS_PRESSED )
			{   
				// Stop responding for a new press till the current message is sent.
			 	readyForNewKey = 0;

			    //Generating the message by adding the checksum to the message.
                message[0]=Encryption(index+1);
			
			    // Send the message.
			   // MUSART_enuSendBufferAsync(USART_1,message,1,messageSent);
			   USART_TXBufferAsyncZC(USARAT_Bytes);

                break ;
			}
            
		}
	}
   else
  {

  }
}





// A runnable the comes every 100 ms to check if any of the buttons is pressed.
// void switchesCheckRunnable (void) 
// {
// 	u8 message = 0;
// 	u8 switchState = 0;
// 	static u8 counter = 0;

// 	if (readyForNewKey == 1) 
//     {
// 		switch (counter) 
//         {
// 		case 0:
// 		switchState = HSWITCH_u32GetSwitchState(UP_SWITCH);
// 		if (switchState == SWITCH_STATUS_PRESSED) {
// 			// Stop responding for a new press till the current message is sent.
// 			readyForNewKey = 0;
			
//             // Generating the message by adding the checksum to the message.
// 			message=Encryption(UP_SWITCH_VALUE);
			
// 			   if(buttonPressed == 0)
// 			   {
// 			    // Send the message.
// 			    MUSART_enuSendBufferAsync(USART_1,&message,1,messageSent);
// 			   }
// 		} 
//         else 
//         {
// 			/* Do Nothing */
// 		}
// 		counter++;
// 		break;
// 		case 1:
// 		switchState = HSWITCH_u32GetSwitchState(DOWN_SWITCH);
// 		if (switchState == SWITCH_STATUS_PRESSED) {
// 			// Stop responding for a new press till the current message is sent.
// 			readyForNewKey = 0;
			
// 			// Generating the message by adding the checksum to the message.
//             message=Encryption(DOWN_SWITCH_VALUE);
			
// 			if(buttonPressed == 0)
// 			   {
// 			    // Send the message.
// 			    MUSART_enuSendBufferAsync(USART_1,&message,1,messageSent);
// 			   }
// 		} else {
// 			/* Do Nothing */
// 		}
// 		counter++;
// 		break;
// 		case 2:
// 		switchState = HSWITCH_u32GetSwitchState(RIGHT_SWITCH);
// 		if (switchState == SWITCH_STATUS_PRESSED) 
//         {
// 			// Stop responding for a new press till the current message is sent.
// 			readyForNewKey = 0;
			
// 			// Generating the message by adding the checksum to the message.
//             message=Encryption(RIGHT_SWITCH_VALUE);
			
// 			if(buttonPressed == 0)
// 			   {
// 			    // Send the message.
// 			    MUSART_enuSendBufferAsync(USART_1,&message,1,messageSent);
// 			   }
// 		} else {
// 			/* Do Nothing */
// 		}
// 		counter++;
// 		break;
// 		case 3:
// 		switchState = HSWITCH_u32GetSwitchState(LEFT_SWITCH);
// 		if (switchState == SWITCH_STATUS_PRESSED) {
// 			// Stop responding for a new press till the current message is sent.
// 			readyForNewKey = 0;
			
// 			// Generating the message by adding the checksum to the message.
//             message=Encryption(LEFT_SWITCH_VALUE);
			
// 			if(buttonPressed == 0)
// 			   {
// 			    // Send the message.
// 			    MUSART_enuSendBufferAsync(USART_1,&message,1,messageSent);
// 			   }
// 		} else {
// 			/* Do Nothing */
// 		}
// 		counter++;
// 		break;
// 		case 4:
// 		switchState = HSWITCH_u32GetSwitchState(EDIT_SWITCH);
// 		if (switchState == SWITCH_STATUS_PRESSED) {
// 			// Stop responding for a new press till the current message is sent.
// 			readyForNewKey = 0;
			
// 			// Generating the message by adding the checksum to the message.
//             message=Encryption(EDIT_SWITCH_VALUE);
			
// 			if(buttonPressed == 0)
// 			   {
// 			    // Send the message.
// 			    MUSART_enuSendBufferAsync(USART_1,&message,1,messageSent);
// 			   }
// 		} else {
// 			/* Do Nothing */
// 		}
// 		counter++;
// 		break;
// 		case 5:
// 		switchState = HSWITCH_u32GetSwitchState(MODE_SWITCH);
// 		if (switchState == SWITCH_STATUS_PRESSED) {
			
// 			// Generating the message by adding the checksum to the message.
//             message=Encryption(MODE_SWITCH_VALUE);
			
// 			if(buttonPressed == 0)
// 			   {
// 				// Stop responding for a new press till the current message is sent.
// 				readyForNewKey = 0;
// 			    // Send the message.
// 			    MUSART_enuSendBufferAsync(USART_1,&message,1,messageSent);
// 			   }
// 		} else {
// 			/* Do Nothing */
// 		}
// 		counter++;
// 		break;
// 		case 6:
// 		switchState = HSWITCH_u32GetSwitchState(OK_SWITCH);
// 		if (switchState == SWITCH_STATUS_PRESSED) {
// 			// Stop responding for a new press till the current message is sent.
// 			readyForNewKey = 0;
			
// 			// Generating the message by adding the checksum to the message.
//             message=Encryption(OK_SWITCH_VALUE);
			
// 			if(buttonPressed == 0)
// 			   {
// 			    // Send the message.
// 			    MUSART_enuSendBufferAsync(USART_1,&message,1,messageSent);
// 			   }
// 		} else {
// 			/* Do Nothing */
// 		}
// 		counter++;
// 		break;
// 		case 7:
// 		switchState = HSWITCH_u32GetSwitchState(STOP_SWITCH);
// 		if (switchState == SWITCH_STATUS_PRESSED) {
// 			// Stop responding for a new press till the current message is sent.
// 			readyForNewKey = 0;
			
// 			// Generating the message by adding the checksum to the message.
//             message=Encryption(STOP_SWITCH_VALUE);
			
// 			if(buttonPressed == 0)
// 			   {
// 			    // Send the message.
// 			    MUSART_enuSendBufferAsync(USART_1,&message,1,messageSent);
// 			   }
// 		} else {
// 			/* Do Nothing */
// 		}
// 		counter++;
// 		break;
// 		case 8:
// 		switchState = HSWITCH_u32GetSwitchState(RESET_SWITCH);
// 		if (switchState == SWITCH_STATUS_PRESSED) {
// 			// Stop responding for a new press till the current message is sent.
// 			readyForNewKey = 0;
			
// 			// Generating the message by adding the checksum to the message.
//             message=Encryption(RESET_SWITCH_VALUE);
			
// 			if(buttonPressed == 0)
// 			   {
// 			    // Send the message.
// 			    MUSART_enuSendBufferAsync(USART_1,&message,1,messageSent);
// 			   }
// 		} else {
// 			/* Do Nothing */
// 		}
// 		counter++;
// 		break;
// 		case 9:
// 		switchState = HSWITCH_u32GetSwitchState(START_SWITCH);
// 		if (switchState == SWITCH_STATUS_PRESSED) 
//         {

// 			// Stop responding for a new press till the current message is sent.
// 			readyForNewKey = 0;
			
// 			// Generating the message by adding the checksum to the message.
//             message=Encryption(START_SWITCH_VALUE);
			
// 			  if(buttonPressed == 0)
// 			   {
// 			    // Send the message.
// 			    MUSART_enuSendBufferAsync(USART_1,&message,1,messageSent);
// 			   }
// 		}
//          else 
//         {
// 			/* Do Nothing */
// 		}
// 		counter = 0;
// 		break;
// 	} 
//     }
//     else 
//     {
// 		/* Do nothing as we are not ready for a new pressed key */	
// 	}
// }