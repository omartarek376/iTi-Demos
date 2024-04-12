#include <Service/RUNNABLE.h>
#include <Service/SCHED.h>
/*---------------------------------------------------------------------------------------------------*/
/*                                           Extern Prototypes                                       */
/*---------------------------------------------------------------------------------------------------*/

extern void RUNNABLE_LCD(void);
extern void Clock_RunnerTask(void);
extern void Stopwatch_RunnerTask(void);
extern void Switch_runnable(void);



/*---------------------------------------------------------------------------------------------------*/
/*                                           Runnables List                                          */
/*---------------------------------------------------------------------------------------------------*/
Runnable_t Runnables_Arr [_RunnableNumber]=
{
    [LCD_RUNNABLE]=
    {
        .Name = "Lcd_runnable",
        .Periodicity = 1,
        .CB = &RUNNABLE_LCD,
    },
    [Clock_Runner]=
    {
        .Name = "Clock_runnable",
        .Periodicity = 100,
        .CB = &Clock_RunnerTask,
        .DelayMs = 900
    },
    [Stopwatch_Runner]=
    {
        .Name = "Stopwatch_runnable",
        .Periodicity = 100,
        .CB = &Stopwatch_RunnerTask,
        .DelayMs = 1165
    },
    [Switch_Runner]=
    {
        .Name = "Switch_runnable",
        .Periodicity = 100,
        .CB = &Switch_runnable,
        .DelayMs = 1020
    }
};