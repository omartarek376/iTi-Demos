#include <stdint.h>
#include <stdio.h>
#include "HAL/LCD.h"
#include "HAL/Switch.h"
#include "MCAL/USART.h"

#define CLOCK_MODE 0
#define STOPWATCH_MODE 1
#define EDIT_MODE_ON 1
#define EDIT_MODE_OFF 0

#define UP_BUTTON 0B1000
#define DOWN_BUTTON 0B1001
#define LEFT_BUTTON 0B1010
#define RIGHT_BUTTON 0B1011
#define OK_BUTTON 0B1100
#define MODE_BUTTON 0B1101
#define EDIT_BUTTON 0B1110

#define UP_BUTTON_STATE 0
#define DOWN_BUTTON_STATE 1
#define LEFT_BUTTON_STATE 2
#define RIGHT_BUTTON_STATE 3
#define OK_BUTTON_STATE 4
#define MODE_BUTTON_STATE 5
#define EDIT_BUTTON_STATE 6

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




DateTime currentDateTime = {0, 0, 56, 1, 13, 4, 2024};
DateTime currentstopwatchTime = {0, 0, 0, 0, 0, 0, 0};
uint8_t Date_Counter = 0;
uint8_t Mode = CLOCK_MODE;
uint8_t Stopwatch_Time = 0;
uint8_t Button_Pressed = 0;
uint8_t Button_arr[7] = {0};
uint8_t Button_Recieved = 0;
uint8_t Edit_Mode_Status = EDIT_MODE_OFF;
extern USART_RXBuffer rx_button_buff;

void Clock_RunnerTask(void)
{
    incrementTime(&currentDateTime);
    if (Mode == CLOCK_MODE)
    {
        uint8_t TimeStr[33];
        uint8_t DateStr[33];
        if (Date_Counter < 2)
        {
            formatTime(&currentDateTime, TimeStr);
            LCD_enuWriteStringAsync(TimeStr, lcd_TimeStringdone);
            Date_Counter++;
        }
        else
        {
            formatDate(&currentDateTime, DateStr);
            LCD_enuWriteStringAsync(DateStr, lcd_DateStringdone);
            Date_Counter = 0;
        }
    }
    else
    {
        //LCD_enuSetCursorAsync(0, 0, Lcd_CursorDone);
        // LCD_enuClearScreenAsync(LCD_CleanDone);
    }
}

void Stopwatch_RunnerTask(void)
{
    incrementTime(&currentstopwatchTime);
    if (Mode == STOPWATCH_MODE)
    {
        uint8_t StopWatchTimeStr[40];
        formatStopWatchTime(&currentstopwatchTime, StopWatchTimeStr);
        LCD_enuWriteStringAsync(StopWatchTimeStr, lcd_StopWatchTimeStringdone);
    }
}

void Switch_runnable(void)
{
    uint8_t Button_Send = 0;
    Switch_getstatus(UP_Switch, &Button_arr[UP_BUTTON_STATE]);
    Switch_getstatus(DOWN_Switch, &Button_arr[DOWN_BUTTON_STATE]);
    Switch_getstatus(LEFT_Switch, &Button_arr[LEFT_BUTTON_STATE]);
    Switch_getstatus(RIGHT_Switch, &Button_arr[RIGHT_BUTTON_STATE]);
    Switch_getstatus(OK_Switch, &Button_arr[OK_BUTTON_STATE]);
    Switch_getstatus(MODE_Switch, &Button_arr[MODE_BUTTON_STATE]);
    Switch_getstatus(EDIT_Switch, &Button_arr[EDIT_BUTTON_STATE]);

    if(Button_arr[UP_BUTTON_STATE] == 1)
    {
        Button_Send = EncodeFrame(UP_BUTTON);
        USART_SendByteAsynchronous(USART1,Button_Send);
        Button_arr[UP_BUTTON_STATE] = 0;
    } 
    else if(Button_arr[DOWN_BUTTON_STATE] == 1)
    {
        Button_Send = EncodeFrame(DOWN_BUTTON);
        USART_SendByteAsynchronous(USART1,Button_Send);
        Button_arr[DOWN_BUTTON_STATE] = 0;
    }
    else if(Button_arr[LEFT_BUTTON_STATE] == 1)
    {
        Button_Send = EncodeFrame(LEFT_BUTTON);
        USART_SendByteAsynchronous(USART1,Button_Send);
        Button_arr[LEFT_BUTTON_STATE] = 0;
    }
    else if(Button_arr[RIGHT_BUTTON_STATE] == 1)
    {
        Button_Send = EncodeFrame(RIGHT_BUTTON);
        USART_SendByteAsynchronous(USART1,Button_Send);
        Button_arr[RIGHT_BUTTON_STATE] = 0;
    }
    else if(Button_arr[OK_BUTTON_STATE] == 1)
    {
        Button_Send = EncodeFrame(OK_BUTTON);
        USART_SendByteAsynchronous(USART1,Button_Send);
        Button_arr[OK_BUTTON_STATE] = 0;
    }
    else if(Button_arr[MODE_BUTTON_STATE] == 1)
    {
        Button_Send = EncodeFrame(MODE_BUTTON);
        USART_SendByteAsynchronous(USART1,Button_Send);
        Button_arr[MODE_BUTTON_STATE] = 0;
    }
    else if(Button_arr[EDIT_BUTTON_STATE] == 1)
    {
        Button_Send = EncodeFrame(EDIT_BUTTON);
        USART_SendByteAsynchronous(USART1,Button_Send);
        Button_arr[EDIT_BUTTON_STATE] = 0;
    }

    switch(Button_Recieved)
    {
        case MODE_BUTTON:
        Mode = !Mode;
        LCD_enuClearScreenAsync(LCD_CleanDone);
        Button_Recieved = 0;
        break;

        case EDIT_BUTTON:
        Edit_Mode_Status = !Edit_Mode_Status;
        if (Edit_Mode_Status == EDIT_MODE_ON)
        {
            if (Mode == CLOCK_MODE)
            {
                LCD_enuSendCommandAsync(LCD_DisplayON_CursorON_BlinkOFF, Lcd_CursorDone);
            }
        }
        else
        {
            if (Mode == CLOCK_MODE)
            {
                LCD_enuSendCommandAsync(LCD_DisplayON_CursorOFF_BlinkOFF, Lcd_CursorDone);
            }
        }
        break;
    }
    
}

void incrementTime(DateTime *dateTime)
{
    if ((dateTime->milliseconds += 100) >= 1000)
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
    sprintf(buffer, "%02d:%02d:%02d.%03d", dateTime->hours, dateTime->minutes, dateTime->seconds, dateTime->milliseconds);
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
    if (Date_Counter == 2)
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
    uint8_t nibble1 = frame >> 4;
    uint8_t nibble2 = frame & 0x0F;
    return nibble1 ^ nibble2;
}


void recieve_callback()
{
    if (DecodeFrame(rx_button_buff.Data) == 0)
    {
        Button_Recieved = *rx_button_buff.Data >> 4;
    }
    USART_ReceiveBuffer(&rx_button_buff);
}

void LCD_CleanDone(void)
{
    // LCD_enuSetCursorAsync(0, 0, Lcd_CursorDone);
}
