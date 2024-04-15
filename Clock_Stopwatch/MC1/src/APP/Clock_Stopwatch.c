#include <stdint.h>
#include <stdio.h>
#include "HAL/LCD.h"
#include "HAL/Switch.h"
#include "MCAL/USART.h"

#define CLOCK_MODE 0
#define STOPWATCH_MODE 1
#define EDIT_MODE_ON 1
#define EDIT_MODE_OFF 0
#define COMMAND_DONE 1
#define COMMAND_IN_PROGRESS 0
#define STOPWATCH_RUNNING 1
#define STOPWATCH_STOPPED 0
#define STOPWATCH_RESET 2
#define OK_MODE_ON 1
#define OK_MODE_OFF 0

#define UP_START_BUTTON 0B1000
#define DOWN_STOP_BUTTON 0B1001
#define LEFT_RESET_BUTTON 0B1010
#define RIGHT_BUTTON 0B1011
#define OK_BUTTON 0B1100
#define MODE_BUTTON 0B1101
#define EDIT_BUTTON 0B1110

#define UP_START_BUTTON_STATE 0
#define DOWN_STOP_BUTTON_STATE 1
#define LEFT_RESET_BUTTON_STATE 2
#define RIGHT_BUTTON_STATE 3
#define OK_BUTTON_STATE 4
#define MODE_BUTTON_STATE 5
#define EDIT_BUTTON_STATE 6
#define NO_BUTTON_PRESSED 0

#define CLOCK_CURSOR_START_X 0
#define CLOCK_CURSOR_END_X 7
#define DATE_CURSOR_START_X 0
#define DATE_CURSOR_END_X 9
#define CURSOR_AT_CLOCK_Y 0
#define CURSOR_AT_DATE_Y 1
#define HOUR_EDIT_POSITION 1
#define MINUTE_EDIT_POSITION 4
#define SECOND_EDIT_POSITION 7
#define DAY_EDIT_POSITION 1
#define MONTH_EDIT_POSITION 4
#define YEAR_EDIT_POSITION 9
#define DECADE_EDIT_POSITION 8
#define CENTURY_EDIT_POSITION 7
#define MILLENIUM_EDIT_POSITION 6

/*******************************************************************************************************************/
/*                                                 Structures                                                      */
/*******************************************************************************************************************/
typedef struct
{
    uint16_t milliseconds;
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t month;
    uint16_t year;
} DateTime;

typedef struct
{
    int8_t X;
    uint8_t Y;
} Cursor;

/*******************************************************************************************************************/
/*                                                 Function Prototypes                                             */
/*******************************************************************************************************************/

int isLeapYear(int year);
int daysInMonth(int month, int year);
void incrementTime(DateTime *dateTime);
void formatDate(const DateTime *dateTime, char *buffer);
void formatTime(const DateTime *dateTime, char *buffer);
void formatStopWatchTime(const DateTime *dateTime, char *buffer);
void Clock_RunnerTask(void);
void lcd_TimeStringdone(void);
void lcd_DateStringdone(void);
void Lcd_CursorDone(void);
void Stopwatch_RunnerTask(void);
uint8_t EncodeFrame(uint8_t Button);
void Switch_runnable(void);
void lcd_StopWatchTimeStringdone(void);
void recieve_callback(void);
uint8_t DecodeFrame(uint8_t frame);
void LCD_CleanDone(void);
void Lcd_EditDone(void);
void Lcd_EditCursorDone(void);
void Lcd_EditOFFDone(void);
void Lcd_OKDone(void);
void Lcd_EditCursorRefreshDoneTime(void);
void Lcd_EditCursorRefreshDoneDate(void);
void lcd_TimeStringEditdone(void);

/*******************************************************************************************************************/
/*                                                 Global Variable                                                 */
/*******************************************************************************************************************/

DateTime currentDateTime = {0, 0, 20, 4, 17, 4, 2024};
DateTime previousDateTime = {0, 0, 0, 0, 0, 0, 0};
DateTime currentstopwatchTime = {0, 0, 0, 0, 0, 0, 0};
uint8_t Date_Counter = 0;
uint8_t Mode = CLOCK_MODE;
uint8_t Stopwatch_Time = 0;
uint8_t Button_arr[7] = {0};
uint8_t Button_Recieved = NO_BUTTON_PRESSED;
uint8_t Edit_Mode_Status = EDIT_MODE_OFF;
extern USART_RXBuffer rx_button_buff;
Cursor Edit_Cursor = {CLOCK_CURSOR_START_X, CURSOR_AT_CLOCK_Y};
uint8_t Command_Guard = COMMAND_DONE;
uint8_t Stopwatch_State = STOPWATCH_STOPPED;
uint8_t OK_Mode_Status = OK_MODE_OFF;
uint8_t TimeStr[33];
uint8_t DateStr[33];

