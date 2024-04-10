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
#include "HAL/HLED/HLED_interface.h"


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

#define DAY_TENS_POSITION				6
#define DAY_UNITS_POSITION				7
#define MONTHS_TENS_POSITION			9
#define MONTHS_UNITS_POSITION			10
#define YEARS_THOUSANDS_POSITION		12
#define YEARS_HUNDREDS_POSITION			13
#define YEARS_TENS_POSITION				14
#define YEARS_UNITS_POSITION			15

#define HOURS_TENS_POSITION				24
#define HOURS_UNITS_POSITION			25
#define MINUTES_TENS_POSITION			27
#define MINUTES_UNITS_POSITION			28
#define SECONDS_TENS_POSITION			30
#define SECONDS_UNITS_POSITION			31



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

MODES Mode = CLOCK_MODE;

static EDITMODES EditMode = NOT_ACTIVATED;

static OKSTATE OKState = NOT_PRESSED;

static LCD_enuRowNumber_t CurrentRow = LCD_enuFirstRow;

static LCD_enuColumnNumber_t CurrentCol = LCD_enuColumn_1;

static u8 CursorPos = 0;

static u8 receiveFlag = FALSE;

static u8 requestHandled = TRUE;

static u8 printEntireScreen = TRUE;

static u8 dayPassed = FALSE ;



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
void clockRunnable(void)
{  
	static u32  entryCounter  = 1;
	static u32  LCD_Counter   = 0;
	static u32  printCounter  = 0;

	u8 RecivedMessage  = 0;

	/* Variables related to the date and time. Initially We are setting them as follows */
	static u8 hours   = 12;
	static u8 minutes = 0;
	static u8 seconds = 0;
	static u8 day = 1;
	static u8 month = 1;
	static u16 year = 2024;

	/************************************************************************************/
	/* 			The following part updates the date and time every 1 seconds 			*/
	/************************************************************************************/
	if((entryCounter % 10 == 0) && (entryCounter != 0))
	{    
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
			}
			if(hours == 24)
			{
				/* If 24 hours passed reset the hours digits and increment the Day digits by one */
				hours = 0;
				day++;
				dayPassed =TRUE;
			}
			
			/* If a total month passed increment the month digits and reset the days digits to one */
			if((month == 1 ||month == 3 ||month == 5 ||month == 7 ||month == 8 ||month == 10 ||month == 12) \
					&& (day == 31))
			{
				day = 1;
				month++;
			}
			else if ((month == 4 ||month == 6 ||month == 9 ||month == 11) && (day == 30))
			{
				day = 1;
				month++;
			}
			else if ((month == 2) && (day == 28))
			{
				day = 1;
				month++;
			}
			else
			{
				/* Do Nothing */
			}
			
			if(month == 13)
			{
				/* If 12 months passed, reset the months digits to 1 and increment the years digits by one */
				month = 1;
				year++;
			}
		}
	}
	entryCounter ++ ;
	if( Mode == CLOCK_MODE && printEntireScreen == FALSE)
		{   
			switch(printCounter)
			{
				case 0:
				LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_9,DummyCB); 
				printCounter ++;
				break;
				case 1:
				if (hours < 10)
				{
					LCD_enuWriteStringAsync("0",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(hours,DummyCB);
				}
				printCounter ++;
				break;
			case 2:
				if(hours < 10)
				{
					LCD_enuWriteNumberAsync(hours,DummyCB);
				}
				else
				{

				}
				printCounter ++ ;
				break;
				case 3:
				LCD_enuWriteStringAsync(":",DummyCB);
				printCounter ++;
				break;
				case 4:
				if (minutes < 10)
				{
					LCD_enuWriteStringAsync("0",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(minutes,DummyCB);
				}
				printCounter ++;
				break;
			case 5:
				if(minutes < 10)
				{
					LCD_enuWriteNumberAsync(minutes,DummyCB);
				}
				else
				{

				}
				printCounter ++ ;
				break;
				case 6:
				LCD_enuWriteStringAsync(":",DummyCB);
				printCounter ++;
				break;
				case 7:
				if (seconds < 10)
				{
					LCD_enuWriteStringAsync("0",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(seconds,DummyCB);
				}
				printCounter ++;
				break;
				case 8:
				if(seconds < 10)
				{
					LCD_enuWriteNumberAsync(seconds,DummyCB);
				}
				else
				{

				}
				printCounter ++ ;
				break;
				case 9:
				if (dayPassed == TRUE)
				{
					LCD_enuSetCursorAsync(LCD_enuFirstRow,LCD_enuColumn_7,DummyCB); 
				}
				else
				{
					LCD_enuSetCursorAsync(LCD_enuSecondRow,LCD_enuColumn_9,DummyCB); 
				}
				printCounter ++;
				break;
				case 10:
				if (dayPassed == TRUE)
				{  
					if(day < 10)
					{
					LCD_enuWriteStringAsync("0",DummyCB);
					}
					else
					{
						LCD_enuWriteNumberAsync(day,DummyCB);
					}
				}
				else
				{
					if(hours < 10)
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
				case 11:
				if (dayPassed == TRUE)
				{  
					if(day < 10)
					{
					LCD_enuWriteNumberAsync(day,DummyCB);
					}
					else
					{
						
					}
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

				case 12:
				if (dayPassed == TRUE)
				{
					LCD_enuWriteStringAsync("/",DummyCB);
				}
				else
				{
					LCD_enuWriteStringAsync(":",DummyCB);
				}
				printCounter ++;
				break;
				case 13:
				if (dayPassed == TRUE)
				{  
					if(month < 10)
					{
					LCD_enuWriteStringAsync("0",DummyCB);
					}
					else
					{
						LCD_enuWriteNumberAsync(month,DummyCB);
					}
				}
				else
				{
					if(minutes < 10)
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
				if (dayPassed == TRUE)
				{  
					if(month < 10)
					{
					LCD_enuWriteNumberAsync(month,DummyCB);
					}
					else
					{
						
					}
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
				case 15:
				if (dayPassed == TRUE)
				{
					LCD_enuWriteStringAsync("/",DummyCB);
				}
				else
				{
					LCD_enuWriteStringAsync(":",DummyCB);
				}
				printCounter ++;
				break;

				case 16:
				if (dayPassed == TRUE)
				{  
					if (year < 1000)
				{
					LCD_enuWriteStringAsync("0",DummyCB);
				}
				else if (year < 100)
				{
					LCD_enuWriteStringAsync("00",DummyCB);
				}
				else if (year < 10)
				{
					LCD_enuWriteStringAsync("000",DummyCB);
				}
				else if (year < 1)
				{
					LCD_enuWriteStringAsync("0000",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(year,DummyCB);
				}
				}
				else
				{
					if(seconds < 10)
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
				case 17:
				if (dayPassed == TRUE)
				{  
					if (year < 1000)
				{
					LCD_enuWriteNumberAsync(year,DummyCB);
				}
				else if (year < 100)
				{
					LCD_enuWriteNumberAsync(year,DummyCB);
				}
				else if (year < 10)
				{
					LCD_enuWriteNumberAsync(year,DummyCB);
				}
				else if (year < 1)
				{
					LCD_enuWriteNumberAsync(year,DummyCB);
				}
				else
				{
					
				}
					
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
				printCounter = 0;
				break;
			}
		}
	/************************************************************************************/
	/* 	The following part checks on the selected mode, if it is the CLOCK_MODE, it Will
		display Date and Time after each update */
	/************************************************************************************/
	if(Mode == CLOCK_MODE )
	{  
		if(printEntireScreen == TRUE)
		{

			switch(LCD_Counter)
			{
			case 0 :
				LCD_enuSetCursorAsync  (LCD_enuFirstRow,LCD_enuColumn_1 ,DummyCB);
				LCD_Counter ++ ;
				break;
			case 1 :
				LCD_enuWriteStringAsync("Date: ",DummyCB);
				LCD_Counter ++ ;
				break;
			case 2 :
				if (day < 10)
				{
				LCD_enuWriteStringAsync("0",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(day,DummyCB);
				}
				LCD_Counter ++ ;
				break;
			case 3:
				if (day < 10)
				{
				LCD_enuWriteNumberAsync(day,DummyCB);
				}
				else
				{
				 
				}
				LCD_Counter ++ ;
				break;
			case 4 :
				LCD_enuWriteStringAsync("/",DummyCB);
				LCD_Counter ++ ;
				break;
			case 5 :
				if (month < 10)
				{
				LCD_enuWriteStringAsync("0",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(month,DummyCB);
				}
				LCD_Counter ++ ;
				break;
			case 6:
				if (month < 10)
				{
				LCD_enuWriteNumberAsync(month,DummyCB);
				}
				else
				{
				 
				}
				LCD_Counter ++ ;
				break;
			case 7 :
				LCD_enuWriteStringAsync("/",DummyCB);
				LCD_Counter ++ ;
				break;
			case 8 :
				if (year < 1000)
				{
					LCD_enuWriteStringAsync("0",DummyCB);
				}
				else if (year < 100)
				{
					LCD_enuWriteStringAsync("00",DummyCB);
				}
				else if (year < 10)
				{
					LCD_enuWriteStringAsync("000",DummyCB);
				}
				else if (year < 1)
				{
					LCD_enuWriteStringAsync("0000",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(year,DummyCB);
				}
				LCD_Counter ++ ;
				break;
			case 9 :
				if (year < 1000)
				{
					LCD_enuWriteNumberAsync(year,DummyCB);
				}
				else if (year < 100)
				{
					LCD_enuWriteNumberAsync(year,DummyCB);
				}
				else if (year < 10)
				{
					LCD_enuWriteNumberAsync(year,DummyCB);
				}
				else if (year < 1)
				{
					LCD_enuWriteNumberAsync(year,DummyCB);
				}
				else
				{
					
				}
				LCD_Counter ++ ;
				break;
			case 10 :
				LCD_enuSetCursorAsync  (LCD_enuSecondRow,LCD_enuColumn_1 ,DummyCB);
				LCD_Counter ++ ;
				break;
			case 11:
				LCD_enuWriteStringAsync("Time:   ",DummyCB);
				LCD_Counter ++ ;
				break;
			case 12:
				if (hours < 10)
				{
					LCD_enuWriteStringAsync("0",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(hours,DummyCB);
				}
				LCD_Counter ++ ;
				break;
			case 13:
				if(hours < 10)
				{
					LCD_enuWriteNumberAsync(hours,DummyCB);
				}
				else
				{

				}
				LCD_Counter ++ ;
				break;
			case 14 :
				LCD_enuWriteStringAsync(":",DummyCB);
				LCD_Counter ++ ;
				break;
			case 15 :
				if (minutes < 10)
				{
					LCD_enuWriteStringAsync("0",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(minutes,DummyCB);
				}
				LCD_Counter ++ ;
				break;
			case 16 :
				if(minutes < 10)
			{
				LCD_enuWriteNumberAsync(minutes,DummyCB);
			}
			else
			{

			}
				LCD_Counter ++ ;
				break;
			case 17:
				LCD_enuWriteStringAsync(":",DummyCB);
				LCD_Counter ++ ;
				break;
			case 18 :
				if (seconds < 10)
				{
					LCD_enuWriteStringAsync("0",DummyCB);
				}
				else
				{
					LCD_enuWriteNumberAsync(seconds,DummyCB);
				}
				LCD_Counter ++ ;
				break;
			case 19 :
			if(seconds < 10)
			{
				LCD_enuWriteNumberAsync(seconds,DummyCB);
			}
			else
			{

			}
				printEntireScreen = FALSE ;
				LCD_Counter = 0 ;
				break;
			}
		}
		
		/************************************************************************************/
		/* 	The following part checks if We are ready for receiving a new button request or
			busy handling a current request */
		/************************************************************************************/
		
		/* Check if the previous button request is handled and We are ready for receving a new request or not */
		if(requestHandled == TRUE)
		{
			MUSART_enuRecieveBufferAsync(USART_1,&RecivedMessage,1,receiveCallback);
			requestHandled = FALSE;
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
			receivedButton = Decryption(RecivedMessage);
			
			/* Check whether the recevied data is correct and We can deal with its content
				or it is corrupted so ignore it */
			if(receivedButton != 0xFF)
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
						case DAY_TENS_POSITION:
							if (month == 2)
							{
								if (day > 18)
								{
									day -= 10;
								}
								else
								{
									day += 10;
								}
							}
							else if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
							{
								if (day > 21)
								{
									day -= 20;
								}
								else
								{
									day += 10;
								}
							}
							else
							{
								if (day > 20)
								{
									day -= 20;
								}
								else
								{
									day += 10;
								}
							}
							break;
						case DAY_UNITS_POSITION:
						/* The cursor now is at the position of the Days' units */
							if((month == 2) && (day == 28))
							{
								/* Do Nothing as 28 is the last day in February */
							}
							else if((month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) && (day == 31))
							{
								/* Do Nothing as 31 is the last day in these months */
							}
							else if ((month == 4 || month == 6 || month == 9 || month == 11) && (day == 30))
							{
								/* Do Nothing as 30 is the last day in these months */
							}
							else
							{
								day++;
							}
							break;
						case MONTHS_TENS_POSITION:
							if ((month == 1) || (month == 2))
							{
								month += 10;
							}
							else if ((month >= 3) && (month<=10))
							{
								month = 1;
							}
							else if ((month == 11) || (month == 12))
							{
								month -= 10;
							}
							break ;
						case MONTHS_UNITS_POSITION:
							if(month == 12)
							{
								/* Do Nothing as 12 is the last month in the year */
							}
							else
							{
								month++;
							}
							break;
						case YEARS_THOUSANDS_POSITION:
							year += 1000;
							break;
						case YEARS_HUNDREDS_POSITION:
							year += 100;
							break;
						case YEARS_TENS_POSITION:
							year += 10;
							break;
						case YEARS_UNITS_POSITION:
							year += 1;
							break;
						case HOURS_TENS_POSITION:
							if ((hours >= 0) && (hours <= 13))
							{
								hours += 10;
							}
							else if ((hours >= 14) && (hours <= 23))
							{
								hours -= 10;
							}
							break;
						case HOURS_UNITS_POSITION:
							if((hours >= 0) && (hours <= 22))
							{
								hours++;
							}
							else if (hours == 23)
							{
								/* Do Nothing as 23 is the last hour could be displayed in the hours digits */
							}
							break;
						case MINUTES_TENS_POSITION:
							if((minutes >= 0) && (minutes <= 49))
							{
								minutes += 10;
							}
							else if (minutes > 50)
							{
								minutes -= 50;
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
								seconds -= 50;
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
							
						/* The cursor now is at the position of the Days' units */
						case DAY_TENS_POSITION:
							if ((day >= 1) && (day <= 9))
							{
								/* Do Nothing as 0 is the last value in the tens digit */
							}
							else
							{
								day -= 10;
							}
							break;
						case DAY_UNITS_POSITION:
							if( day == 10 ||day == 20 ||day == 30 )
							{
								/* Do Nothing as 0 is the last value in the units digit */
							}
							else
							{
								day--;
							}
							break;
						case MONTHS_TENS_POSITION:
							if((month >= 1) && (month <= 10))
							{
								/* Do Nothing as 0 is the last value in the tens digit */
							}
							else if (month == 11 || month == 12)
							{
								month -= 10;
							}
							break ;
						case MONTHS_UNITS_POSITION:
							if((month == 1) || (month == 10))
							{
								/* Do Nothing as January is the first month and no month is before him
									and as 0 is the last value in the units digit */
							}
							else
							{
								month--;
							}
							break;
						case YEARS_THOUSANDS_POSITION:
							if (year < 1000)
							{

							}
							else
							{
								year -= 1000;
							}
							break;
						case YEARS_HUNDREDS_POSITION:
							if (year < 100)
							{

							}
							else
							{
								year -= 100;
							}
							break;
						case YEARS_TENS_POSITION:
							if (year < 10)
							{

							}
							else
							{
								year -= 10;
							}
							break;
						case YEARS_UNITS_POSITION:
							if (year < 1)
							{

							}
							else
							{
								year--;
							}
							break;
						case HOURS_TENS_POSITION:
							if((hours >= 0) && (hours <= 9))
							{

							}
							else if (hours>=10 && hours <=23)
							{
								hours -= 10;
							}
							break;
						case HOURS_UNITS_POSITION:
							if((hours == 0) || (hours == 10) || (hours == 20))
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
						}
						else if (Mode == STOPWATCH_MODE)
						{
							Mode = CLOCK_MODE;
						}
					}
					break;
			case EDIT_SWITCH_VALUE :
				if(EditMode == NOT_ACTIVATED)
				{
					EditMode = ACTIVATED ;
					LCD_enuSetCursorAsync  (LCD_enuSecondRow,LCD_enuColumn_3 ,DummyCB);
				}
				else if (EditMode == ACTIVATED)
				{
					EditMode = NOT_ACTIVATED ;
				}
					break;
				}
				
				/* After handling the latest request, raise this flag to make
					the driver ready to receive a new button request */
				requestHandled = TRUE;
			}
			else
			{
				/* If the data is corrupted raise this flag to make the driver ready to receive a new button request */
				requestHandled = TRUE;
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