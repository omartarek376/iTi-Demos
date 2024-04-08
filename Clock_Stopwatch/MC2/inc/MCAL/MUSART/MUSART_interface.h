#ifndef D__PIO_ARM_WS_SERVICES_SCHED_DRIVER_INC_MCAL_MUSART_MUSART_INTERFACE_H_
#define D__PIO_ARM_WS_SERVICES_SCHED_DRIVER_INC_MCAL_MUSART_MUSART_INTERFACE_H_

/********************************************************************************************************/
/************************************************Includes************************************************/
/********************************************************************************************************/
#include "LIB/STD_TYPES.h"
#include "CFG/MUSART/MUSART_config.h"
#include "MCAL/MUSART/MUSART_priv.h"

/********************************************************************************************************/
/************************************************Defines*************************************************/
/********************************************************************************************************/
#define USART_DISABLE  0
#define USART_ENABLE   1

#define USART_1  0
#define USART_2  1
#define USART_6  2

#define USART_8Bit_WORDLENGTH           0
#define USART_9Bit_WORDLENGTH           1

#define USART_OVER_SAMPLING_16          0
#define USART_OVER_SAMPLING_8           1

#define USART_PARITY_DISABLE            0
#define USART_PARITY_ENABLE             1

#define USART_EVEN_PARITY               0
#define USART_ODD_PARITY                1

#define  USART_STOPBIT_HALF        0x00001000
#define  USART_STOPBIT_ONE         0x00000000
#define  USART_STOPBIT_ONEHALF     0x00003000
#define  USART_STOPBIT_TWO         0x00002000

/********************************************************************************************************/
/************************************************Types***************************************************/
/********************************************************************************************************/
typedef enum
{
    MUSART_enuOK                 , // Enumeration for operation success
    MUSART_enuNOK                , // Enumeration for operation failure
    MUSART_enuWRONG_UARTID       ,
    MUSART_enuNULL_POINTER       ,
   
}MUSART_enuErrorStatus;            // Enumeration for error status


/********************************************************************************************************/
/************************************************APIs****************************************************/
/********************************************************************************************************/
/**
 * @brief Initializes and configures USART communication channels based on specified parameters.
 *
 * This function initializes USART communication channels according to the configurations provided
 * in the preprocessor directives. It configures baud rate, word length, parity, stop bits, and 
 * oversampling ratio for each USART channel enabled in the configuration. The function calculates
 * appropriate register values for baud rate setting and configures the USART control registers 
 * accordingly.
 *
 * @note This function assumes that the USART peripherals and their associated GPIO pins are 
 *       appropriately configured prior to its invocation.
 */
void MUSART_enuInit(void);

/**
 * @brief Sends a byte synchronously over a specified USART channel.
 *
 * This function sends a byte over the USART channel specified by USART_ID. It checks if the provided
 * USART_ID corresponds to USART1, USART2, or USART6. If the USART_ID is valid, the function writes 
 * the byte to the data register of the respective USART channel and waits until the transmission 
 * is complete before returning. If the USART_ID is invalid, it returns an error status 
 * indicating the wrong USART ID.
 *
 * @param[in]  USART_ID      The ID of the USART channel to send the byte over (USART_1, USART_2, or USART_6).
 * @param[in]  USART_BYTE    The byte to be sent over the USART channel.
 *
 * @return MUSART_enuErrorStatus An enumeration representing the status of the USART operation:
 *         - MUSART_enuOK if the operation was successful.
 *         - MUSART_enuWRONG_UARTID if the provided USART_ID is invalid.
 */
MUSART_enuErrorStatus MUSART_enuSendByteSync(u32 USART_ID, u8 USART_BYTE);