/*******************************************************************************************************************/
/*                                                 Runnables                                                       */
/*******************************************************************************************************************/

void Clock_RunnerTask(void)
{
    incrementTime(&currentDateTime);
    if (Mode == CLOCK_MODE && Command_Guard == COMMAND_DONE && Edit_Mode_Status == EDIT_MODE_OFF)
    {
        if (Date_Counter == 0)
        {
            if (previousDateTime.seconds != currentDateTime.seconds)
            {
                formatTime(&currentDateTime, TimeStr);
                LCD_enuWriteStringAsync(TimeStr, lcd_TimeStringdone);
                previousDateTime = currentDateTime;
                Date_Counter++;
            }
        }
        else
        {
            formatDate(&currentDateTime, DateStr);
            LCD_enuWriteStringAsync(DateStr, lcd_DateStringdone);
            Date_Counter = 0;
        }
    }
}

void Stopwatch_RunnerTask(void)
{
    uint8_t StopWatchTimeStr[40];
    formatStopWatchTime(&currentstopwatchTime, StopWatchTimeStr);
    switch (Stopwatch_State)
    {
    case STOPWATCH_RESET:
        currentstopwatchTime.milliseconds = 0;
        currentstopwatchTime.seconds = 0;
        currentstopwatchTime.minutes = 0;
        currentstopwatchTime.hours = 0;
        if (Mode == STOPWATCH_MODE && Command_Guard == COMMAND_DONE)
        {
            LCD_enuWriteStringAsync(StopWatchTimeStr, lcd_StopWatchTimeStringdone);
        }
        break;

    case STOPWATCH_RUNNING:
        incrementTime(&currentstopwatchTime);
        if (Mode == STOPWATCH_MODE && Command_Guard == COMMAND_DONE)
        {
            LCD_enuWriteStringAsync(StopWatchTimeStr, lcd_StopWatchTimeStringdone);
        }
        break;

    case STOPWATCH_STOPPED:
        if (Mode == STOPWATCH_MODE && Command_Guard == COMMAND_DONE)
        {
            LCD_enuWriteStringAsync(StopWatchTimeStr, lcd_StopWatchTimeStringdone);
        }
        break;
    }
}

