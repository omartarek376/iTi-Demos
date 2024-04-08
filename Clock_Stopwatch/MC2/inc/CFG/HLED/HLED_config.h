#ifndef HAL_LED_HAL_config_H_
#define HAL_LED_HAL_config_H_



// Enumeration defining the LEDs used in the system
typedef enum
{
    Led_Start,          // Start of LED enumeration
    Led_GREEN,
    Led_RED,
    Led_YELLOW,
    Led_Switch,

    _Led_Num            // Total number of LEDs (must be last)
} LEDS_t;




#endif