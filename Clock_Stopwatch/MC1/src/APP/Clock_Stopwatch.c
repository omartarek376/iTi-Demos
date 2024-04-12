#include <stdint.h>
#include <stdio.h>
#include "HAL/LCD.h"
#include "HAL/Switch.h"

#define CLOCK_MODE 0
#define STOPWATCH_MODE 1

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

int isLeapYear(int year);
int daysInMonth(int month, int year);
void incrementTime(DateTime *dateTime);
void formatDate(const DateTime *dateTime, char *buffer);
void formatTime(const DateTime *dateTime, char *buffer);
void Clock_RunnerTask(void);
void lcd_TimeStringdone(void);
void lcd_DateStringdone(void);
void Lcd_CursorDone(void);
void formatStopWatchTime(const DateTime *dateTime, char *buffer);
void lcd_StopWatchTimeStringdone(void);
void Stopwatch_RunnerTask(void);

DateTime currentDateTime = {0, 55, 59, 23, 28, 2, 2023};
DateTime currentstopwatchTime = {0, 0, 0, 0, 0, 0, 0};
uint8_t Date_Counter = 0;
uint8_t Mode = STOPWATCH_MODE;
uint8_t Stopwatch_Time = 0;
uint8_t Button_Pressed = 0;
uint8_t Button_arr[7] = {0};

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
    Switch_getstatus(UP_BUTTON, &Button_arr[UP_BUTTON_STATE]);
    Switch_getstatus(DOWN_BUTTON, &Button_arr[DOWN_BUTTON_STATE]);
    Switch_getstatus(LEFT_BUTTON, &Button_arr[LEFT_BUTTON_STATE]);
    Switch_getstatus(RIGHT_BUTTON, &Button_arr[RIGHT_BUTTON_STATE]);
    Switch_getstatus(OK_BUTTON, &Button_arr[OK_BUTTON_STATE]);
    Switch_getstatus(MODE_BUTTON, &Button_arr[MODE_BUTTON_STATE]);
    Switch_getstatus(EDIT_BUTTON, &Button_arr[EDIT_BUTTON_STATE]);

    if(Button_arr[UP_BUTTON_STATE] == 1)
    {
        Button_arr[UP_BUTTON_STATE] = 0;
    } 
    else if(Button_arr[DOWN_BUTTON_STATE] == 1)
    {
        Button_arr[DOWN_BUTTON_STATE] = 0;
    }
    else if(Button_arr[LEFT_BUTTON_STATE] == 1)
    {
        Button_arr[LEFT_BUTTON_STATE] = 0;
    }
    else if(Button_arr[RIGHT_BUTTON_STATE] == 1)
    {
        Button_arr[RIGHT_BUTTON_STATE] = 0;
    }
    else if(Button_arr[OK_BUTTON_STATE] == 1)
    {
        Button_arr[OK_BUTTON_STATE] = 0;
    }
    else if(Button_arr[MODE_BUTTON_STATE] == 1)
    {
        Button_arr[MODE_BUTTON_STATE] = 0;
    }
    else if(Button_arr[EDIT_BUTTON_STATE] == 1)
    {
        Button_arr[EDIT_BUTTON_STATE] = 0;
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
