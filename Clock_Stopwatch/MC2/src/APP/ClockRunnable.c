#include "MCAL/MUSART/MUSART_interface.h"
#include "HAL/HLCD/HLCD_interface.h"

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

u8 Decryption(u8 value)                                                                          
{ 
  u8 CheckSumBits= (value) & 0x0F ;
  u8 Data = (value >> 4) ;
  if(CheckSumBits ^ value == 0x0F)
  {
     
  }
  else
  {
    Data = 0xFF; 
  }
  return Data;
}

void DummyCB(void)
{

}

typedef enum
{
  Clock_Mode    ,
  Stopwatch_Mode,
}MODES;
MODES Mode = Clock_Mode ;

typedef enum
{ 
  Not_Activated,
  Activated    ,
}EDITMODES;
EDITMODES EditMode = Not_Activated ;

typedef enum
{ 
  Not_PRESSED   ,
  FirstPRESSED  ,
  SecondPRESSED ,
 
}OKSTATE;
OKSTATE OKState = Not_PRESSED ;


u8 receiveFlag = 0;
u8 RequesetHandeled=1;
void receiveCallback()
{ 
   receiveFlag = 1;
}

void clockRunnable(void)
{  
 static u8 counter  = 0 ;
 static u8 LCD_Counter =0;

 u8 RecivedMessage  = 0 ;

 static u8  Hours   = 12 ;
 static u8  Minutes = 0  ;
 static u8  Seconds = 0  ;
 static u8  Day   = 1    ;
 static u8  Month = 1    ;
 static u16 Year  = 2024 ;
   

   if(counter %10 == 0 & counter != 0 )
   {
    Seconds ++;
    if(Seconds == 60)
    {
        Seconds = 0; 
        Minutes ++ ; 
        if(Minutes == 60)
        {
            Minutes = 0 ;
            Hours ++ ; 
        }
        if(Hours == 24)
        {
            Hours = 0 ;
            Day ++    ; 
        }
        if((Month == 1 ||Month == 3 ||Month == 5 ||Month == 7 ||Month == 8 ||Month == 10 ||Month == 12) \
        && Day == 31 )
        {
            Day = 1;
            Month ++;
        }
        else if ((Month == 4 ||Month == 6 ||Month == 9 ||Month == 11 ) && Day == 30)
        {
            Day = 1;
            Month ++;
        }
        else 
        {
            Day = 1;
            Month ++;
        }
        if(Month == 13)
        {
            Month = 0;
            Year ++ ;
        }
    }
      counter ++ ;
   }

if(Mode == Clock_Mode)
{
switch(LCD_Counter)
{ 
   case 0 :
   LCD_enuSetCursorAsync  (LCD_enuFirstRow,LCD_enuColumn_1 ,DummyCB);
   break;
   case 1 :
   LCD_enuWriteStringAsync("Date: ",DummyCB);
   break;
   case 2 :
   if (Day < 10)
   {
    LCD_enuWriteStringAsync("0",DummyCB);
   }
   break;
   case 3:
   LCD_enuWriteNumberAsync(Day,DummyCB);
   break;
   case 4 :
   LCD_enuWriteStringAsync("/",DummyCB);
   break;
   case 5 :
   if (Month < 10)
   {
    LCD_enuWriteStringAsync("0",DummyCB);
   }
   break;
   case 6:
   LCD_enuWriteNumberAsync(Month,DummyCB);
   break;
   case 7 :
   LCD_enuWriteStringAsync("/",DummyCB);
   break;
   case 8 :
   if (Year < 1000)
   {
    LCD_enuWriteStringAsync("0",DummyCB);
   }
   else if (Year < 100)
   {
    LCD_enuWriteStringAsync("00",DummyCB);
   }
   else if (Year < 10)
   {
    LCD_enuWriteStringAsync("000",DummyCB);
   }
   else if (Year < 1)
   {
    LCD_enuWriteStringAsync("0000",DummyCB);
   }
   break;
   case 9 :
   LCD_enuWriteNumberAsync(Year,DummyCB);
   break;
   case 10 :
   LCD_enuSetCursorAsync  (LCD_enuSecondRow,LCD_enuColumn_1 ,DummyCB);
   break;
   case 11:
   LCD_enuWriteStringAsync("Time:   ",DummyCB);
   break;
   case 12:
   if(Hours < 10)
   {
    LCD_enuWriteStringAsync("0",DummyCB);
   }
   break;
   case 13:
   LCD_enuWriteNumberAsync(Hours,DummyCB);
   break;
   case 14 :
   LCD_enuWriteStringAsync(":",DummyCB);
   break;
   case 15 :
   if (Minutes < 10)
   {
    LCD_enuWriteStringAsync("0",DummyCB);
   }
   break;
   case 16 :
   LCD_enuWriteNumberAsync(Minutes,DummyCB);
   break;
   case 17:
   LCD_enuWriteStringAsync(":",DummyCB);
   break;
   case 18 :
   if (Seconds < 10)
   {
    LCD_enuWriteStringAsync("0",DummyCB);
   }
   break;
   case 19 :
   LCD_enuWriteNumberAsync(Seconds,DummyCB);
   break;
}

 if(RequesetHandeled == 1)
 {
   MUSART_enuRecieveBufferAsync(USART_1,&RecivedMessage,1,receiveCallback);
   RequesetHandeled=0;
 }


 if(receiveFlag == 1)
 {
    u8 RecivedButton = 0;
    RecivedButton = Decryption(RecivedMessage);
    static LCD_enuRowNumber_t    CurrentRow = LCD_enuFirstRow ;
    static LCD_enuColumnNumber_t CurrentCol = LCD_enuColumn_1 ;
    u8 CursorPos = 0;
    if(RecivedButton != 0xFF)
    {
        switch (RecivedButton)
        {   
            case UP_SWITCH_VALUE :
            if(EditMode == Activated && OKState == Not_PRESSED )
            {
                if (CurrentRow == LCD_enuFirstRow)
                {

                }
                else
                {
                    CurrentRow = LCD_enuFirstRow;
                }
                LCD_enuSetCursorAsync (CurrentRow,CurrentCol,DummyCB);
               
            }
            else if (EditMode == Activated && OKState == FirstPRESSED )
            {
                CursorPos = (CurrentCol) + (CurrentRow*16);
                switch (CursorPos)
                {
                    case 6:
                    if (Month == 2)
                    {
                       if (Day>18)
                       {
                        Day -=10;
                       }
                       else
                       {
                        Day +=10;
                       }
                    }
                    else if (Month == 1 ||Month == 3 ||Month == 5 ||Month == 7 ||Month == 8 ||Month == 10 ||Month == 12) 
                    {
                        if (Day>21)
                       {
                        Day -=20;
                       }
                       else
                       {
                        Day +=10;
                       }
                    }
                    else
                    {
                       if (Day>20)
                       {
                        Day -=20;
                       }
                       else
                       {
                        Day +=10;
                       }
                    }
                    break;
                    case 7:
                    if(Month==2 && Day==28)
                    {
                        Day=20;
                    }
                    else if((Month == 1 ||Month == 3 ||Month == 5 ||Month == 7 ||Month == 8 ||Month == 10 ||Month == 12) && Day == 31)
                    {
                         Day =30;
                    }
                    else if ((Month == 4 ||Month == 6 ||Month == 9 ||Month == 11) && Day == 30)
                    {

                    }
                    else
                    {
                        Day ++ ;
                    }
                    break;
                    case 9 :
                    if(Month == 1 || Month == 2)
                    {
                        Month += 10 ;
                    }
                    else if(Month>=3 && Month<=10)
                    {
                        Month = 1;
                    }
                    else if (Month == 11 || Month == 12)
                    {
                        Month -= 10 ;
                    }
                    break ;
                    case 10 :
                    if(Month==12)
                    {
                        Month = 1;
                    }
                    else
                    {
                        Month ++ ;
                    }
                    break;
                    case 12 :
                    Year += 1000;
                    break;
                    case 13 :
                    Year += 100;
                    break;
                    case 14 :
                    Year += 10;
                    break;
                    case 15 :
                    Year += 1;
                    break;
                    case 24 :
                    if(Hours>=0 && Hours <=13)
                    {
                        Hours +=10;
                    }
                    else if (Hours>=14 && Hours <=23)
                    {
                         Hours -=10;  
                    }
                    break;
                    case 25:
                    if(Hours>=0 && Hours <=22)
                    {
                        Hours ++;
                    }
                    else if (Hours == 23)
                    {
                         Hours = 0;  
                    }
                    break;
                    case 27 :
                    if(Minutes>=0 && Minutes <=49)
                    {
                        Minutes += 10;
                    }
                    else if (Minutes > 50)
                    {
                        Minutes -= 50; 
                    }
                    break;
                    case 28 :
                    if(Minutes>=0 && Minutes <=58)
                    {
                        Minutes ++;
                    }
                    else if (Minutes == 59)
                    {
                        Minutes = 0 ; 
                    }
                    break;
                    case 30 :
                    if(Seconds>=0 && Seconds <=49)
                    {
                        Seconds += 10;
                    }
                    else if (Seconds> 50)
                    {
                        Seconds-= 50; 
                    }
                    break;
                    case 31 :
                    if(Seconds>=0 && Seconds <=58)
                    {
                        Seconds ++;
                    }
                    else if (Seconds == 59)
                    {
                        Seconds = 0 ; 
                    }
                    break;
                }
            }
            break;

           case DOWN_SWITCH_VALUE :
            if(EditMode == Activated && OKState == Not_PRESSED )
            {
                if (CurrentRow == LCD_enuSecondRow)
                {

                }
                else
                {
                    CurrentRow = LCD_enuSecondRow;
                }
                LCD_enuSetCursorAsync (CurrentRow,CurrentCol,DummyCB);
               
            }
            else if (EditMode == Activated && OKState == FirstPRESSED )
            {
                CursorPos = (CurrentCol) + (CurrentRow*16);
                switch (CursorPos)
                {
                    case 6:
                    if (Day >=1 && Day <=9)
                    {
                       
                    }
                    else
                    {
                       Day -=10 ;
                    }
                    break;
                    case 7:
                    if( Day == 10 ||Day == 20 ||Day == 30 )
                    {
                       
                    }
                    else
                    {
                        Day -- ;
                    }
                    break;
                    case 9 :
                    if(Month >=1 && Month <=10)
                    {
                    
                    }
                    else if (Month == 11 || Month == 12)
                    {
                        Month -= 10 ;
                    }
                    break ;
                    case 10 :
                    if(Month == 1 || Month == 10)
                    {
                        
                    }
                    else
                    {
                        Month -- ;
                    }
                    break;
                    case 12 :
                    if (Year < 1000)
                    {

                    }
                    else
                    {
                     Year -= 1000;
                    }
                    break;
                    case 13 :
                   if (Year < 100)
                    {

                    }
                    else
                    {
                     Year -= 100;
                    }
                    break;
                    case 14 :
                    if (Year < 10)
                    {

                    }
                    else
                    {
                     Year -= 10;
                    }
                    break;
                    case 15 :
                   if (Year < 1)
                    {

                    }
                    else
                    {
                     Year --;
                    }
                    break;
                    case 24 :
                    if(Hours>=0 && Hours <=9)
                    {
                        
                    }
                    else if (Hours>=10 && Hours <=23)
                    {
                         Hours -=10;  
                    }
                    break;
                    case 25:
                    if(Hours == 0 || Hours == 10 || Hours == 20)
                    {
                        
                    }
                    else 
                    {
                         Hours --;
                    }
                    break;
                    case 27 :
                    if(Minutes>=0 && Minutes <=9)
                    {
                        
                    }
                    else
                    {
                        Minutes -=10 ; 
                    }
                    break;
                    case 28 :
                    if(Minutes % 10 == 0)
                    {
                        
                    }
                    else
                    {
                        Minutes -- ; 
                    }
                    break;
                    case 30 :
                   if(Seconds>=0 && Seconds <=9)
                    {
                        
                    }
                    else
                    {
                        Seconds -=10 ; 
                    }
                    break;
                    case 31 :
                     if(Seconds % 10 == 0)
                    {
                        
                    }
                    else
                    {
                        Seconds -- ; 
                    }
                    break;
                }
            }
            break;
            case RIGHT_SWITCH_VALUE :
            if(EditMode == Activated)
            {
                if(CurrentCol == LCD_enuColumn_16 ||CurrentCol == LCD_enuColumn_32)
                {

                }
                else
                {
                    LCD_enuSetCursorAsync (CurrentRow,CurrentCol+1,DummyCB);
                }
            }
            break;
            case LEFT_SWITCH_VALUE :
            if(EditMode == Activated)
            {
                if(CurrentCol == LCD_enuColumn_1 ||CurrentCol == LCD_enuColumn_17)
                {

                }
                else
                {
                    LCD_enuSetCursorAsync (CurrentRow,CurrentCol-1,DummyCB);
                }
            }
            break;
             case OK_SWITCH_VALUE :
            if(EditMode == Activated)
            {
               if (OKState == Not_PRESSED)
               {
                OKState = FirstPRESSED;
               }
               else if (OKState == FirstPRESSED)
               {
                OKState = Not_PRESSED;
               }
            }
            break;
             case MODE_SWITCH_VALUE :
              if(EditMode == Not_Activated)
              {
               if (Mode == Clock_Mode)
               {
                Mode = Stopwatch_Mode;
               }
               else if (Mode == Stopwatch_Mode)
               {
                Mode == Clock_Mode;
               }
              }
            break;
            case EDIT_SWITCH_VALUE :
            EditMode = Activated ;
            LCD_enuSetCursorAsync  (LCD_enuFirstRow,LCD_enuColumn_1 ,DummyCB);
             break;
        }
    }

 }
}
}