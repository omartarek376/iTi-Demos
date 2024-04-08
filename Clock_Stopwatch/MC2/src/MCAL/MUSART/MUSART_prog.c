/********************************************************************************************************/
/************************************************Includes************************************************/
/********************************************************************************************************/
#include "LIB/STD_TYPES.h"
#include "MCAL/MUSART/MUSART_interface.h"
#include "CFG/MUSART/MUSART_config.h"
#include "MCAL/MUSART/MUSART_priv.h"


/********************************************************************************************************/
/************************************************Defines*************************************************/
/********************************************************************************************************/

#define SR_RXNE     5
#define SR_TXE      7 



#define CR1_OVER8               15          /* 0: oversampling by 16, 1: oversampling by 8 */
#define CR1_UE                  13
#define CR1_M                   12          /* 0: 8 bits data, 1: 9 bits data */
#define CR1_WAKE                11         
#define CR1_PCE                 10
#define CR1_PS                  9           /* 0: even parity, 1: odd parity*/
#define CR1_PEIE                8
#define CR1_TXIE                7
#define CR1_TCIE                6
#define CR1_RXNEIE              5
#define CR1_IDLEIE              4
#define CR1_TE                  3
#define CR1_RE                  2
#define CR1_RWU                 1           
#define CR1_SBK                 0          

#define BRR_DIVFRACTION                 0
#define BRR_MANTISSA                    4

#define  SR_TXE_MASK               0x80
#define  SR_RXNE_MASK              0x20

#define USART_FRACTION_OVERFLOW_LIMIT 0x10
#define USART_SECOND_BYTE_MASK        0xF0
/********************************************************************************************************/
/************************************************Types***************************************************/
/********************************************************************************************************/
typedef struct {
    u32 SR;
    u32 DR;
    u32 BRR;
    u32 CR1;
    u32 CR2;
    u32 CR3;
    u32 GTPR;
} USARTReg_t;

 static USARTReg_t* const USART_PORTS[] = 
{
    (USARTReg_t* const)(USART1_BASE_ADDRESS),
    (USARTReg_t* const)(USART2_BASE_ADDRESS),
    (USARTReg_t* const)(USART6_BASE_ADDRESS)
};

typedef struct
{
    u8*  Data;
    u32  Pos;
    u32  Size;
}Buffer_t;

typedef struct
{
    Buffer_t Buffer;
    u32      State;
    TxCB     CB;
}TxBuffer_t;

typedef struct
{
    Buffer_t Buffer;
    u32      State;
    RxCB     CB;
}RxBuffer_t;

typedef enum {
    TX_STATE_READY,
    TX_STATE_BUSY,
} TxState_t;
typedef enum {
    RX_STATE_READY,
    RX_STATE_BUSY,
} RxState_t;

/********************************************************************************************************/
/************************************************Variables***********************************************/
/********************************************************************************************************/
// Initialize TxBuffer with the state set to ready
static TxBuffer_t TxBuffer = {
    .Buffer = { .Data = NULL, .Pos = 0, .Size = 0 },        // Initialize buffer parameters
    .State = TX_STATE_READY,                                // Set the state to ready
    .CB = NULL                                              // Initialize callback function pointer
};
static RxBuffer_t RxBuffer = {
    .Buffer = { .Data = NULL, .Pos = 0, .Size = 0 },       // Initialize buffer parameters
    .State = RX_STATE_READY,                               // Set the state to ready
    .CB = NULL                                             // Initialize callback function pointer
};
/********************************************************************************************************/
/*****************************************Static Functions Prototype*************************************/
/********************************************************************************************************/



