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
static u32  entryCounter    = 0;
u32  S_LCD_Counter          = 0;
u32  S_printCounter         = 0;

/* Variables related to the date and time. Initially We are setting them as follows */
u8  S_hours   = 0;
u8  S_minutes = 0;
u8  S_seconds = 0;
u16 S_milliseconds = 0;


extern MODES Mode ;

extern MODES previousMode ;

extern EDITMODES EditMode;

u8 printEntireScreen = TRUE;

u8 startFlag = FALSE ;

u8 clearOnce = FALSE ;

u8  stopwatchNotStarted = FALSE ;

/************************************************************************************/
/*							Static Functions' Implementation						*/
/************************************************************************************/


/**
 *@brief : Dummy function that passed to any Asynchronus function.
 *@param : void.
 *@return: void.
 */
static void DummyCB(void)
{

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

	/************************************************************************************/
	/* 			The following part updates the date and time every 1 seconds 			*/
	/************************************************************************************/
	if(startFlag == TRUE && entryCounter %5 == 0 && entryCounter !=0)
	{ 
		S_milliseconds +=100;

		if(S_milliseconds == 1000)
		{
			S_milliseconds =0;
			/* Every 1 second update the seconds variable */
			S_seconds++;
			if(S_seconds == 60)
			{
				/* If 60 seconds passed reset the seconds digits and increment the minutes digits by one */
				S_seconds = 0;
				S_minutes++;
				if(S_minutes == 60)
				{
					/* If 60 minutes passed reset the minutes digits and increment the hours digits by one */
					S_minutes = 0;
					S_hours++;
					if (S_hours == 100)
					{
						S_hours=0;
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
		switch(S_printCounter)
		{
		case 0:
			if(clearOnce == FALSE)
			{
				LCD_enuClearScreenAsync(DummyCB);
				clearOnce = TRUE ;
			}
			S_printCounter ++ ;
			break ;
		case 1 :
			S_printCounter ++ ;
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
			S_printCounter ++;
			break;
		case 3:
			S_printCounter++;
			break;
		case 4 :
			if (previousMode == CLOCK_MODE)
			{
				LCD_enuWriteStringAsync("STOPWATCH",DummyCB);
			}
			else
			{
				if (S_hours < 10)
				{
					LCD_enuWriteStringAsync("0",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(S_hours,DummyCB);
				}
			}
			S_printCounter ++;
			break;
		case 5:
			S_printCounter ++;
			break;
		case 6:
			S_printCounter ++;
			break;
		case 7:
			S_printCounter ++;
			break;
		case 8:
			S_printCounter ++;
			break;
		case 9:
			if (previousMode == CLOCK_MODE)
			{
				LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_3,DummyCB);
			}
			else
			{
				if(S_hours < 10)
				{
					LCD_enuWriteNumberAsync(S_hours,DummyCB);
				}
				else
				{

				}
			}
			S_printCounter ++;
			break;
		case 10:
			S_printCounter ++;
			break;
		case 11:
			if (previousMode == CLOCK_MODE)
			{
				if (S_hours < 10)
				{
					LCD_enuWriteStringAsync("0",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(S_hours,DummyCB);
				}
			}
			else
			{
				LCD_enuWriteStringAsync(":",DummyCB);
			}
			S_printCounter ++;
			break;
		case 12:
			S_printCounter ++;
			break;
		case 13:
			if (previousMode == CLOCK_MODE)
			{
				if(S_hours < 10)
				{
					LCD_enuWriteNumberAsync(S_hours,DummyCB);
				}
				else
				{

				}
			}
			else
			{
				if (S_minutes < 10)
				{
					LCD_enuWriteStringAsync("0",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(S_minutes,DummyCB);
				}
			}
			S_printCounter ++;
			break;
		case 14:
			S_printCounter ++;
			break;
		case 15:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			S_printCounter ++;
			break;
		case 16:
			S_printCounter ++;
			break;
		case 17:
			if(S_milliseconds == 0)
			{
				LCD_enuWriteStringAsync("000",DummyCB);
			}
			else
			{
				LCD_enuWriteNumberAsync(S_milliseconds,DummyCB);
			}
			S_printCounter ++;
			break;
		case 18:
			S_printCounter ++;
			break;
		case 19 :
			if (previousMode == CLOCK_MODE)
			{
				LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_5,DummyCB);
			}
			else
			{
				if (S_minutes < 10)
				{
					LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_7,DummyCB);
				}
				else
				{
					LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_8,DummyCB);
				}
			}
			S_printCounter ++;
			break;
		case 20:
			S_printCounter ++;
			break;
		case 21:
			if (previousMode == CLOCK_MODE)
			{
				LCD_enuWriteStringAsync(":",DummyCB);
			}
			else
			{
				if(S_minutes < 10)
				{
					LCD_enuWriteNumberAsync(S_minutes,DummyCB);
				}
				else
				{

				}
			}
			S_printCounter ++;
			break;
		case 22:
			S_printCounter ++;
			break;
		case 23:
			if (previousMode == CLOCK_MODE)
			{
				if (S_minutes < 10)
				{
					LCD_enuWriteStringAsync("0",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(S_minutes,DummyCB);
				}
			}
			else
			{
				LCD_enuWriteStringAsync(":",DummyCB);
			}
			S_printCounter ++;
			break;
		case 24:
			S_printCounter ++;
			break;
		case 25:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			S_printCounter ++;
			break;
		case 26:
			S_printCounter ++;
			break;
		case 27:
			if(S_milliseconds == 0)
			{
				LCD_enuWriteStringAsync("000",DummyCB);
			}
			else
			{
				LCD_enuWriteNumberAsync(S_milliseconds,DummyCB);
			}
			S_printCounter ++;
			break;
		case 28:
			S_printCounter ++;
			break;
		case 29 :
			if (previousMode == CLOCK_MODE)
			{
				if (S_minutes < 10)
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
			S_printCounter ++;
			break;
		case 30:
			S_printCounter ++;
			break;
		case 31:
			if (previousMode == CLOCK_MODE)
			{
				if(S_minutes < 10)
				{
					LCD_enuWriteNumberAsync(S_minutes,DummyCB);
				}
				else
				{

				}
			}
			else
			{
				if (S_seconds < 10)
				{
					LCD_enuWriteStringAsync("0",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(S_seconds,DummyCB);
				}
			}
			S_printCounter ++;
			break;
		case 32:
			S_printCounter ++;
			break;
		case 33:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			S_printCounter ++;
			break;
		case 34:
			S_printCounter ++;
			break;
		case 35:
			if(S_milliseconds == 0)
			{
				LCD_enuWriteStringAsync("000",DummyCB);
			}
			else
			{
				LCD_enuWriteNumberAsync(S_milliseconds,DummyCB);
			}
			S_printCounter ++;
			break;
		case 36:
			S_printCounter ++;
			break;
		case 37:
			if (previousMode == CLOCK_MODE)
			{
				LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_8,DummyCB);
			}
			else
			{
				if (S_seconds < 10)
				{
					LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_9,DummyCB);
				}
				else
				{
					LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_11,DummyCB);
				}
			}
			S_printCounter ++;
			break;
		case 38:
			S_printCounter ++;
			break;
		case 39:
			if (previousMode == CLOCK_MODE)
			{
				LCD_enuWriteStringAsync(":",DummyCB);
			}
			else
			{
				if(S_seconds < 10)
				{
					LCD_enuWriteNumberAsync(S_seconds,DummyCB);
				}
				else
				{

				}
			}
			S_printCounter ++;
			break;
		case 40:
			S_printCounter ++;
			break;
		case 41:
			if (previousMode == CLOCK_MODE)
			{
				if (S_seconds < 10)
				{
					LCD_enuWriteStringAsync("0",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(S_seconds,DummyCB);
				}
			}
			else
			{
				LCD_enuWriteStringAsync(":",DummyCB);
			}
			S_printCounter ++;
			break;
		case 42:
			S_printCounter ++;
			break;
		case 43:
			if (previousMode == CLOCK_MODE)
			{
				if(S_seconds < 10)
				{
					LCD_enuWriteNumberAsync(S_seconds,DummyCB);
				}
				else
				{

				}
			}
			else
			{
				if(S_milliseconds == 0)
				{
					LCD_enuWriteStringAsync("000",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(S_milliseconds,DummyCB);
				}
			}
			S_printCounter ++;
			break;
		case 44:
			S_printCounter++;
			break;
		case 45:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			S_printCounter ++;
			break;
		case 46:
			S_printCounter ++;
			break;
		case 47:
			if(S_milliseconds == 0)
			{
				LCD_enuWriteStringAsync("000",DummyCB);
			}
			else
			{
				LCD_enuWriteNumberAsync(S_milliseconds,DummyCB);
			}
			S_printCounter ++;
			break;
		case 48:
			S_printCounter ++;
			break;
		case 49 :
			if (previousMode == CLOCK_MODE)
			{
				LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_11,DummyCB);
			}
			else
			{

			}
			S_printCounter ++;

			break;
		case 50:
			S_printCounter ++;
			break;
		case 51 :
			if (previousMode == CLOCK_MODE)
			{
				LCD_enuWriteStringAsync(":",DummyCB);
			}
			else
			{

			}
			S_printCounter ++;
			break;
		case 52:
			S_printCounter ++;
			break;
		case 53:
			if (previousMode == CLOCK_MODE)
			{
				if(S_milliseconds == 0)
				{
					LCD_enuWriteStringAsync("000",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(S_milliseconds,DummyCB);
				}
				//previousMode = STOPWATCH_MODE;
			}
			else
			{

			}
			S_printCounter ++;
			break;
		case 54 :
			S_printCounter ++;
			break;
		case 55:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			S_printCounter ++;
			break;
		case 56:
			S_printCounter ++;
			break;
		case 57:
			if(S_milliseconds == 0)
			{
				LCD_enuWriteStringAsync("000",DummyCB);
			}
			else
			{
				LCD_enuWriteNumberAsync(S_milliseconds,DummyCB);
			}
			S_printCounter ++;
			break;
		case 58:
			S_printCounter ++;
			break;
		case 59:
			S_printCounter ++;
			break;
		case 60:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			S_printCounter ++;
			break;
		case 61:
			S_printCounter ++;
			break;
		case 62:
			if(S_milliseconds == 0)
			{
				LCD_enuWriteStringAsync("000",DummyCB);
			}
			else
			{
				LCD_enuWriteNumberAsync(S_milliseconds,DummyCB);
			}
			S_printCounter ++;
			break;
		case 63:
			S_printCounter ++;
			break;
		case 64:
			S_printCounter ++;
			break;
		case 65:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			S_printCounter ++;
			break;
		case 66:
			S_printCounter ++;
			break;
		case 67:
			if(S_milliseconds == 0)
			{
				LCD_enuWriteStringAsync("000",DummyCB);
			}
			else
			{
				LCD_enuWriteNumberAsync(S_milliseconds,DummyCB);
			}
			S_printCounter ++;
			break;
		case 68:
			S_printCounter ++;
			break;
		case 69:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			S_printCounter ++;
			break;
		case 70:
			S_printCounter ++;
			break;
		case 71:
			if(S_milliseconds == 0)
			{
				LCD_enuWriteStringAsync("000",DummyCB);
			}
			else
			{
				LCD_enuWriteNumberAsync(S_milliseconds,DummyCB);
			}
			S_printCounter ++;
			break;
		case 72:
			S_printCounter ++;
			break;
		case 73:
			LCD_enuSetCursorAsync(LCD_enuSecondRow, LCD_enuColumn_9, DummyCB);
			S_printCounter ++;
			break;
		case 74:
			S_printCounter ++;
			break;
		case 75:
			if (S_seconds < 10)
			{
				LCD_enuWriteStringAsync("0", DummyCB);
			}
			else
			{
				LCD_enuWriteNumberAsync(S_seconds, DummyCB);
			}
			S_printCounter ++;
			break;
		case 76:
			S_printCounter ++;
			break;
		case 77:
			if (S_seconds < 10)
			{
				LCD_enuWriteNumberAsync(S_seconds, DummyCB);
			}
			else
			{

			}
			S_printCounter ++;
			break;
		case 78:
			S_printCounter ++;
			break;
		case 79:
			LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_12,DummyCB);
			S_printCounter ++;
			break;
		case 80:
			S_printCounter ++;
			break;
		case 81:
			if(S_milliseconds == 0)
			{
				LCD_enuWriteStringAsync("000",DummyCB);
			}
			else
			{
				LCD_enuWriteNumberAsync(S_milliseconds,DummyCB);
			}
			S_printCounter ++;
			break;
		case 82:
			S_printCounter ++;
			break;
		case 83:
			S_printCounter ++;
			break;
		case 84:
			S_printCounter =0;
			break;
		}
	}
	/************************************************************************************/
	/* 	The following part checks on the selected mode, if it is the CLOCK_MODE, it Will
		display Date and Time after each update */
	/************************************************************************************/
	if(Mode == STOPWATCH_MODE)
	{  
		if(printEntireScreen == TRUE || ((startFlag == FALSE) && (stopwatchNotStarted == TRUE)))
		{
			switch(S_LCD_Counter)
			{
			case 0:
				S_LCD_Counter++;
				break;
			case 1:
				S_LCD_Counter++;
				break;
			case 2:
				S_LCD_Counter++;
				break;
			case 3:
				LCD_enuClearScreenAsync(DummyCB);
				S_LCD_Counter++;
				break;
			case 4:
				S_LCD_Counter++;
				break;
			case 5 :
				LCD_enuSetCursorAsync  (LCD_enuFirstRow,LCD_enuColumn_4 ,DummyCB);
				S_LCD_Counter ++ ;
				break;
			case 6:
				S_LCD_Counter ++ ;
				break;
			case 7:
				S_LCD_Counter ++ ;
				break;
			case 8:
				S_LCD_Counter ++ ;
				break;
			case 9 :
				LCD_enuWriteStringAsync("STOPWATCH",DummyCB);
				S_LCD_Counter ++ ;
				break;
			case 10:
				S_LCD_Counter ++ ;
				break;
			case 11:
				S_LCD_Counter ++ ;
				break;
			case 12:
				S_LCD_Counter ++ ;
				break;
			case 13:
				S_LCD_Counter ++ ;
				break;
			case 14:
				S_LCD_Counter ++ ;
				break;
			case 15:
				S_LCD_Counter ++ ;
				break;
			case 16:
				S_LCD_Counter ++ ;
				break;
			case 17:
				S_LCD_Counter ++ ;
				break;
			case 18:
				S_LCD_Counter ++ ;
				break;
			case 19 :
				LCD_enuSetCursorAsync  (LCD_enuSecondRow,LCD_enuColumn_3 ,DummyCB);
				S_LCD_Counter ++ ;
				break;
			case 20 :
				LCD_enuWriteStringAsync("00:",DummyCB);
				S_LCD_Counter ++ ;
				break;
			case 21:
				S_LCD_Counter ++ ;
				break;
			case 22:
				S_LCD_Counter ++ ;
				break;
			case 23:
				S_LCD_Counter ++ ;
				break;
			case 24 :
				LCD_enuWriteStringAsync("00:",DummyCB);
				S_LCD_Counter ++ ;
				break;
			case 25:
				S_LCD_Counter ++ ;
				break;
			case 26:
				S_LCD_Counter ++ ;
				break;
			case 27:
				S_LCD_Counter ++ ;
				break;
			case 28 :
				LCD_enuWriteStringAsync("00:",DummyCB);
				S_LCD_Counter ++ ;
				break;
			case 29:
				S_LCD_Counter ++ ;
				break;
			case 30:
				S_LCD_Counter ++ ;
				break;
			case 31:
				S_LCD_Counter ++ ;
				break;
			case 32 :
				LCD_enuWriteStringAsync("000",DummyCB);
				S_LCD_Counter ++ ;
				break;
			case 33:
				S_LCD_Counter ++ ;
				break;
			case 34:
				S_LCD_Counter ++ ;
				break;
			case 35:
				S_LCD_Counter = 0 ;
				printEntireScreen = FALSE ;
				stopwatchNotStarted = FALSE;
				break;
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