void Switch_runnable(void)
{
    /* Button Recieve Part */
    Button_Recieved = DecodeFrame(Button_Recieved);
    switch (Button_Recieved)
    {
    case MODE_BUTTON:
        Mode = !Mode;
        Command_Guard = COMMAND_IN_PROGRESS;
        Edit_Mode_Status = EDIT_MODE_OFF;
        OK_Mode_Status = OK_MODE_OFF;
        LCD_enuClearScreenAsync(LCD_CleanDone);
        Button_Recieved = NO_BUTTON_PRESSED;
        break;

    case EDIT_BUTTON:
        Edit_Mode_Status = !Edit_Mode_Status;
        if (Edit_Mode_Status == EDIT_MODE_ON)
        {
            if (Mode == CLOCK_MODE)
            {
                LCD_enuSendCommandAsync(LCD_DisplayON_CursorOFF_BlinkON, Lcd_EditDone);
            }
        }
        else
        {
            if (Mode == CLOCK_MODE)
            {
                LCD_enuSendCommandAsync(LCD_DisplayON_CursorOFF_BlinkOFF, Lcd_EditOFFDone);
                OK_Mode_Status = OK_MODE_OFF;
            }
        }
        Button_Recieved = NO_BUTTON_PRESSED;
        break;

    case UP_START_BUTTON:
        if (Mode == STOPWATCH_MODE)
        {
            Stopwatch_State = STOPWATCH_RUNNING;
        }
        else
        {
            if (Edit_Mode_Status == EDIT_MODE_ON && OK_Mode_Status == OK_MODE_OFF)
            {
                Edit_Cursor.Y = !Edit_Cursor.Y;
                LCD_enuSetCursorAsync(Edit_Cursor.Y, Edit_Cursor.X, Lcd_EditCursorDone);
            }
            if (Edit_Mode_Status == EDIT_MODE_ON && OK_Mode_Status == OK_MODE_ON)
            {
                switch (Edit_Cursor.Y)
                {
                case CURSOR_AT_CLOCK_Y:
                    switch (Edit_Cursor.X)
                    {
                    case HOUR_EDIT_POSITION:
                        currentDateTime.hours++;
                        if (currentDateTime.hours > 23)
                        {
                            currentDateTime.hours = 0;
                        }
                        LCD_enuSetCursorAsync(CURSOR_AT_CLOCK_Y,CLOCK_CURSOR_START_X,Lcd_EditCursorRefreshDoneTime);
                        break;

                    case MINUTE_EDIT_POSITION:
                        currentDateTime.minutes++;
                        if (currentDateTime.minutes > 59)
                        {
                            currentDateTime.minutes = 0;
                        }
                        LCD_enuSetCursorAsync(CURSOR_AT_CLOCK_Y,CLOCK_CURSOR_START_X,Lcd_EditCursorRefreshDoneTime);
                        break;

                    case SECOND_EDIT_POSITION:
                        currentDateTime.seconds++;
                        if (currentDateTime.seconds > 59)
                        {
                            currentDateTime.seconds = 0;
                        }
                        LCD_enuSetCursorAsync(CURSOR_AT_CLOCK_Y,CLOCK_CURSOR_START_X,Lcd_EditCursorRefreshDoneTime);
                        break;
                    }
                    break;
                case CURSOR_AT_DATE_Y:
                    switch (Edit_Cursor.X)
                    {
                    case DAY_EDIT_POSITION:
                        currentDateTime.day++;
                        if (currentDateTime.day > daysInMonth(currentDateTime.month, currentDateTime.year))
                        {
                            currentDateTime.day = 1;
                        }
                        LCD_enuSetCursorAsync(CURSOR_AT_DATE_Y,DATE_CURSOR_START_X,Lcd_EditCursorRefreshDoneDate);
                        break;

                    case MONTH_EDIT_POSITION:
                        currentDateTime.month++;
                        if (currentDateTime.month > 12)
                        {
                            currentDateTime.month = 1;
                        }
                        LCD_enuSetCursorAsync(CURSOR_AT_DATE_Y,DATE_CURSOR_START_X,Lcd_EditCursorRefreshDoneDate);
                        break;

                    case YEAR_EDIT_POSITION:
                        currentDateTime.year++;
                        LCD_enuSetCursorAsync(CURSOR_AT_DATE_Y,DATE_CURSOR_START_X,Lcd_EditCursorRefreshDoneDate);
                        break;

                    case DECADE_EDIT_POSITION:
                        currentDateTime.year += 10;
                        LCD_enuSetCursorAsync(CURSOR_AT_DATE_Y,DATE_CURSOR_START_X,Lcd_EditCursorRefreshDoneDate);
                        break;

                    case CENTURY_EDIT_POSITION:
                        currentDateTime.year += 100;
                        LCD_enuSetCursorAsync(CURSOR_AT_DATE_Y,DATE_CURSOR_START_X,Lcd_EditCursorRefreshDoneDate);
                        break;

                    case MILLENIUM_EDIT_POSITION:
                        currentDateTime.year += 1000;
                        LCD_enuSetCursorAsync(CURSOR_AT_DATE_Y,DATE_CURSOR_START_X,Lcd_EditCursorRefreshDoneDate);
                        break;
                    }
                    break;
                }
                break;
            }
        }
        Button_Recieved = NO_BUTTON_PRESSED;
        break;

    case DOWN_STOP_BUTTON:
        if (Mode == STOPWATCH_MODE)
        {
            Stopwatch_State = STOPWATCH_STOPPED;
        }
        else
        {
            if (Edit_Mode_Status == EDIT_MODE_ON && OK_Mode_Status == OK_MODE_OFF)
            {
                Edit_Cursor.Y = !Edit_Cursor.Y;
                LCD_enuSetCursorAsync(Edit_Cursor.Y, Edit_Cursor.X, Lcd_EditCursorDone);
            }
            if (Edit_Mode_Status == EDIT_MODE_ON && OK_Mode_Status == OK_MODE_ON)
            {
                switch (Edit_Cursor.Y)
                {
                case CURSOR_AT_CLOCK_Y:
                    switch (Edit_Cursor.X)
                    {
                    case HOUR_EDIT_POSITION:
                        currentDateTime.hours--;
                        if (currentDateTime.hours < 0)
                        {
                            currentDateTime.hours = 23;
                        }
                        LCD_enuSetCursorAsync(CURSOR_AT_CLOCK_Y,CLOCK_CURSOR_START_X,Lcd_EditCursorRefreshDoneTime);
                        break;

                    case MINUTE_EDIT_POSITION:
                        currentDateTime.minutes--;
                        if (currentDateTime.minutes < 0)
                        {
                            currentDateTime.minutes = 59;
                        }
                        LCD_enuSetCursorAsync(CURSOR_AT_CLOCK_Y,CLOCK_CURSOR_START_X,Lcd_EditCursorRefreshDoneTime);
                        break;

                    case SECOND_EDIT_POSITION:
                        currentDateTime.seconds--;
                        if (currentDateTime.seconds < 0)
                        {
                            currentDateTime.seconds = 59;
                        }
                        LCD_enuSetCursorAsync(CURSOR_AT_CLOCK_Y,CLOCK_CURSOR_START_X,Lcd_EditCursorRefreshDoneTime);
                        break;
                    }
                    break;
                case CURSOR_AT_DATE_Y:
                    switch (Edit_Cursor.X)
                    {
                    case DAY_EDIT_POSITION:
                        currentDateTime.day--;
                        if (currentDateTime.day < 1)
                        {
                            currentDateTime.day = daysInMonth(currentDateTime.month, currentDateTime.year);
                        }
                        LCD_enuSetCursorAsync(CURSOR_AT_DATE_Y,DATE_CURSOR_START_X,Lcd_EditCursorRefreshDoneDate);
                        break;

                    case MONTH_EDIT_POSITION:
                        currentDateTime.month--;
                        if (currentDateTime.month < 1)
                        {
                            currentDateTime.month = 12;
                        }
                        LCD_enuSetCursorAsync(CURSOR_AT_DATE_Y,DATE_CURSOR_START_X,Lcd_EditCursorRefreshDoneDate);
                        break;

                    case YEAR_EDIT_POSITION:
                        currentDateTime.year--;
                        LCD_enuSetCursorAsync(CURSOR_AT_DATE_Y,DATE_CURSOR_START_X,Lcd_EditCursorRefreshDoneDate);
                        break;

                    case DECADE_EDIT_POSITION:
                        currentDateTime.year -= 10;
                        LCD_enuSetCursorAsync(CURSOR_AT_DATE_Y,DATE_CURSOR_START_X,Lcd_EditCursorRefreshDoneDate);
                        break;

                    case CENTURY_EDIT_POSITION:
                        currentDateTime.year -= 100;
                        LCD_enuSetCursorAsync(CURSOR_AT_DATE_Y,DATE_CURSOR_START_X,Lcd_EditCursorRefreshDoneDate);
                        break;

                    case MILLENIUM_EDIT_POSITION:
                        currentDateTime.year -= 1000;
                        LCD_enuSetCursorAsync(CURSOR_AT_DATE_Y,DATE_CURSOR_START_X,Lcd_EditCursorRefreshDoneDate);
                        break;
                    }
                    break;
                }    
            }
        }
        Button_Recieved = NO_BUTTON_PRESSED;
        break;

    case LEFT_RESET_BUTTON:
        if (Mode == STOPWATCH_MODE)
        {
            Stopwatch_State = STOPWATCH_RESET;
        }
        else
        {
            Edit_Cursor.X--;
            if (Edit_Cursor.Y == CURSOR_AT_CLOCK_Y && Edit_Cursor.X < CLOCK_CURSOR_START_X)
            {
                Edit_Cursor.X = CLOCK_CURSOR_END_X;
            }
            else if (Edit_Cursor.Y == CURSOR_AT_DATE_Y && Edit_Cursor.X < DATE_CURSOR_START_X)
            {
                Edit_Cursor.X = DATE_CURSOR_END_X;
            }
            LCD_enuSetCursorAsync(Edit_Cursor.Y, Edit_Cursor.X, Lcd_EditCursorDone);
        }
        Button_Recieved = NO_BUTTON_PRESSED;
        break;

    case RIGHT_BUTTON:
        Edit_Cursor.X++;
        if (Edit_Cursor.Y == CURSOR_AT_CLOCK_Y && Edit_Cursor.X > CLOCK_CURSOR_END_X)
        {
            Edit_Cursor.X = CLOCK_CURSOR_START_X;
        }
        else if (Edit_Cursor.Y == CURSOR_AT_DATE_Y && Edit_Cursor.X > DATE_CURSOR_END_X)
        {
            Edit_Cursor.X = DATE_CURSOR_START_X;
        }
        LCD_enuSetCursorAsync(Edit_Cursor.Y, Edit_Cursor.X, Lcd_EditCursorDone);
        Button_Recieved = NO_BUTTON_PRESSED;
        break;

    case OK_BUTTON:
        OK_Mode_Status = !OK_Mode_Status;
        if (OK_Mode_Status == OK_MODE_ON && Edit_Mode_Status == EDIT_MODE_ON && Mode == CLOCK_MODE)
        {
            LCD_enuSendCommandAsync(LCD_DisplayON_CursorON_BlinkOFF, Lcd_OKDone);
        }
        else if (OK_Mode_Status == OK_MODE_OFF && Edit_Mode_Status == EDIT_MODE_ON && Mode == CLOCK_MODE)
        {
            LCD_enuSendCommandAsync(LCD_DisplayON_CursorOFF_BlinkON, Lcd_OKDone);
        }
        Button_Recieved = NO_BUTTON_PRESSED;
        break;
    }
    /* Button Send Part */
    uint8_t Button_Send = 0;
    Switch_getstatus(UP_Switch, &Button_arr[UP_START_BUTTON_STATE]);
    Switch_getstatus(DOWN_Switch, &Button_arr[DOWN_STOP_BUTTON_STATE]);
    Switch_getstatus(LEFT_Switch, &Button_arr[LEFT_RESET_BUTTON_STATE]);
    Switch_getstatus(RIGHT_Switch, &Button_arr[RIGHT_BUTTON_STATE]);
    Switch_getstatus(OK_Switch, &Button_arr[OK_BUTTON_STATE]);
    Switch_getstatus(MODE_Switch, &Button_arr[MODE_BUTTON_STATE]);
    Switch_getstatus(EDIT_Switch, &Button_arr[EDIT_BUTTON_STATE]);

    if (Button_arr[UP_START_BUTTON_STATE] == 1)
    {
        Button_Send = EncodeFrame(UP_START_BUTTON);
        USART_SendByteAsynchronous(USART1, Button_Send);
        Button_arr[UP_START_BUTTON_STATE] = 0;
    }
    else if (Button_arr[DOWN_STOP_BUTTON_STATE] == 1)
    {
        Button_Send = EncodeFrame(DOWN_STOP_BUTTON);
        USART_SendByteAsynchronous(USART1, Button_Send);
        Button_arr[DOWN_STOP_BUTTON_STATE] = 0;
    }
    else if (Button_arr[LEFT_RESET_BUTTON_STATE] == 1)
    {
        Button_Send = EncodeFrame(LEFT_RESET_BUTTON);
        USART_SendByteAsynchronous(USART1, Button_Send);
        Button_arr[LEFT_RESET_BUTTON_STATE] = 0;
    }
    else if (Button_arr[RIGHT_BUTTON_STATE] == 1)
    {
        Button_Send = EncodeFrame(RIGHT_BUTTON);
        USART_SendByteAsynchronous(USART1, Button_Send);
        Button_arr[RIGHT_BUTTON_STATE] = 0;
    }
    else if (Button_arr[OK_BUTTON_STATE] == 1)
    {
        Button_Send = EncodeFrame(OK_BUTTON);
        USART_SendByteAsynchronous(USART1, Button_Send);
        Button_arr[OK_BUTTON_STATE] = 0;
    }
    else if (Button_arr[MODE_BUTTON_STATE] == 1)
    {
        Button_Send = EncodeFrame(MODE_BUTTON);
        USART_SendByteAsynchronous(USART1, Button_Send);
        Button_arr[MODE_BUTTON_STATE] = 0;
    }
    else if (Button_arr[EDIT_BUTTON_STATE] == 1)
    {
        Button_Send = EncodeFrame(EDIT_BUTTON);
        USART_SendByteAsynchronous(USART1, Button_Send);
        Button_arr[EDIT_BUTTON_STATE] = 0;
    }
}