/********************************************************************************************************/
/*********************************************APIs Implementation****************************************/
/********************************************************************************************************/
void MUSART_enuInit(void)
{
     #if USART1_MODE == USART_ENABLE // Check if USART1 is enabled in the configuration

     u32  LOC_u32RegValue1 = 0; // Initialize a local variable to hold register values for USART1
     volatile USARTReg_t* USART1 = USART_PORTS[USART_1]; // Get the pointer to USART1 registers

    u32 USARTDIV_1=0;    
    u32 DIV_Mantissa_1=0;
    u32 DIV_Fraction_1=0;

// Check if the baud rate is not zero
if (BAUD_RATE_1 != 0) {
    // Calculate the USART divider value (USARTDIV_1) to achieve the desired baud rate
    USARTDIV_1 = (((u64)SYSCLK_FREQUENCY * 1000) / (BAUD_RATE_1 * (8 * (2 - USART1_SAMPLING))));

    // Extract the mantissa part from the USART divider
    DIV_Mantissa_1 = USARTDIV_1 / 1000;

    // Calculate the fractional part of the USART divider
    DIV_Fraction_1 = (USARTDIV_1 % 1000) * (8 * (2 - USART1_SAMPLING));

    // Check if the fractional part needs to be rounded up
    if (DIV_Fraction_1 % 1000 >= 500) {
        // Round up the fractional part
        DIV_Fraction_1 = (DIV_Fraction_1 / 1000) + 1;
    } else {
        // Truncate the fractional part
        DIV_Fraction_1 = DIV_Fraction_1 / 1000;
    }

    // Check if the rounded fractional part exceeds the overflow limit
    if (DIV_Fraction_1 >= USART_FRACTION_OVERFLOW_LIMIT) {
        // Adjust the mantissa to account for the overflow
        DIV_Mantissa_1 += DIV_Fraction_1 & USART_SECOND_BYTE_MASK;
    }
}
LOC_u32RegValue1 =  USART1 -> BRR;
LOC_u32RegValue1 &=  USART_BRR_CLR_MASK;
LOC_u32RegValue1 |= DIV_Mantissa_1 << BRR_MANTISSA ;
LOC_u32RegValue1 |= DIV_Fraction_1 << BRR_DIVFRACTION  ;
USART1 -> BRR = LOC_u32RegValue1;

    // Enable USART1 and configure its control registers
     USART1 -> CR1  |= (1 << CR1_UE );     // Enable USART1 (USART Enable)
     USART1 -> CR1  |= (1 << CR1_TE );     // Enable USART1 transmitter (Transmitter Enable)
     USART1 -> CR1  |= (1 << CR1_RE );     // Enable USART1 receiver (Receiver Enable)

     
     #if   USART1_WORD_LENGTH == USART_8Bit_WORDLENGTH 
         USART1->CR1 &= ~(1 << CR1_M); // Set word length to 8 bits
     #elif USART1_WORD_LENGTH == USART_9Bit_WORDLENGTH
         USART1 -> CR1 |= (1 << CR1_M ); // Set word length to 9 bits
     #endif
     
     #if  USART1_SAMPLING == USART_OVER_SAMPLING_16  
         USART1->CR1 &= ~(1 << CR1_OVER8); // Set over-sampling ratio to 16
     #elif USART1_SAMPLING == USART_OVER_SAMPLING_8
         USART1 -> CR1 |= (1 << CR1_OVER8 ); // Set over-sampling ratio to 8
     #endif

     #if  USART1_PARITY  == USART_PARITY_DISABLE 
         USART1-> CR1 &= ~(1 << CR1_PCE); // Disable parity
     #elif USART1_PARITY  == USART_PARITY_ENABLE
         USART1 -> CR1 |= (1 << CR1_PCE ); // Enable parity
         if(USART1_PARITY_MODE == USART_EVEN_PARITY)
         {
            USART1-> CR1 &= ~(1 << CR1_PS); // Set even parity
         }
         else
         {
           USART1 -> CR1 |= (1 << CR1_PS ); // Set odd parity
         }
     #endif
      
      // Configure USART1 stop bits
      LOC_u32RegValue1  =  USART1 -> CR2;
      LOC_u32RegValue1 &= USART_STOPBIT_CLR_MASK;
      LOC_u32RegValue1 |= USART1_STOP_BITS ;
      USART1 -> CR2 = LOC_u32RegValue1;
#endif

    #if USART2_MODE == USART_ENABLE
    // Initialize USART2 if enabled
    u32 LOC_u32RegValue2 = 0;
    USARTReg_t* USART2 = USART_PORTS[USART_2]; // Get the USART2 port

    u32 USARTDIV_2=0;    
    u32 DIV_Mantissa_2=0;
    u32 DIV_Fraction_2=0;

    // Check if the baud rate is not zero
if (BAUD_RATE_2 != 0) {
    // Calculate the USART divider value (USARTDIV_1) to achieve the desired baud rate
    USARTDIV_2 = (((u64)SYSCLK_FREQUENCY * 1000) / (BAUD_RATE_2 * (8 * (2 - USART2_SAMPLING))));

    // Extract the mantissa part from the USART divider
    DIV_Mantissa_2 = USARTDIV_2 / 1000;

    // Calculate the fractional part of the USART divider
    DIV_Fraction_2 = (USARTDIV_2 % 1000) * (8 * (2 - USART2_SAMPLING));

    // Check if the fractional part needs to be rounded up
    if (DIV_Fraction_2 % 1000 >= 500) {
        // Round up the fractional part
        DIV_Fraction_2 = (DIV_Fraction_2 / 1000) + 1;
    } else {
        // Truncate the fractional part
        DIV_Fraction_2 = DIV_Fraction_2 / 1000;
    }

    // Check if the rounded fractional part exceeds the overflow limit
    if (DIV_Fraction_2 >= USART_FRACTION_OVERFLOW_LIMIT) {
        // Adjust the mantissa to account for the overflow
        DIV_Mantissa_2 += DIV_Fraction_2 & USART_SECOND_BYTE_MASK;
    }
}
LOC_u32RegValue2 =  USART2 -> BRR;
LOC_u32RegValue2 &=  USART_BRR_CLR_MASK;
LOC_u32RegValue2 |= DIV_Mantissa_2 << BRR_MANTISSA ;
LOC_u32RegValue2 |= DIV_Fraction_2 << BRR_DIVFRACTION  ;
USART2 -> BRR = LOC_u32RegValue2;


    // Enable USART2 and configure its control registers
    USART2->CR1 |= (1 << CR1_UE);       // Enable USART2 (USART Enable)
    USART2->CR1 |= (1 << CR1_TE);       // Enable USART2 transmitter (Transmitter Enable)
    USART2->CR1 |= (1 << CR1_RE);       // Enable USART2 receiver (Receiver Enable)

    // Configure USART2 word length
    #if USART2_WORD_LENGTH == USART_8Bit_WORDLENGTH
        USART2->CR1 &= ~(1 << CR1_M);   // Set USART2 to 8-bit data mode (Word Length = 8 bits)
    #elif USART2_WORD_LENGTH == USART_9Bit_WORDLENGTH
        USART2->CR1 |= (1 << CR1_M);    // Set USART2 to 9-bit data mode (Word Length = 9 bits)
    #endif

    // Configure USART2 sampling
    #if USART2_SAMPLING == USART_OVER_SAMPLING_16
        USART2->CR1 &= ~(1 << CR1_OVER8); // Set USART2 to oversampling by 16 (OVER8 = 0)
    #elif USART2_SAMPLING == USART_OVER_SAMPLING_8
        USART2->CR1 |= (1 << CR1_OVER8);  // Set USART2 to oversampling by 8 (OVER8 = 1)
    #endif

    // Configure USART2 parity
    #if USART2_PARITY == USART_PARITY_DISABLE
        USART2->CR1 &= ~(1 << CR1_PCE);  // Disable USART2 parity (Parity Control Enable)
    #elif USART2_PARITY == USART_PARITY_ENABLE
        USART2->CR1 |= (1 << CR1_PCE);   // Enable USART2 parity (Parity Control Enable)
        if (USART2_PARITY_MODE == USART_EVEN_PARITY)
        {
            USART2->CR1 &= ~(1 << CR1_PS); // Set USART2 to even parity mode (Parity Selection)
        }
        else
        {
            USART2->CR1 |= (1 << CR1_PS);  // Set USART2 to odd parity mode (Parity Selection)
        }
    #endif

    // Configure USART2 stop bits
    LOC_u32RegValue2 = USART2->CR2;
    LOC_u32RegValue2 &= USART_STOPBIT_CLR_MASK;  // Clear the stop bits field
    LOC_u32RegValue2 |= USART2_STOP_BITS;        // Set the stop bits field
    USART2->CR2 = LOC_u32RegValue2;
#endif


     #if USART6_MODE == USART_ENABLE
    // Initialize USART6 if enabled
    u32 LOC_u32RegValue6 = 0;
    USARTReg_t* USART6 = USART_PORTS[USART_6]; // Get the USART6 port

    
    u32 USARTDIV_6=0;    
    u32 DIV_Mantissa_6=0;
    u32 DIV_Fraction_6=0;

    // Check if the baud rate is not zero
if (BAUD_RATE_6 != 0) {
    // Calculate the USART divider value (USARTDIV_1) to achieve the desired baud rate
    USARTDIV_6 = (((u64)SYSCLK_FREQUENCY * 1000) / (BAUD_RATE_6 * (8 * (2 - USART6_SAMPLING))));

    // Extract the mantissa part from the USART divider
    DIV_Mantissa_6 = USARTDIV_6 / 1000;

    // Calculate the fractional part of the USART divider
    DIV_Fraction_6 = (USARTDIV_6 % 1000) * (8 * (2 - USART6_SAMPLING));

    // Check if the fractional part needs to be rounded up
    if (DIV_Fraction_6 % 1000 >= 500) {
        // Round up the fractional part
        DIV_Fraction_6 = (DIV_Fraction_6 / 1000) + 1;
    } else {
        // Truncate the fractional part
        DIV_Fraction_6 = DIV_Fraction_6 / 1000;
    }

    // Check if the rounded fractional part exceeds the overflow limit
    if (DIV_Fraction_6 >= USART_FRACTION_OVERFLOW_LIMIT) {
        // Adjust the mantissa to account for the overflow
        DIV_Mantissa_6 += DIV_Fraction_6 & USART_SECOND_BYTE_MASK;
    }
}
LOC_u32RegValue6 =  USART6 -> BRR;
LOC_u32RegValue6 &=  USART_BRR_CLR_MASK;
LOC_u32RegValue6 |= DIV_Mantissa_6 << BRR_MANTISSA ;
LOC_u32RegValue6 |= DIV_Fraction_6 << BRR_DIVFRACTION  ;
USART6 -> BRR = LOC_u32RegValue6;

    // Enable USART6 and configure its control registers
    USART6->CR1 |= (1 << CR1_UE);       // Enable USART6 (USART Enable)
    USART6->CR1 |= (1 << CR1_TE);       // Enable USART6 transmitter (Transmitter Enable)
    USART6->CR1 |= (1 << CR1_RE);       // Enable USART6 receiver (Receiver Enable)

    // Configure USART6 word length
    #if USART6_WORD_LENGTH == USART_8Bit_WORDLENGTH
        USART6->CR1 &= ~(1 << CR1_M);   // Set USART6 to 8-bit data mode (Word Length = 8 bits)
    #elif USART6_WORD_LENGTH == USART_9Bit_WORDLENGTH
        USART6->CR1 |= (1 << CR1_M);    // Set USART6 to 9-bit data mode (Word Length = 9 bits)
    #endif
     
    // Configure USART6 sampling
    #if USART6_SAMPLING == USART_OVER_SAMPLING_16
        USART6->CR1 &= ~(1 << CR1_OVER8); // Set USART6 to oversampling by 16 (OVER8 = 0)
    #elif USART6_SAMPLING == USART_OVER_SAMPLING_8
        USART6->CR1 |= (1 << CR1_OVER8);  // Set USART6 to oversampling by 8 (OVER8 = 1)
    #endif

    // Configure USART6 parity
    #if USART6_PARITY == USART_PARITY_DISABLE
        USART6->CR1 &= ~(1 << CR1_PCE);  // Disable USART6 parity (Parity Control Enable)
    #elif USART6_PARITY == USART_PARITY_ENABLE
        USART6->CR1 |= (1 << CR1_PCE);   // Enable USART6 parity (Parity Control Enable)
        if (USART6_PARITY_MODE == USART_EVEN_PARITY)
        {
            USART6->CR1 &= ~(1 << CR1_PS); // Set USART6 to even parity mode (Parity Selection)
        }
        else
        {
            USART6->CR1 |= (1 << CR1_PS);  // Set USART6 to odd parity mode (Parity Selection)
        }
    #endif
      
    // Configure USART6 stop bits
    LOC_u32RegValue6 = USART6->CR2;
    LOC_u32RegValue6 &= USART_STOPBIT_CLR_MASK;  // Clear the stop bits field
    LOC_u32RegValue6 |= USART6_STOP_BITS;        // Set the stop bits field
    USART6->CR2 = LOC_u32RegValue6;

    // Configure USART6 baud rate
    LOC_u32RegValue6 = USART6->BRR;
    LOC_u32RegValue6 &= USART_BRR_CLR_MASK;          // Clear the baud rate registers
    LOC_u32RegValue6 |= DIV_Mantissa_6 << BRR_MANTISSA; // Set the mantissa part of the baud rate
    LOC_u32RegValue6 |= DIV_Fraction_6 << BRR_DIVFRACTION; // Set the fraction part of the baud rate
    USART6->BRR = LOC_u32RegValue6;
#endif

}
MUSART_enuErrorStatus MUSART_enuSendByteSync(u32 USART_ID, u8 USART_BYTE)
{
    MUSART_enuErrorStatus Ret_enuUSART_ErrorStatus = MUSART_enuOK;
    if(USART_ID != USART_1 && USART_ID != USART_2 && USART_ID != USART_6)
    {
         Ret_enuUSART_ErrorStatus = MUSART_enuWRONG_UARTID;
    }
    else
    {
       volatile USARTReg_t* USART = USART_PORTS[USART_ID]; 
        USART->DR = USART_BYTE;
        while(!(USART->SR & SR_TXE_MASK));  
    }
    
    return Ret_enuUSART_ErrorStatus;
}
MUSART_enuErrorStatus MUSART_enuRecieveByteSync(u32 USART_ID, u8* USART_BYTE)
{
    MUSART_enuErrorStatus Ret_enuUSART_ErrorStatus = MUSART_enuOK;
    //u32 LOC_u32USART_INDEX = 0;

    if (USART_ID != USART_1 && USART_ID != USART_2 && USART_ID != USART_6)
    {
        Ret_enuUSART_ErrorStatus = MUSART_enuWRONG_UARTID;
    }
    else
    {
       volatile USARTReg_t* USART = USART_PORTS[USART_ID];

        // Wait until RXNE flag is set
        while (!(USART->SR & SR_RXNE_MASK));

        // Read the received byte from the data register
        *USART_BYTE = USART->DR;
    }

    return Ret_enuUSART_ErrorStatus;
}

