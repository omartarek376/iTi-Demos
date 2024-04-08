#ifndef D__PIO_ARM_WS_SERVICES_SCHED_DRIVER_INC_MCAL_MUSART_MUSART_PRIV_H_
#define D__PIO_ARM_WS_SERVICES_SCHED_DRIVER_INC_MCAL_MUSART_MUSART_PRIV_H_



/********************************************************************************************************/
/************************************************Includes************************************************/
/********************************************************************************************************/
//#include "MCAL/MUSART/MUSART_config.h"

/********************************************************************************************************/
/************************************************Defines*************************************************/
/********************************************************************************************************/
#define USART1_BASE_ADDRESS 0x40011000
#define USART2_BASE_ADDRESS 0x40004400
#define USART6_BASE_ADDRESS 0x40011400


#define USART_STOPBIT_CLR_MASK         0xCFFF 
#define USART_BRR_CLR_MASK             0xFFFF0000

/********************************************************************************************************/
/************************************************Types***************************************************/
/********************************************************************************************************/
typedef void(*TxCB)(void);
typedef void(*RxCB)(void);
/********************************************************************************************************/
/************************************************APIs****************************************************/
/********************************************************************************************************/

// // Calculate USARTDIV
// #define OVERSAMPLING_RATIO_1  USART1_SAMPLING ? 8 : 16 
// #define USARTDIV_1       ((SYSCLK_FREQUENCY) / (OVERSAMPLING_RATIO_1 * (BAUD_RATE_1)))
// #define DIV_Mantissa_1   (USARTDIV_1 / OVERSAMPLING_RATIO_1 )
// #define DIV_Fraction_1   ((u32)(((USARTDIV_1 - DIV_Mantissa_1) * 16) + 0.5)) // Round to nearest integer

// // Calculate USARTDIV
// #define OVERSAMPLING_RATIO_2  USART2_SAMPLING ? 8 : 16 
// #define USARTDIV_2       ((SYSCLK_FREQUENCY) / (OVERSAMPLING_RATIO_2 * (BAUD_RATE_2)))
// #define DIV_Mantissa_2  (USARTDIV_2 / OVERSAMPLING_RATIO_2 )
// #define DIV_Fraction_2   ((u32)(((USARTDIV_2 - DIV_Mantissa_2) * 16) + 0.5)) // Round to nearest integer


// // Calculate USARTDIV
// #define OVERSAMPLING_RATIO_6  USART6_SAMPLING ? 8 : 16 
// #define USARTDIV_6       ((SYSCLK_FREQUENCY) / (OVERSAMPLING_RATIO_6 * (BAUD_RATE_6)))
// #define DIV_Mantissa_6  (USARTDIV_6 / OVERSAMPLING_RATIO_6 )
// #define DIV_Fraction_6   ((u32)(((USARTDIV_6 - DIV_Mantissa_6) * 16) + 0.5)) // Round to nearest integer















#endif // D__PIO_ARM_WS_SERVICES_SCHED_DRIVER_INC_MCAL_MUSART_MUSART_PRIV_H_