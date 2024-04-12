#include <Service/SCHEDULER/RUNNABLE.h>
#include <Service/SCHEDULER/SCHED.h>
/*---------------------------------------------------------------------------------------------------*/
/*                                           Extern Prototypes                                       */
/*---------------------------------------------------------------------------------------------------*/

extern void RUNNABLE_LCD(void);
extern void LCD_Write();



/*---------------------------------------------------------------------------------------------------*/
/*                                           Runnables List                                          */
/*---------------------------------------------------------------------------------------------------*/
Runnable_t Runnables_Arr [_RunnableNumber]=
{
    /**
     * [Name]=
     * {    .Name = 
     *      .Periodicity= 
     *      .DelayMs =
     *      .CB = 
     * }
     */

    /*[BlinkingLed_Runnable2]=
    {
        .Name = "Blinking Led 2",
        .Periodicity = 1000,
        .CB = &BlinkingLed2,
        .DelayMs = 1500,
    },*/

    [LCD_RUNNABLE]=
    {
        .Name = "Lcd_runnable",
        .Periodicity = 1,
        .CB = &RUNNABLE_LCD
    },

   [LCD_Writing_runnable]=
    {
        .Name = "write",
        .Periodicity = 10,
        .DelayMs = 1000,
        .CB = &LCD_Write
    },

};