MUSART_enuErrorStatus MUSART_enuSendBufferAsync(u32 USART_ID,u8* USART_BUFFER,u16 LENGTH,TxCB CB)
{
    MUSART_enuErrorStatus Ret_enuUSART_ErrorStatus = MUSART_enuOK;

    // Check if USART ID is valid
    if (USART_ID != USART_1 && USART_ID != USART_2 && USART_ID != USART_6)
    {
        Ret_enuUSART_ErrorStatus = MUSART_enuWRONG_UARTID;
    }
    else if( USART_BUFFER == NULL && CB == NULL )
    {
        Ret_enuUSART_ErrorStatus = MUSART_enuNULL_POINTER;
    }
    else
    {
       volatile USARTReg_t* USART = USART_PORTS[USART_ID];
       
        if(TxBuffer.State == TX_STATE_READY)
        { 
             TxBuffer.State = TX_STATE_BUSY;
             TxBuffer.Buffer.Data = USART_BUFFER;
             TxBuffer.Buffer.Pos = 0;
             TxBuffer.Buffer.Size = LENGTH;
             TxBuffer.CB = CB;       
             
             USART -> DR = TxBuffer.Buffer.Data[0];
             TxBuffer.Buffer.Pos ++ ;
             USART -> CR1   |= (1 << CR1_TXIE);    // Enable USART1 transmission interrupt (Transmit Interrupt Enable)
             USART -> CR1   |= (1 << CR1_TCIE );   // Enable USART1 transmission complete interrupt (Transmission Complete Interrupt Enable)
        }
        else
    {
        // TxBuffer is not ready for transmission, handle accordingly
        Ret_enuUSART_ErrorStatus = TX_STATE_BUSY;
    }

    }
    return Ret_enuUSART_ErrorStatus ;
}

