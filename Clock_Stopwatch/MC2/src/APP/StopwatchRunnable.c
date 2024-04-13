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


#define UP_SWITCH_VALUE					0x01
#define DOWN_SWITCH_VALUE				0x02
#define RIGHT_SWITCH_VALUE				0x03
#define LEFT_SWITCH_VALUE				0x04
#define OK_SWITCH_VALUE					0x05
#define RESET_SWITCH_VALUE				0x06
#define MODE_SWITCH_VALUE				0x07
#define STOP_SWITCH_VALUE				0x08
#define START_SWITCH_VALUE				0x09
#define EDIT_SWITCH_VALUE				0x0A

#define CORRUPTED_MESSAGE				0xFF

#define HOURS_TENS_POSITION				  18
#define HOURS_UNITS_POSITION			  19
#define MINUTES_TENS_POSITION			  21
#define MINUTES_UNITS_POSITION		  22
#define SECONDS_TENS_POSITION			  24
#define SECONDS_UNITS_POSITION		  25
#define MSECONDS_HUNDREDS_POSITION	27

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

static OKSTATE OKState = NOT_PRESSED;

static LCD_enuRowNumber_t CurrentRow = LCD_enuFirstRow;

static LCD_enuColumnNumber_t CurrentCol = LCD_enuColumn_1;

static u8 CursorPos = 0;

static u8 receiveFlag = FALSE;

static u8 requestHandled = TRUE;

static u8 printEntireScreen = TRUE;

static u8 startFlag = TRUE ;

static u8 resetFlag = FALSE ;

static u8 buttonHandled = TRUE ;

u8 stopwatchRecivedMessage[1]  = {0};


/************************************************************************************/
/*							Static Functions' Implementation						*/
/************************************************************************************/


/**
 *@brief : Function that decryptes the received encrypted message from the UART.
 *@param : Rececived message.
 *@return: Data that is extracted from the recevied encrypted message.
 */
