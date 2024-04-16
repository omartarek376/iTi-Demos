/******************************************************************************
 *
 * Project: Digital Clock: 	- Display configurable date and time.
 * 							- Display configurable stopwatch.
 *
 * File Name: ClockRunnable.c
 *
 * Description: Runnable concerned with Clock mode in the project.
 *
 * Authors: Mina Ayman, Shaher Shah.
 *
 * Date: 08-04-2024
 *
 *******************************************************************************/


/************************************************************************************/
/*										Includes									*/
/************************************************************************************/

#include "MCAL/MUSART/MUSART_interface.h"
#include "HAL/HLCD/HLCD_interface.h"


/************************************************************************************/
/*									Macros Declaration								*/
/************************************************************************************/



#define RESET_SWITCH_VALUE				0x06
#define MODE_SWITCH_VALUE				0x07
#define STOP_SWITCH_VALUE				0x08
#define START_SWITCH_VALUE				0x09

#define CORRUPTED_MESSAGE				0xFF

#define HOURS_TENS_POSITION				  18
#define HOURS_UNITS_POSITION			  19
#define MINUTES_TENS_POSITION			  21
#define MINUTES_UNITS_POSITION		      22
#define SECONDS_TENS_POSITION			  24
#define SECONDS_UNITS_POSITION		      25
#define MSECONDS_HUNDREDS_POSITION	      27

/************************************************************************************/
/*							User-defined types Declaration							*/
/************************************************************************************/


/**
 *@brief : Boolean Values
 */
enum
{
	FALSE = 0,
	TRUE
};

/**
 *@brief : The Modes types
 */
typedef enum
{
	CLOCK_MODE,
	STOPWATCH_MODE
}MODES;

/**
 *@brief : The edit button status types
 */
typedef enum
{ 
	NOT_ACTIVATED,
	ACTIVATED
}EDITMODES;

/**
 *@brief : The Ok button status types
 */
typedef enum
{ 
	NOT_PRESSED,
	FIRST_PRESSED
}OKSTATE;


/************************************************************************************/
/*								Variables's Declaration								*/
/************************************************************************************/

extern MODES Mode ;

extern MODES previousMode ;

static EDITMODES EditMode = NOT_ACTIVATED;

static u8 receiveFlag = FALSE;

static u8 printEntireScreen = TRUE;

static u8 startFlag = FALSE ;

static u8 resetFlag = FALSE ;

static u8 buttonHandled = TRUE ;

static u8 clearOnce = FALSE ;

u8 stopwatchRecivedMessage[1]  = {0};


/************************************************************************************/
/*							Static Functions' Implementation						*/
/************************************************************************************/


/**
 *@brief : Function that decryptes the received encrypted message from the UART.
 *@param : Rececived message.
 *@return: Data that is extracted from the recevied encrypted message.
 */
// static u8 Decryption(u8 value)                                                                          
// {
// 	/* Extracting the Checksum bits from the received encrypted message */
// 	u8 CheckSumBits = (value) & 0x0F;
// 	/* Extracting the data bits from the received encrypted message */
// 	u8 Data = (value >> 4);

// 	/* Check if the received message is correct and not corrupted */
// 	if((CheckSumBits ^ value) == 0x0F)
// 	{
// 		/* If the received message is correct and not corrupted, return it */
// 	}
// 	else
// 	{
// 		/* If the received message is corrupted, return an info indicates that */
// 		Data = CORRUPTED_MESSAGE;
// 	}
// 	return Data;
// }

// static u8 Decryption(u8 value) {
//     u8 CheckSumBits = value & 0x0F;
//     u8 Data = (value >> 4);

//     /* Verify Checksum */
//     u8 calculatedChecksum = (~Data) & 0x0F;
//     if (CheckSumBits == calculatedChecksum) {
//         return Data; // Return data if checksum matches
//     } else {
//         return CORRUPTED_MESSAGE; // Return error code for corrupted message
//     }
// }