MUSART_enuErrorStatus MUSART_enuRecieveBufferAsync(u32 USART_ID,u8* USART_BUFFER,u16 LENGTH,RxCB CB)
{
     MUSART_enuErrorStatus Ret_enuUSART_ErrorStatus = MUSART_enuOK;

    // Check if USART ID is valid
    if (USART_ID != USART_1 && USART_ID != USART_2 && USART_ID != USART_6)
    {
        Ret_enuUSART_ErrorStatus = MUSART_enuWRONG_UARTID;
    }
    else if (USART_BUFFER == NULL && CB == NULL)
    {
        Ret_enuUSART_ErrorStatus = MUSART_enuNULL_POINTER;
    }
    else
    {
         volatile USARTReg_t* USART = USART_PORTS[USART_ID];
       
        if(RxBuffer.State == RX_STATE_READY)
        {    
             //USART->SR &= ~(1 << SR_RXNE);
             RxBuffer.State = RX_STATE_BUSY;
             RxBuffer.Buffer.Data = USART_BUFFER;
             RxBuffer.Buffer.Pos = 0;
             RxBuffer.Buffer.Size = LENGTH;
             RxBuffer.CB = CB; 
             USART-> CR1  |= (1 << CR1_RXNEIE ); // Enable USART1 receive interrupt (Receive Data Register Not Empty Interrupt Enable)
        }
         else
        {
             // TxBuffer is not ready for transmission
             Ret_enuUSART_ErrorStatus = RX_STATE_BUSY;
        }

    }
     return Ret_enuUSART_ErrorStatus ;
}