/*******************************************************************************************************************/
/*                                                 Helper Functions                                                */
/*******************************************************************************************************************/

void incrementTime(DateTime *dateTime)
{
    if ((dateTime->milliseconds += 200) >= 1000)
    {
        dateTime->milliseconds -= 1000;
        if (++dateTime->seconds >= 60)
        {
            dateTime->seconds = 0;
            if (++dateTime->minutes >= 60)
            {
                dateTime->minutes = 0;
                if (++dateTime->hours >= 24)
                {
                    dateTime->hours = 0;
                    dateTime->day++;
                    if (dateTime->day > daysInMonth(dateTime->month, dateTime->year))
                    {
                        dateTime->day = 1;
                        dateTime->month++;
                        if (dateTime->month > 12)
                        {
                            dateTime->month = 1;
                            dateTime->year++;
                        }
                    }
                }
            }
        }
    }
}

void formatDate(const DateTime *dateTime, char *buffer)
{
    sprintf(buffer, "%02d/%02d/%04d", dateTime->day, dateTime->month, dateTime->year);
}

void formatTime(const DateTime *dateTime, char *buffer)
{
    sprintf(buffer, "%02d:%02d:%02d", dateTime->hours, dateTime->minutes, dateTime->seconds);
}

void formatStopWatchTime(const DateTime *dateTime, char *buffer)
{
    sprintf(buffer, "%02d:%02d:%02d:%03d", dateTime->hours, dateTime->minutes, dateTime->seconds, dateTime->milliseconds);
}