static u8 Decryption(u8 value)                                                                          
{
	/* Extracting the Checksum bits from the received encrypted message */
	u8 CheckSumBits = (value) & 0x0F;
	/* Extracting the data bits from the received encrypted message */
	u8 Data = (value >> 4);

	/* Check if the received message is correct and not corrupted */
	if((CheckSumBits ^ value) == 0x0F)
	{
		/* If the received message is correct and not corrupted, return it */
	}
	else
	{
		/* If the received message is corrupted, return an info indicates that */
		Data = CORRUPTED_MESSAGE;
	}
	return Data;
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
}


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
	static u8 minutes = 9;
	static u8 seconds = 0;
	static u16 milliseconds = 0;


	/************************************************************************************/
	/* 			The following part updates the date and time every 1 seconds 			*/
	/************************************************************************************/
	if(startFlag == TRUE && entryCounter %5 == 0 && printEntireScreen == FALSE && entryCounter !=0)
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
	if(startFlag == TRUE && printEntireScreen == FALSE )
	{
		entryCounter ++ ;
	}
	if( Mode == STOPWATCH_MODE && printEntireScreen == FALSE &&  startFlag == TRUE)
	{
		switch(printCounter)
		{
		case 0 :
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
		case 1:
			printCounter++;
			break;
		case 2 :
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
		case 3:
			printCounter ++;
			break;
		case 4:
			printCounter ++;
			break;
		case 5:
			printCounter ++;
			break;
		case 6:
			printCounter ++;
			break;
		case 7:
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
		case 8:
			printCounter ++;
			break;
		case 9:
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
		case 10:
			printCounter ++;
			break;
		case 11:
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
		case 12:
			printCounter ++;
			break;
		case 13:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			printCounter ++;
			break;
		case 14:
			printCounter ++;
			break;
		case 15:
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
		case 16:
			printCounter ++;
			break;
		case 17 :
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
		case 18:
			printCounter ++;
			break;
		case 19:
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
		case 20:
			printCounter ++;
			break;
		case 21:
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
		case 22:
			printCounter ++;
			break;
		case 23:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			printCounter ++;
			break;
		case 24:
			printCounter ++;
			break;
		case 25:
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
		case 26:
			printCounter ++;
			break;
		case 27 :
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
		case 28:
			printCounter ++;
			break;
		case 29:
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
		case 30:
			printCounter ++;
			break;
		case 31:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			printCounter ++;
			break;
		case 32:
			printCounter ++;
			break;
		case 33:
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
		case 34:
			printCounter ++;
			break;
		case 35:
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
		case 36:
			printCounter ++;
			break;
		case 37:
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
		case 38:
			printCounter ++;
			break;
		case 39:
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
		case 40:
			printCounter ++;
			break;
		case 41:
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
		case 42:
			printCounter++;
			break;
		case 43:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			printCounter ++;
			break;
		case 44:
			printCounter ++;
			break;
		case 45:
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
		case 46:
			printCounter ++;
			break;
		case 47 :
			if (previousMode == CLOCK_MODE)
			{
				LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_11,DummyCB);
			}
			else
			{

			}
			printCounter ++;

			break;
		case 48:
			printCounter ++;
			break;
		case 49 :
			if (previousMode == CLOCK_MODE)
			{
				LCD_enuWriteStringAsync(":",DummyCB);
			}
			else
			{

			}
			printCounter ++;
			break;
		case 50:
			printCounter ++;
			break;
		case 51:
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
		case 52 :
			printCounter ++;
			break;
		case 53:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			printCounter ++;
			break;
		case 54:
			printCounter ++;
			break;
		case 55:
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
		case 56:
			printCounter ++;
			break;
		case 57:
			printCounter ++;
			break;
		case 58:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			printCounter ++;
			break;
		case 59:
			printCounter ++;
			break;
		case 60:
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
		case 61:
			printCounter ++;
			break;
		case 62:
			printCounter ++;
			break;
		case 63:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			printCounter ++;
			break;
		case 64:
			printCounter ++;
			break;
		case 65:
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
		case 66:
			printCounter ++;
			break;
		case 67:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			printCounter ++;
			break;
		case 68:
			printCounter ++;
			break;
		case 69:
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
		case 70:
			printCounter ++;
			break;
		case 71:
			LCD_enuSetCursorAsync(LCD_enuSecondRow, LCD_enuColumn_9, DummyCB);
			printCounter ++;
			break;
		case 72:
			printCounter ++;
			break;
		case 73:
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
		case 74:
			printCounter ++;
			break;
		case 75:
			if (seconds < 10)
			{
				LCD_enuWriteNumberAsync(seconds, DummyCB);
			}
			else
			{

			}
			printCounter ++;
			break;
		case 76:
			printCounter ++;
			break;
		case 77:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			printCounter ++;
			break;
		case 78:
			printCounter ++;
			break;
		case 79:
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
		case 80:
			printCounter ++;
			break;
		case 81:
			printCounter ++;
			break;
		case 82:
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
				//LCD_enuWriteStringAsync("00:",DummyCB);
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
				//LCD_enuWriteStringAsync("00:",DummyCB);
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
				//LCD_enuWriteStringAsync("00:",DummyCB);
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
				//LCD_enuWriteStringAsync("000",DummyCB);
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
			 buttonHandled = FALSE;	 
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
				case UP_SWITCH_VALUE:
					/* In case of the user entered the edit mode but still does not pressed the OK
						button, he/she will be only able to navigate through the display */
					if((EditMode == ACTIVATED) && (OKState == NOT_PRESSED))
					{
						/* If the cursor is already in the first row there is no hidden place above that :) */
						if (CurrentRow == LCD_enuFirstRow)
						{
							/* Do Nothing You already on the first row */
						}
						else
						{
							/* If You are on the second row, the cursor should then go up to the first row */
							CurrentRow = LCD_enuFirstRow;
						}
						LCD_enuSetCursorAsync (CurrentRow, CurrentCol, DummyCB);
					}
					/* In case of the user entered the edit mode and pressed the OK button, he/she will
						be able to edit the displayed date and time */
					else if ((EditMode == ACTIVATED) && (OKState == FIRST_PRESSED))
					{
						/* Calculating the position where the user is setting the cursor on to make sure
							that he is editing only the date and time digits*/
						CursorPos = (CurrentCol) + (CurrentRow * 16);

						/* Check on the position of the cursor */
						switch (CursorPos)
						{
						/* The cursor now is at the position of the Days' units */
						case HOURS_TENS_POSITION:
							if ((hours >= 0) && (hours <= 89))
							{
								hours += 10;
							}
							else if ((hours >= 90))
							{

							}
							break;
						case HOURS_UNITS_POSITION:
							if((hours >= 0) && (hours <= 98))
							{
								hours++;
							}
							else if (hours == 99)
							{
								/* Do Nothing as 99 is the last hour could be displayed in the hours digits */
							}
							break;
						case MINUTES_TENS_POSITION:
							if((minutes >= 0) && (minutes <= 49))
							{
								minutes += 10;
							}
							else if (minutes > 50)
							{

							}
							break;
						case MINUTES_UNITS_POSITION:
							if((minutes >= 0) && (minutes <= 58))
							{
								minutes++;
							}
							else if (minutes == 59)
							{
								/* Do Nothing as 59 is the last minutes could be displayed in the minutes digits */
							}
							break;
						case SECONDS_TENS_POSITION:
							if((seconds >= 0) && (seconds <= 49))
							{
								seconds += 10;
							}
							else if (seconds> 50)
							{

							}
							break;
						case SECONDS_UNITS_POSITION:
							if((seconds >= 0) && (seconds <= 58))
							{
								seconds++;
							}
							else if (seconds == 59)
							{
								/* Do Nothing as 59 is the last seconds could be displayed in the seconds digits */
							}
							break;
						case MSECONDS_HUNDREDS_POSITION:
							if(milliseconds >= 0 && milliseconds <=800)
							{
								milliseconds +=100 ;
							}
							else
							{
								/* Do Nothing as 900 is the last milliseconds could be displayed in the milliseconds digits */
							}
							break;
						default:   
							/* You are in a position that you can not edit the data in it,
								so your request is ignored*/
							break;
						}
					}
					break;
				case DOWN_SWITCH_VALUE:
					/* In case of the user entered the edit mode but still does not pressed the OK
						button, he/she will be only able to navigate through the display */
					if((EditMode == ACTIVATED) && (OKState == NOT_PRESSED))
					{
						/* If the cursor is already in the last row there is no hidden place under that :) */
						if (CurrentRow == LCD_enuSecondRow)
						{
							/* Do Nothing You already on the last row */
						}
						else
						{
							/* If You are on the first row, the cursor should then go down to the second row */
							CurrentRow = LCD_enuSecondRow;
						}
						LCD_enuSetCursorAsync (CurrentRow,CurrentCol,DummyCB);
					}
					else if (EditMode == ACTIVATED && OKState == FIRST_PRESSED )
					{
						/* Calculating the position where the user is setting the cursor on to make sure
							that he is editing only the date and time digits*/
						CursorPos = (CurrentCol) + (CurrentRow * 16);

						/* Check on the position of the cursor */
						switch (CursorPos)
						{
						case HOURS_TENS_POSITION:
							if((hours >= 0) && (hours <= 9))
							{

							}
							else if (hours>=10 && hours <=99)
							{
								hours -= 10;
							}
							break;
						case HOURS_UNITS_POSITION:
							if(hours % 10 == 0)
							{

							}
							else
							{
								hours--;
							}
							break;
						case MINUTES_TENS_POSITION:
							if((minutes >= 0) && (minutes <= 9))
							{

							}
							else
							{
								minutes -= 10;
							}
							break;
						case MINUTES_UNITS_POSITION:
							if((minutes % 10) == 0)
							{

							}
							else
							{
								minutes--;
							}
							break;
						case SECONDS_TENS_POSITION:
							if((seconds >= 0) && (seconds <= 9))
							{

							}
							else
							{
								seconds -= 10;
							}
							break;
						case SECONDS_UNITS_POSITION:
							if((seconds % 10) == 0)
							{

							}
							else
							{
								seconds--;
							}
							break;
						case MSECONDS_HUNDREDS_POSITION:
							if(milliseconds == 0)
							{

							}
							else
							{
								milliseconds -=100;
							}
							break;
						default:
							/* You are in a position that you can not edit the data in it,
								so your request is ignored*/
							break;
						}
					}
					break;
				case RIGHT_SWITCH_VALUE:
					/* In case of the user entered the edit mode, he/she will be able to navigate through the display */
					if(EditMode == ACTIVATED)
					{
						/* If the cursor is already in the last column, there is no hidden place to the right of that position :) */
						if(CurrentCol == LCD_enuColumn_16 || CurrentCol == LCD_enuColumn_32)
						{
							/* Do Nothing You already on the last column */
						}
						else
						{
							/* If You are on the another column rather the last one, the cursor should
								then go right to the next column in the same row */
							LCD_enuSetCursorAsync(CurrentRow, (CurrentCol + 1), DummyCB);
						}
					}
					break;
				case LEFT_SWITCH_VALUE:
					/* In case of the user entered the edit mode, he/she will be able to navigate through the display */
					if(EditMode == ACTIVATED)
					{
						/* If the cursor is already in the first column, there is no hidden place to the left of that position :) */
						if(CurrentCol == LCD_enuColumn_1 || CurrentCol == LCD_enuColumn_17)
						{
							/* Do Nothing You already on the first column */
						}
						else
						{
							/* If You are on the another column rather the first one, the cursor should
								then go left to the next column in the same row */
							LCD_enuSetCursorAsync (CurrentRow, (CurrentCol - 1), DummyCB);
						}
					}
					break;
				case OK_SWITCH_VALUE:
					if(EditMode == ACTIVATED)
					{
						if (OKState == NOT_PRESSED)
						{
							OKState = FIRST_PRESSED;
						}
						else if (OKState == FIRST_PRESSED)
						{
							OKState = NOT_PRESSED;
						}
					}
					break;
				case MODE_SWITCH_VALUE :
					if(EditMode == NOT_ACTIVATED)
					{
						if (Mode == CLOCK_MODE)
						{
							Mode = STOPWATCH_MODE;
							previousMode = CLOCK_MODE;
						}
						else if (Mode == STOPWATCH_MODE)
						{
							Mode = CLOCK_MODE;
							previousMode = STOPWATCH_MODE;
						}
					}
					break;
				case EDIT_SWITCH_VALUE :
					if(EditMode == NOT_ACTIVATED)
					{
						EditMode = ACTIVATED ;
						LCD_enuSetCursorAsync  (LCD_enuFirstRow,LCD_enuColumn_1 ,DummyCB);
					}
					else if (EditMode == ACTIVATED)
					{
						EditMode = NOT_ACTIVATED ;
					}
					break;

				case START_SWITCH_VALUE :
					startFlag = TRUE;
					break;
				case STOP_SWITCH_VALUE :
					startFlag = FALSE;
					break;
				case RESET_SWITCH_VALUE :
					resetFlag = TRUE ;
					startFlag = FALSE;

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