void USART1_IRQHandler(void)
{   
   volatile USARTReg_t* USART1 = USART_PORTS[USART_1];
    if (USART1->SR & SR_TXE_MASK && TxBuffer.State == TX_STATE_BUSY)
    {
        if(TxBuffer.Buffer.Pos < TxBuffer.Buffer.Size)
        {  
               USART1->DR = TxBuffer.Buffer.Data[TxBuffer.Buffer.Pos];
               TxBuffer.Buffer.Pos++ ;
        } 
        else if (TxBuffer.Buffer.Pos == TxBuffer.Buffer.Size)
        {   
            //USART1->SR &= ~(1 << SR_TXE);     // Clear Transmit data register empty Flag
            //USART1->CR1 &= ~(1 << CR1_TCIE);   // Disable USART transmission complete interrupt
            USART1->CR1 &= ~(1 << CR1_TXIE);    // Disable USART transmission interrupt
            TxBuffer.State = TX_STATE_READY;
            if(TxBuffer.CB)
            {
            TxBuffer.CB();
            }

        }
        else
        {

        }
    }
    

    if(USART1->SR & SR_RXNE_MASK)
    {
        if(RxBuffer.Buffer.Pos < RxBuffer.Buffer.Size-1)
        {
              RxBuffer.Buffer.Data[RxBuffer.Buffer.Pos] = USART1 -> DR;
              RxBuffer.Buffer.Pos++ ;
        }
        else if (RxBuffer.Buffer.Pos == RxBuffer.Buffer.Size-1)
        {    
             RxBuffer.Buffer.Data[RxBuffer.Buffer.Pos] = USART1 -> DR;
             USART1->SR &= ~(1<<SR_RXNE);        // Clear Read data register not empty
             USART1->CR1 &= ~(1 << CR1_RXNEIE);  // Disable USART receive interrupt
             
             RxBuffer.State = RX_STATE_READY;
             if(RxBuffer.CB)
             {
                RxBuffer.CB();
             }
        }
    }
    
}