int isLeapYear(int year)
{
    if (year % 4 != 0)
    {
        return 0;
    }
    else if (year % 100 != 0)
    {
        return 1;
    }
    else if (year % 400 != 0)
    {
        return 0;
    }
    return 1;
}

int daysInMonth(int month, int year)
{
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeapYear(year))
    {
        return 29;
    }
    return days[month - 1];
}

void Lcd_CursorDone(void)
{
}

void lcd_TimeStringdone(void)
{
    if (Date_Counter == 1)
    {
        LCD_enuSetCursorAsync(1, 0, Lcd_CursorDone);
    }
    else
    {
        LCD_enuSetCursorAsync(0, 0, Lcd_CursorDone);
    }
}

void lcd_DateStringdone(void)
{
    LCD_enuSetCursorAsync(0, 0, Lcd_CursorDone);
}

void lcd_StopWatchTimeStringdone(void)
{
    LCD_enuSetCursorAsync(0, 0, Lcd_CursorDone);
}

uint8_t EncodeFrame(uint8_t Button)
{
    return (Button << 4) | Button;
}

uint8_t DecodeFrame(uint8_t frame)
{
    uint8_t result = 0;
    if (((frame >> 4) ^ (frame & 0x0F)) == 0)
    {
        result = frame >> 4;
    }
    return result;
}