/**
 * @brief Receives a byte synchronously from a specified USART channel.
 *
 * This function receives a byte from the USART channel specified by USART_ID. It checks if the provided
 * USART_ID corresponds to USART1, USART2, or USART6. If the USART_ID is valid, the function waits until
 * a byte is received (RXNE flag is set) and reads the received byte from the data register of the 
 * respective USART channel. If the USART_ID is invalid, it returns an error status indicating the 
 * wrong USART ID.
 *
 * @param[in]  USART_ID        The ID of the USART channel to receive the byte from (USART_1, USART_2, or USART_6).
 * @param[out] USART_BYTE      Pointer to a variable where the received byte will be stored.
 *
 * @return MUSART_enuErrorStatus An enumeration representing the status of the USART operation:
 *         - MUSART_enuOK if the operation was successful.
 *         - MUSART_enuWRONG_UARTID if the provided USART_ID is invalid.
 */
MUSART_enuErrorStatus MUSART_enuRecieveByteSync(u32 USART_ID, u8* USART_BYTE);

/**
 * @brief Sends a buffer of data asynchronously over a specified USART channel.
 *
 * This function initiates the asynchronous transmission of a buffer of data over the USART channel
 * specified by USART_ID. It checks if the provided USART_ID corresponds to USART1, USART2, or USART6.
 * If the USART_ID is valid and the provided buffer and callback function pointers are not NULL, the
 * function configures the USART transmission interrupt and transmission complete interrupt to handle
 * the asynchronous transmission process. It then starts sending the data from the buffer asynchronously.
 * If the transmission buffer is already busy, the function returns an appropriate error status.
 *
 * @param[in]  USART_ID      The ID of the USART channel for asynchronous transmission (USART_1, USART_2, or USART_6).
 * @param[in]  USART_BUFFER  Pointer to the buffer containing the data to be transmitted.
 * @param[in]  LENGTH        The length of the data buffer to be transmitted.
 * @param[in]  CB            The callback function to be called upon completion of the transmission.
 *
 * @return MUSART_enuErrorStatus An enumeration representing the status of the USART operation:
 *         - MUSART_enuOK if the operation was successful.
 *         - MUSART_enuWRONG_UARTID if the provided USART_ID is invalid.
 *         - MUSART_enuNULL_POINTER if the provided USART_BUFFER or CB is NULL.
 *         - TX_STATE_BUSY if the transmission buffer is already busy.
 */
MUSART_enuErrorStatus MUSART_enuSendBufferAsync(u32 USART_ID,u8* USART_BUFFER,u16 LENGTH,TxCB CB);

/**
 * @brief Receives a buffer of data asynchronously from a specified USART channel.
 *
 * This function initiates the asynchronous reception of a buffer of data from the USART channel specified
 * by USART_ID. It checks if the provided USART_ID corresponds to USART1, USART2, or USART6. If the USART_ID
 * is valid and the provided buffer and callback function pointers are not NULL, the function configures
 * the USART receive interrupt to handle the asynchronous reception process. It then starts receiving data
 * into the buffer asynchronously. If the reception buffer is already busy, the function returns an
 * appropriate error status.
 *
 * @param[in]  USART_ID      The ID of the USART channel for asynchronous reception (USART_1, USART_2, or USART_6).
 * @param[out] USART_BUFFER  Pointer to the buffer where the received data will be stored.
 * @param[in]  LENGTH        The length of the data buffer to be received.
 * @param[in]  CB            The callback function to be called upon completion of the reception.
 *
 * @return MUSART_enuErrorStatus An enumeration representing the status of the USART operation:
 *         - MUSART_enuOK if the operation was successful.
 *         - MUSART_enuWRONG_UARTID if the provided USART_ID is invalid.
 *         - MUSART_enuNULL_POINTER if the provided USART_BUFFER or CB is NULL.
 *         - RX_STATE_BUSY if the reception buffer is already busy.
 */
MUSART_enuErrorStatus MUSART_enuRecieveBufferAsync(u32 USART_ID,u8* USART_BUFFER,u16 LENGTH,RxCB CB);
void MUSART_vTxDone(void);
void MUSART_vIsRx(void);
















#endif 