static u8 Decryption(u8 value) {
    u8 CheckSumBits = value & 0x0F;
    u8 Data = (value >> 4);

    /* Verify Checksum */
    if ((Data ^ CheckSumBits) == 0) {
        return Data; // Return data if checksum matches
    } else {
        return CORRUPTED_MESSAGE; // Return error code for corrupted message
    }
}

/**
 *@brief : Dummy function that passed to any Asynchronus function.
 *@param : void.
 *@return: void.
 */
static void DummyCB(void)
{

}


/**
 *@brief : Callback function of the MUSART_enuRecieveBufferAsync function, it raises a receive flag.
 *@param : void.
 *@return: void.
 */
static void receiveCallback(void)
{ 
	receiveFlag = TRUE;
	buttonHandled = FALSE;	 
}

//USART_Req_t StopwatchRecived_Bytes = {.length = 1, .buffer = stopwatchRecivedMessage, .USART_Peri = USART_Peri_1, .CB = receiveCallback };

/************************************************************************************/
/*								Functions' Implementation							*/
/************************************************************************************/


/**
 *@brief : A runnable that comes every 100 milliseconds to update the date and time, and to
			respond to the users requests.
 *@param : void.
 *@return: void.
 */
void StopwatchRunnable(void)
{  
	static u32  entryCounter  = 0;
	static u32  LCD_Counter   = 0;
	static u32  printCounter  = 0;

	/* Variables related to the date and time. Initially We are setting them as follows */
	static u8 hours   = 0;
	static u8 minutes = 0;
	static u8 seconds = 0;
	static u16 milliseconds = 0;


	/************************************************************************************/
	/* 			The following part updates the date and time every 1 seconds 			*/
	/************************************************************************************/
	if(startFlag == TRUE && entryCounter %5 == 0 && entryCounter !=0)
	{ 
		milliseconds +=100;

		if(milliseconds == 1000)
		{
			milliseconds =0;
			/* Every 1 second update the seconds variable */
			seconds++;
			if(seconds == 60)
			{
				/* If 60 seconds passed reset the seconds digits and increment the minutes digits by one */
				seconds = 0;
				minutes++;
				if(minutes == 60)
				{
					/* If 60 minutes passed reset the minutes digits and increment the hours digits by one */
					minutes = 0;
					hours++;
					if (hours == 100)
					{
						hours=0;
					}
				}

			}
		}

	}
	if(startFlag == TRUE)
	{
		entryCounter ++ ;
	}
	if( Mode == STOPWATCH_MODE && printEntireScreen == FALSE &&  startFlag == TRUE)
	{
		switch(printCounter)
		{
			case 0:
			if(clearOnce == FALSE)
			{
				LCD_enuClearScreenAsync(DummyCB);
				clearOnce = TRUE ;
			}
			printCounter ++ ;
			break ;
			case 1 :
			printCounter ++ ;
			break;
		case 2 :
			if (previousMode == CLOCK_MODE)
			{
				LCD_enuSetCursorAsync  (LCD_enuFirstRow,LCD_enuColumn_4 ,DummyCB);
			}
			else
			{
				LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_3,DummyCB);
			}
			printCounter ++;
			break;
		case 3:
			printCounter++;
			break;
		case 4 :
			if (previousMode == CLOCK_MODE)
			{
				LCD_enuWriteStringAsync("STOPWATCH",DummyCB);
			}
			else
			{
				if (hours < 10)
				{
					LCD_enuWriteStringAsync("0",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(hours,DummyCB);
				}
			}
			printCounter ++;
			break;
		case 5:
			printCounter ++;
			break;
		case 6:
			printCounter ++;
			break;
		case 7:
			printCounter ++;
			break;
		case 8:
			printCounter ++;
			break;
		case 9:
			if (previousMode == CLOCK_MODE)
			{
				LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_3,DummyCB);
			}
			else
			{
				if(hours < 10)
				{
					LCD_enuWriteNumberAsync(hours,DummyCB);
				}
				else
				{

				}
			}
			printCounter ++;
			break;
		case 10:
			printCounter ++;
			break;
		case 11:
			if (previousMode == CLOCK_MODE)
			{
				if (hours < 10)
				{
					LCD_enuWriteStringAsync("0",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(hours,DummyCB);
				}
			}
			else
			{
				LCD_enuWriteStringAsync(":",DummyCB);
			}
			printCounter ++;
			break;
		case 12:
			printCounter ++;
			break;
		case 13:
			if (previousMode == CLOCK_MODE)
			{
				if(hours < 10)
				{
					LCD_enuWriteNumberAsync(hours,DummyCB);
				}
				else
				{

				}
			}
			else
			{
				if (minutes < 10)
				{
					LCD_enuWriteStringAsync("0",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(minutes,DummyCB);
				}
			}
			printCounter ++;
			break;
		case 14:
			printCounter ++;
			break;
		case 15:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			printCounter ++;
			break;
		case 16:
			printCounter ++;
			break;
		case 17:
			if(milliseconds == 0)
			{
				LCD_enuWriteStringAsync("000",DummyCB);
			}
			else
			{
				LCD_enuWriteNumberAsync(milliseconds,DummyCB);
			}
			printCounter ++;
			break;
		case 18:
			printCounter ++;
			break;
		case 19 :
			if (previousMode == CLOCK_MODE)
			{
				LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_5,DummyCB);
			}
			else
			{
				if (minutes < 10)
				{
					LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_7,DummyCB);
				}
				else
				{
					LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_8,DummyCB);
				}
			}
			printCounter ++;
			break;
		case 20:
			printCounter ++;
			break;
		case 21:
			if (previousMode == CLOCK_MODE)
			{
				LCD_enuWriteStringAsync(":",DummyCB);
			}
			else
			{
				if(minutes < 10)
				{
					LCD_enuWriteNumberAsync(minutes,DummyCB);
				}
				else
				{

				}
			}
			printCounter ++;
			break;
		case 22:
			printCounter ++;
			break;
		case 23:
			if (previousMode == CLOCK_MODE)
			{
				if (minutes < 10)
				{
					LCD_enuWriteStringAsync("0",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(minutes,DummyCB);
				}
			}
			else
			{
				LCD_enuWriteStringAsync(":",DummyCB);
			}
			printCounter ++;
			break;
		case 24:
			printCounter ++;
			break;
		case 25:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			printCounter ++;
			break;
		case 26:
			printCounter ++;
			break;
		case 27:
			if(milliseconds == 0)
			{
				LCD_enuWriteStringAsync("000",DummyCB);
			}
			else
			{
				LCD_enuWriteNumberAsync(milliseconds,DummyCB);
			}
			printCounter ++;
			break;
		case 28:
			printCounter ++;
			break;
		case 29 :
			if (previousMode == CLOCK_MODE)
			{
				if (minutes < 10)
				{
					LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_7,DummyCB);
				}
				else
				{
					LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_8,DummyCB);
				}
			}
			else
			{
				LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_9,DummyCB);
			}
			printCounter ++;
			break;
		case 30:
			printCounter ++;
			break;
		case 31:
			if (previousMode == CLOCK_MODE)
			{
				if(minutes < 10)
				{
					LCD_enuWriteNumberAsync(minutes,DummyCB);
				}
				else
				{

				}
			}
			else
			{
				if (seconds < 10)
				{
					LCD_enuWriteStringAsync("0",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(seconds,DummyCB);
				}
			}
			printCounter ++;
			break;
		case 32:
			printCounter ++;
			break;
		case 33:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			printCounter ++;
			break;
		case 34:
			printCounter ++;
			break;
		case 35:
			if(milliseconds == 0)
			{
				LCD_enuWriteStringAsync("000",DummyCB);
			}
			else
			{
				LCD_enuWriteNumberAsync(milliseconds,DummyCB);
			}
			printCounter ++;
			break;
		case 36:
			printCounter ++;
			break;
		case 37:
			if (previousMode == CLOCK_MODE)
			{
				LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_8,DummyCB);
			}
			else
			{
				if (seconds < 10)
				{
					LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_9,DummyCB);
				}
				else
				{
					LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_11,DummyCB);
				}
			}
			printCounter ++;
			break;
		case 38:
			printCounter ++;
			break;
		case 39:
			if (previousMode == CLOCK_MODE)
			{
				LCD_enuWriteStringAsync(":",DummyCB);
			}
			else
			{
				if(seconds < 10)
				{
					LCD_enuWriteNumberAsync(seconds,DummyCB);
				}
				else
				{

				}
			}
			printCounter ++;
			break;
		case 40:
			printCounter ++;
			break;
		case 41:
			if (previousMode == CLOCK_MODE)
			{
				if (seconds < 10)
				{
					LCD_enuWriteStringAsync("0",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(seconds,DummyCB);
				}
			}
			else
			{
				LCD_enuWriteStringAsync(":",DummyCB);
			}
			printCounter ++;
			break;
		case 42:
			printCounter ++;
			break;
		case 43:
			if (previousMode == CLOCK_MODE)
			{
				if(seconds < 10)
				{
					LCD_enuWriteNumberAsync(seconds,DummyCB);
				}
				else
				{

				}
			}
			else
			{
				if(milliseconds == 0)
				{
					LCD_enuWriteStringAsync("000",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(milliseconds,DummyCB);
				}
			}
			printCounter ++;
			break;
		case 44:
			printCounter++;
			break;
		case 45:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			printCounter ++;
			break;
		case 46:
			printCounter ++;
			break;
		case 47:
			if(milliseconds == 0)
			{
				LCD_enuWriteStringAsync("000",DummyCB);
			}
			else
			{
				LCD_enuWriteNumberAsync(milliseconds,DummyCB);
			}
			printCounter ++;
			break;
		case 48:
			printCounter ++;
			break;
		case 49 :
			if (previousMode == CLOCK_MODE)
			{
				LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_11,DummyCB);
			}
			else
			{

			}
			printCounter ++;

			break;
		case 50:
			printCounter ++;
			break;
		case 51 :
			if (previousMode == CLOCK_MODE)
			{
				LCD_enuWriteStringAsync(":",DummyCB);
			}
			else
			{

			}
			printCounter ++;
			break;
		case 52:
			printCounter ++;
			break;
		case 53:
			if (previousMode == CLOCK_MODE)
			{
				if(milliseconds == 0)
				{
					LCD_enuWriteStringAsync("000",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(milliseconds,DummyCB);
				}
				//previousMode = STOPWATCH_MODE;
			}
			else
			{

			}
			printCounter ++;
			break;
		case 54 :
			printCounter ++;
			break;
		case 55:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			printCounter ++;
			break;
		case 56:
			printCounter ++;
			break;
		case 57:
			if(milliseconds == 0)
			{
				LCD_enuWriteStringAsync("000",DummyCB);
			}
			else
			{
				LCD_enuWriteNumberAsync(milliseconds,DummyCB);
			}
			printCounter ++;
			break;
		case 58:
			printCounter ++;
			break;
		case 59:
			printCounter ++;
			break;
		case 60:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			printCounter ++;
			break;
		case 61:
			printCounter ++;
			break;
		case 62:
			if(milliseconds == 0)
			{
				LCD_enuWriteStringAsync("000",DummyCB);
			}
			else
			{
				LCD_enuWriteNumberAsync(milliseconds,DummyCB);
			}
			printCounter ++;
			break;
		case 63:
			printCounter ++;
			break;
		case 64:
			printCounter ++;
			break;
		case 65:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			printCounter ++;
			break;
		case 66:
			printCounter ++;
			break;
		case 67:
			if(milliseconds == 0)
			{
				LCD_enuWriteStringAsync("000",DummyCB);
			}
			else
			{
				LCD_enuWriteNumberAsync(milliseconds,DummyCB);
			}
			printCounter ++;
			break;
		case 68:
			printCounter ++;
			break;
		case 69:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			printCounter ++;
			break;
		case 70:
			printCounter ++;
			break;
		case 71:
			if(milliseconds == 0)
			{
				LCD_enuWriteStringAsync("000",DummyCB);
			}
			else
			{
				LCD_enuWriteNumberAsync(milliseconds,DummyCB);
			}
			printCounter ++;
			break;
		case 72:
			printCounter ++;
			break;
		case 73:
			LCD_enuSetCursorAsync(LCD_enuSecondRow, LCD_enuColumn_9, DummyCB);
			printCounter ++;
			break;
		case 74:
			printCounter ++;
			break;
		case 75:
			if (seconds < 10)
			{
				LCD_enuWriteStringAsync("0", DummyCB);
			}
			else
			{
				LCD_enuWriteNumberAsync(seconds, DummyCB);
			}
			printCounter ++;
			break;
		case 76:
			printCounter ++;
			break;
		case 77:
			if (seconds < 10)
			{
				LCD_enuWriteNumberAsync(seconds, DummyCB);
			}
			else
			{

			}
			printCounter ++;
			break;
		case 78:
			printCounter ++;
			break;
		case 79:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			printCounter ++;
			break;
		case 80:
			printCounter ++;
			break;
		case 81:
			if(milliseconds == 0)
			{
				LCD_enuWriteStringAsync("000",DummyCB);
			}
			else
			{
				LCD_enuWriteNumberAsync(milliseconds,DummyCB);
			}
			printCounter ++;
			break;
		case 82:
			printCounter ++;
			break;
		case 83:
			printCounter ++;
			break;
		case 84:
			printCounter =0;
			break;
		}
	}
	/************************************************************************************/
	/* 	The following part checks on the selected mode, if it is the CLOCK_MODE, it Will
		display Date and Time after each update */
	/************************************************************************************/
	if(Mode == STOPWATCH_MODE)
	{  
		if(printEntireScreen == TRUE)
		{
			switch(LCD_Counter)
			{
			case 0:
				LCD_Counter++;
				break;
			case 1:
				LCD_Counter++;
				break;
			case 2:
				LCD_Counter++;
				break;
			case 3:
				LCD_enuClearScreenAsync(DummyCB);
				LCD_Counter++;
				break;
			case 4:
				LCD_Counter++;
				break;
			case 5 :
				LCD_enuSetCursorAsync  (LCD_enuFirstRow,LCD_enuColumn_4 ,DummyCB);
				LCD_Counter ++ ;
				break;
			case 6:
				LCD_Counter ++ ;
				break;
			case 7:
				LCD_Counter ++ ;
				break;
			case 8:
				LCD_Counter ++ ;
				break;
			case 9 :
				LCD_enuWriteStringAsync("STOPWATCH",DummyCB);
				LCD_Counter ++ ;
				break;
			case 10:
				LCD_Counter ++ ;
				break;
			case 11:
				LCD_Counter ++ ;
				break;
			case 12:
				LCD_Counter ++ ;
				break;
			case 13:
				LCD_Counter ++ ;
				break;
			case 14:
				LCD_Counter ++ ;
				break;
			case 15:
				LCD_Counter ++ ;
				break;
			case 16:
				LCD_Counter ++ ;
				break;
			case 17:
				LCD_Counter ++ ;
				break;
			case 18:
				LCD_Counter ++ ;
				break;
			case 19 :
				LCD_enuSetCursorAsync  (LCD_enuSecondRow,LCD_enuColumn_3 ,DummyCB);
				LCD_Counter ++ ;
				break;
			case 20 :
				LCD_enuWriteStringAsync("00:",DummyCB);
				LCD_Counter ++ ;
				break;
			case 21:
				LCD_Counter ++ ;
				break;
			case 22:
				LCD_Counter ++ ;
				break;
			case 23:
				LCD_Counter ++ ;
				break;
			case 24 :
				LCD_enuWriteStringAsync("00:",DummyCB);
				LCD_Counter ++ ;
				break;
			case 25:
				LCD_Counter ++ ;
				break;
			case 26:
				LCD_Counter ++ ;
				break;
			case 27:
				LCD_Counter ++ ;
				break;
			case 28 :
				LCD_enuWriteStringAsync("00:",DummyCB);
				LCD_Counter ++ ;
				break;
			case 29:
				LCD_Counter ++ ;
				break;
			case 30:
				LCD_Counter ++ ;
				break;
			case 31:
				LCD_Counter ++ ;
				break;
			case 32 :
				LCD_enuWriteStringAsync("000",DummyCB);
				LCD_Counter ++ ;
				break;
			case 33:
				LCD_Counter ++ ;
				break;
			case 34:
				LCD_Counter ++ ;
				break;
			case 35:
				LCD_Counter = 0 ;
				printEntireScreen = FALSE ;
				break;
			}
		}
		/************************************************************************************/
		/* 	The following part checks if We are ready for receiving a new button request or
			busy handling a current request */
		/************************************************************************************/

		/* Check if the previous button request is handled and We are ready for receving a new request or not */
		if( buttonHandled == TRUE)
		{
			MUSART_enuRecieveBufferAsync(USART_1,stopwatchRecivedMessage,1,receiveCallback);	
			 //USART_RXBufferAsyncZC(StopwatchRecived_Bytes);
		}
		else
		{
			/* Do Nothing till the current request is handled */
		}

		/************************************************************************************/
		/* 	The following part checks if We received a new message (a new button press) */
		/************************************************************************************/

		/* If a message is received go on handle it */
		if(receiveFlag == TRUE)
		{
			/* Lower the flag so We could be able to receive the next request once the current one is handled */
			receiveFlag = FALSE;

			u8 receivedButton = 0;
			receivedButton = Decryption(stopwatchRecivedMessage[0]);

			/* Check whether the recevied data is correct and We can deal with its content
				or it is corrupted so ignore it */
			if(receivedButton != CORRUPTED_MESSAGE)
			{
				/* Check which button the user had pressed to react upon it */
				switch (receivedButton)
				{
				case MODE_SWITCH_VALUE :
					if(EditMode == NOT_ACTIVATED)
					{   

						// if (Mode == CLOCK_MODE)
						// {
						// 	Mode = STOPWATCH_MODE;
						// 	previousMode = CLOCK_MODE;
						// }
						// else if (Mode == STOPWATCH_MODE)
						// {
							Mode = CLOCK_MODE;
							previousMode = STOPWATCH_MODE;
							clearOnce = FALSE ;
						// }
						printCounter = 0;
						LCD_Counter = 0;
						stopwatchRecivedMessage[0]= 0 ; 
					}
					break;
				case START_SWITCH_VALUE :
					startFlag = TRUE;
					break;
				case STOP_SWITCH_VALUE :
					startFlag = FALSE;
					printCounter = 0;
					break;
				case RESET_SWITCH_VALUE :
					LCD_enuClearScreenAsync(DummyCB);
					resetFlag = TRUE ;
					startFlag = FALSE;
					printEntireScreen =TRUE ;
					LCD_Counter  = 0;
					printCounter = 0;
					hours   = 0;
					minutes = 0;
					seconds = 0;
					milliseconds = 0;
					break;

				}

				/* After handling the latest request, raise this flag to make
					the driver ready to receive a new button request */
				//requestHandled = TRUE;
				buttonHandled = TRUE;
			}
			else
			{
				/* If the data is corrupted raise this flag to make the driver ready to receive a new button request */
				//requestHandled = TRUE;
			 	buttonHandled = TRUE;
			}
		}
		else
		{
			/* Do Nothing till a message is received thorugh UART asking for a mission */
		}
	}
}


/* We need to create a flag that give us an info whether we already displayed the Date and time and 
	We only need to update certain digits OR we need to clear and display from the beginning*/
