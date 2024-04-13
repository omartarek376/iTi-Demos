#ifndef D__PIO_ARM_WS_SERVICES_SCHED_DRIVER_INC_MCAL_MUSART_MUSART_CONFIG_H_
#define D__PIO_ARM_WS_SERVICES_SCHED_DRIVER_INC_MCAL_MUSART_MUSART_CONFIG_H_

#include "MCAL/MUSART/MUSART_interface.h" 

#define SYSCLK_FREQUENCY   16000000UL            // Example: 16 MHz

/********************************************Configration For USART1*******************************************/
#define BAUD_RATE_1        9600                // Example: 9600 baud
/*
 * OPTIONS :
 *           USART_ENABLE   
 *           USART_DISABLE  
 */
#define  USART1_MODE                   USART_ENABLE

/*
 * OPTIONS :
 *           USART_8Bit_WORDLENGTH   
 *           USART_9Bit_WORDLENGTH   
 */
#define  USART1_WORD_LENGTH            USART_8Bit_WORDLENGTH

/*
 * OPTIONS :
 *           USART_OVER_SAMPLING_16   
 *           USART_OVER_SAMPLING_8   
 */
#define  USART1_SAMPLING            USART_OVER_SAMPLING_8

/*
 * OPTIONS :
 *           USART_PARITY_DISABLE  
 *           USART_PARITY_ENABLE   
 */
#define  USART1_PARITY            USART_PARITY_DISABLE

/*
 * OPTIONS :
 *           USART_EVEN_PARITY   
 *           USART_ODD_PARITY    
 */
#define  USART1_PARITY_MODE            USART_EVEN_PARITY

/*
 * OPTIONS :
 *           USART_STOPBIT_HALF   
 *           USART_STOPBIT_ONE
 *           USART_STOPBIT_ONEHALF
 *           USART_STOPBIT_TWO   
 */
#define  USART1_STOP_BITS              USART_STOPBIT_ONE

/****************************************Configration for UART2 **********************************************/
#define BAUD_RATE_2        9600                // Example: 9600 baud

/*
 * OPTIONS :
 *           USART_ENABLE   
 *           USART_DISABLE  
 */
#define  USART2_MODE                   USART_DISABLE 

/*
 * OPTIONS :
 *           USART_8Bit_WORDLENGTH   
 *           USART_9Bit_WORDLENGTH   
 */
#define  USART2_WORD_LENGTH            USART_8Bit_WORDLENGTH

/*
 * OPTIONS :
 *           USART_OVER_SAMPLING_16   
 *           USART_OVER_SAMPLING_8   
 */
#define  USART2_SAMPLING            USART_OVER_SAMPLING_16 

/*
 * OPTIONS :
 *           USART_PARITY_DISABLE  
 *           USART_PARITY_ENABLE   
 */
#define  USART2_PARITY            USART_PARITY_DISABLE

/*
 * OPTIONS :
 *           USART_EVEN_PARITY   
 *           USART_ODD_PARITY    
 */
#define  USART2_PARITY_MODE            USART_EVEN_PARITY

/*
 * OPTIONS :
 *           USART_STOPBIT_HALF   
 *           USART_STOPBIT_ONE
 *           USART_STOPBIT_ONEHALF
 *           USART_STOPBIT_TWO   
 */
#define  USART2_STOP_BITS              USART_STOPBIT_ONE

/****************************************Configration for UART6 **********************************************/
#define BAUD_RATE_6        9600                // Example: 9600 baud


/*
 * OPTIONS :
 *           USART_ENABLE   
 *           USART_DISABLE  
 */
#define  USART6_MODE                   USART_DISABLE 

/*
 * OPTIONS :
 *           USART_8Bit_WORDLENGTH   
 *           USART_9Bit_WORDLENGTH   
 */
#define  USART6_WORD_LENGTH            USART_8Bit_WORDLENGTH

/*
 * OPTIONS :
 *           USART_OVER_SAMPLING_16   
 *           USART_OVER_SAMPLING_8   
 */
#define  USART6_SAMPLING            USART_OVER_SAMPLING_16 

/*
 * OPTIONS :
 *           USART_PARITY_DISABLE  
 *           USART_PARITY_ENABLE   
 */
#define  USART6_PARITY            USART_PARITY_DISABLE

/*
 * OPTIONS :
 *           USART_EVEN_PARITY   
 *           USART_ODD_PARITY    
 */
#define  USART6_PARITY_MODE            USART_EVEN_PARITY

/*
 * OPTIONS :
 *           USART_STOPBIT_HALF   
 *           USART_STOPBIT_ONE
 *           USART_STOPBIT_ONEHALF
 *           USART_STOPBIT_TWO   
 */
#define  USART6_STOP_BITS              USART_STOPBIT_ONE
















#endif // D__PIO_ARM_WS_SERVICES_SCHED_DRIVER_INC_MCAL_MUSART_MUSART_CONFIG_H_