void recieve_callback()
{
    Button_Recieved = rx_button_buff.Data;
    USART_ReceiveBuffer(&rx_button_buff);
}

void LCD_CleanDone(void)
{
    Date_Counter = 0;
    Command_Guard = COMMAND_DONE;
}

void Lcd_EditDone(void)
{
    Command_Guard = COMMAND_IN_PROGRESS;
    LCD_enuSetCursorAsync(CURSOR_AT_CLOCK_Y, CLOCK_CURSOR_START_X, Lcd_EditCursorDone);
}

void Lcd_EditCursorDone(void)
{
    Command_Guard = COMMAND_DONE;
}

void Lcd_EditOFFDone(void)
{
    Command_Guard = COMMAND_IN_PROGRESS;
    Date_Counter = 0;
    Edit_Cursor.X = CLOCK_CURSOR_START_X;
    Edit_Cursor.Y = CURSOR_AT_CLOCK_Y;
    LCD_enuSetCursorAsync(0, 0, Lcd_EditCursorDone);
}

void Lcd_OKDone(void)
{
    Command_Guard = COMMAND_DONE;
}

void Lcd_EditCursorRefreshDoneTime(void)
{
    Command_Guard = COMMAND_IN_PROGRESS;
    formatTime(&currentDateTime, TimeStr);
    LCD_enuWriteStringAsync(TimeStr, lcd_TimeStringEditdone);
}

void Lcd_EditCursorRefreshDoneDate(void)
{
    Command_Guard = COMMAND_IN_PROGRESS;
    formatDate(&currentDateTime, DateStr);
    LCD_enuWriteStringAsync(DateStr, lcd_TimeStringEditdone);
}

void lcd_TimeStringEditdone(void)
{
    LCD_enuSetCursorAsync(Edit_Cursor.Y,Edit_Cursor.X,Lcd_EditCursorDone);
}