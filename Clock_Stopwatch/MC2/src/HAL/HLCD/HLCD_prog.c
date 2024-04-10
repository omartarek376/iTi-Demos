
/******************************************************************************
 *
 * Module: 	LCD driver to: 	- Initialize GPIO pins to which the LCD is connected to.
 * 							- Displaying what the user wants.
 *
 * Supported Models:	- 16 x 2 Character LCD.
 *
 * File Name: LCD.c
 *
 * Description: Driver's APIs' implementation.
 *
 * Author: Shaher Shah Abdalla Kamal
 *
 * Date: 01-01-2024 (Edited for ARM at 18-03-2024)
 *
 *******************************************************************************/


/************************************************************************************/
/*										Includes									*/
/************************************************************************************/


#include "MCAL/MRCC/MRCC_interface.h"
#include "MCAL/MGPIO/MGPIO_interface.h"
#include "HAL/HLCD/HLCD_interface.h"



/* Check first if the user entered an invalid data bits mode */
#if ((LCD_DATA_BITS_MODE != LCD_FOUR_BITS_MODE) && (LCD_DATA_BITS_MODE != LCD_EIGHT_BITS_MODE))

#error "INVALID DATA BITS MODE is selected :(\nIt must be either LCD_FOUR_BITS_MODE or LCD_EIGHT_BITS_MODE"

#else


/************************************************************************************/
/*									extern Variables								*/
/************************************************************************************/

#if (LCD_DATA_BITS_MODE == LCD_EIGHT_BITS_MODE)
extern LCD_strLCDPinConfig_t arrayofLCDPinConfig [11];
#elif (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE)
extern LCD_strLCDPinConfig_t arrayofLCDPinConfig [7];
#endif



/************************************************************************************/
/*									Macros Declaration								*/
/************************************************************************************/


#define LCD_CGRAM_START_ADDRESS		0x40
#define LCD_DDRAM_START_ADDRESS		0x80

#define LCD_TWO_LINES_FOUR_BITS_MODE_INIT1		0x33
#define LCD_TWO_LINES_FOUR_BITS_MODE_INIT2		0x32

#define REMAINING_STAGES_4_BIT_MODE_CASE		9
#define REMAINING_STAGES_8_BIT_MODE_CASE		6



/************************************************************************************/
/*							User-defined types Declaration							*/
/************************************************************************************/


typedef struct{
    u8* string;
    u64 number;
    u8 state;
    u8 type;
    u8 cursorLocation;
}request_t;

typedef struct{
	void (*callBack)(void);
}process_t;


/* The possible states of the LCD */
enum{
	stateOff,
	stateInitialization,
	stateOperational
};

/* The possible states for this driver,
 * - ready means We are ready to receive a new request from the user
 * - busy means We aren't ready for a new request from the user */
enum{
	readyForRequest,
	busyWithRequest
};

/* The possible requests' types */
enum{
	reqNULL,
	reqClearScreen,
	reqSetCursor,
	reqWriteString,
	reqWriteNumber

};



/************************************************************************************/
/*								Variables's Declaration								*/
/************************************************************************************/


u8 lcdState = stateOff;

request_t userReq;

process_t writeProc;

process_t clearProc;

process_t setCursorProc;

process_t initProc;

process_t writeNumProc;



/************************************************************************************/
/*							Static Functions' Implementation						*/
/************************************************************************************/


/**
 *@brief : Process that writes a command on the data bus.
 *@param : A command.
 *@return: void.
 */
static void LCD_writeCommandSM(u8 Copy_uint8Command){

	static u8 entryCounter = 0;
	entryCounter++;

	switch(entryCounter){

	/* Set the value of RS pin as output low as we are sending a command */
	case 1:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[RS_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[RS_4BITMODE].LCD_pin_number,\
				GPIO_LOW);
		break;
		/* Set the value of R/w pin as output low as we are writing */
	case 2:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[RW_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[RW_4BITMODE].LCD_pin_number,\
				GPIO_LOW);
		break;
		/* Set the value of the E pin to be output low to make pulse on it later once we
		 * put the data on the data pins */
	case 3:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[E_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[E_4BITMODE].LCD_pin_number,\
				GPIO_LOW);
		break;

#if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE)

		/* Putting the upper 4-bits data on the data pins of the LCD */
	case 4:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D7_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[D7_4BITMODE].LCD_pin_number,\
				( ( Copy_uint8Command & (1<<D7) ) >> D7 ));
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D6_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[D6_4BITMODE].LCD_pin_number,\
				( ( Copy_uint8Command & (1<<D6) ) >> D6 ));
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D5_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[D5_4BITMODE].LCD_pin_number,\
				( ( Copy_uint8Command & (1<<D5) ) >> D5 ));
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D4_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[D4_4BITMODE].LCD_pin_number,\
				( ( Copy_uint8Command & (1<<D4) ) >> D4 ));
		break;
		/* Send a pulse to the E pin to send the data to the LCD */
		/* Set the value of the E pin to be output high */
	case 5:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[E_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[E_4BITMODE].LCD_pin_number,\
				GPIO_HIGH);
		break;
		/* Set the value of the E pin to be output low */
	case 6:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[E_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[E_4BITMODE].LCD_pin_number,\
				GPIO_LOW);
		break;
		/* Putting the lower 4-bits data on the data pins of the LCD */
	case 7:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D7_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[D7_4BITMODE].LCD_pin_number,\
				( ( Copy_uint8Command & (1<<D3) ) >> D3 ));
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D6_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[D6_4BITMODE].LCD_pin_number,\
				( ( Copy_uint8Command & (1<<D2) ) >> D2 ));
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D5_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[D5_4BITMODE].LCD_pin_number,\
				( ( Copy_uint8Command & (1<<D1) ) >> D1 ));
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D4_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[D4_4BITMODE].LCD_pin_number,\
				( ( Copy_uint8Command & (1<<D0) ) >> D0 ));
		break;
		/* Send a pulse to the E pin to send the data to the LCD */
		/* Set the value of the E pin to be output high */
	case 8:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[E_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[E_4BITMODE].LCD_pin_number,\
				GPIO_HIGH);
		break;
		/* Set the value of the E pin to be output low */
	case 9:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[E_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[E_4BITMODE].LCD_pin_number,\
				GPIO_LOW);

		entryCounter = 0;
		break;


#elif (LCD_DATA_BITS_MODE == LCD_EIGHT_BITS_MODE)

		/* Putting the data on the data pins of the LCD */
	case 4:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D7].LCD_port_number,\
				arrayofLCDPinConfig[D7].LCD_pin_number,\
				( ( Copy_uint8Command & (1<<D7) ) >> D7 ));
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D6].LCD_port_number,\
				arrayofLCDPinConfig[D6].LCD_pin_number,\
				( ( Copy_uint8Command & (1<<D6) ) >> D6 ));
		MGPIO_enuSetPinValueue(arrayofLCDPinConfig[D5].LCD_port_number,\
				arrayofLCDPinConfig[D5].LCD_pin_number,\
				( ( Copy_uint8Command & (1<<D5) ) >> D5 ));
		MGPIO_enuSetPinValueue(arrayofLCDPinConfig[D4].LCD_port_number,\
				arrayofLCDPinConfig[D4].LCD_pin_number,\
				( ( Copy_uint8Command & (1<<D4) ) >> D4 ));
		MGPIO_enuSetPinValueue(arrayofLCDPinConfig[D3].LCD_port_number,\
				arrayofLCDPinConfig[D3].LCD_pin_number,\
				( ( Copy_uint8Command & (1<<D3) ) >> D3 ));
		MGPIO_enuSetPinValueue(arrayofLCDPinConfig[D2].LCD_port_number,\
				arrayofLCDPinConfig[D2].LCD_pin_number,\
				( ( Copy_uint8Command & (1<<D2) ) >> D2 ));
		MGPIO_enuSetPinValueue(arrayofLCDPinConfig[D1].LCD_port_number,\
				arrayofLCDPinConfig[D1].LCD_pin_number,\
				( ( Copy_uint8Command & (1<<D1) ) >> D1 ));
		MGPIO_enuSetPinValueue(arrayofLCDPinConfig[D0].LCD_port_number,\
				arrayofLCDPinConfig[D0].LCD_pin_number,\
				( ( Copy_uint8Command & (1<<D0) ) >> D0 ));
		break;
		/* Send a pulse to the E pin to send the data to the LCD */
		/* Set the value of the E pin to be output high */
	case 5:
		MGPIO_enuSetPinValueue(arrayofLCDPinConfig[E_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[E_4BITMODE].LCD_pin_number,\
				GPIO_HIGH);
		break;
		/* Set the value of the E pin to be output low */
	case 6:
		MGPIO_enuSetPinValueue(arrayofLCDPinConfig[E_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[E_4BITMODE].LCD_pin_number,\
				GPIO_LOW);

		entryCounter = 0;
		break;

#endif /* #if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE) */

	default:
		/* Do Nothing */
		break;
	}
}


/**
 *@brief : Process that writes a data on the data bus.
 *@param : A data.
 *@return: void.
 */
static void LCD_writeDataSM(u8 Copy_uint8Data){
	static u8 entryCounter = 0;
	entryCounter++;

	switch(entryCounter){

	/* Set the value of RS pin as output high as we are sending data */
	case 1:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[RS_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[RS_4BITMODE].LCD_pin_number,\
				GPIO_HIGH);
		break;
		/* Set the value of R/w pin as output low as we are writing */
	case 2:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[RW_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[RW_4BITMODE].LCD_pin_number,\
				GPIO_LOW);
		break;
		/* Set the value of the E pin to be output low to make pulse on it later once we
		 * put the data on the data pins */
	case 3:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[E_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[E_4BITMODE].LCD_pin_number,\
				GPIO_LOW);
		break;

#if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE)

		/* Putting the upper 4-bits data on the data pins of the LCD */
	case 4:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D7_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[D7_4BITMODE].LCD_pin_number,\
				( ( Copy_uint8Data & (1<<D7) ) >> D7 ));
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D6_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[D6_4BITMODE].LCD_pin_number,\
				( ( Copy_uint8Data & (1<<D6) ) >> D6 ));
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D5_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[D5_4BITMODE].LCD_pin_number,\
				( ( Copy_uint8Data & (1<<D5) ) >> D5 ));
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D4_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[D4_4BITMODE].LCD_pin_number,\
				( ( Copy_uint8Data & (1<<D4) ) >> D4 ));
		break;
		/* Send a pulse to the E pin to send the data to the LCD */
		/* Set the value of the E pin to be output high */
	case 5:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[E_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[E_4BITMODE].LCD_pin_number,\
				GPIO_HIGH);
		break;
		/* Set the value of the E pin to be output low */
	case 6:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[E_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[E_4BITMODE].LCD_pin_number,\
				GPIO_LOW);
		break;
		/* Putting the lower 4-bits data on the data pins of the LCD */
	case 7:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D7_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[D7_4BITMODE].LCD_pin_number,\
				( ( Copy_uint8Data & (1<<D3) ) >> D3 ));
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D6_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[D6_4BITMODE].LCD_pin_number,\
				( ( Copy_uint8Data & (1<<D2) ) >> D2 ));
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D5_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[D5_4BITMODE].LCD_pin_number,\
				( ( Copy_uint8Data & (1<<D1) ) >> D1 ));
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D4_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[D4_4BITMODE].LCD_pin_number,\
				( ( Copy_uint8Data & (1<<D0) ) >> D0 ));
		break;
		/* Send a pulse to the E pin to send the data to the LCD */
		/* Set the value of the E pin to be output high */
	case 8:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[E_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[E_4BITMODE].LCD_pin_number,\
				GPIO_HIGH);
		break;
		/* Set the value of the E pin to be output low */
	case 9:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[E_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[E_4BITMODE].LCD_pin_number,\
				GPIO_LOW);

		entryCounter = 0;
		break;


#elif (LCD_DATA_BITS_MODE == LCD_EIGHT_BITS_MODE)

		/* Putting the data on the data pins of the LCD */
	case 4:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D7].LCD_port_number,\
				arrayofLCDPinConfig[D7].LCD_pin_number,\
				( ( Copy_uint8Data & (1<<D7) ) >> D7 ));
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D6].LCD_port_number,\
				arrayofLCDPinConfig[D6].LCD_pin_number,\
				( ( Copy_uint8Data & (1<<D6) ) >> D6 ));
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D5].LCD_port_number,\
				arrayofLCDPinConfig[D5].LCD_pin_number,\
				( ( Copy_uint8Data & (1<<D5) ) >> D5 ));
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D4].LCD_port_number,\
				arrayofLCDPinConfig[D4].LCD_pin_number,\
				( ( Copy_uint8Data & (1<<D4) ) >> D4 ));
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D3].LCD_port_number,\
				arrayofLCDPinConfig[D3].LCD_pin_number,\
				( ( Copy_uint8Data & (1<<D3) ) >> D3 ));
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D2].LCD_port_number,\
				arrayofLCDPinConfig[D2].LCD_pin_number,\
				( ( Copy_uint8Data & (1<<D2) ) >> D2 ));
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D1].LCD_port_number,\
				arrayofLCDPinConfig[D1].LCD_pin_number,\
				( ( Copy_uint8Data & (1<<D1) ) >> D1 ));
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[D0].LCD_port_number,\
				arrayofLCDPinConfig[D0].LCD_pin_number,\
				( ( Copy_uint8Data & (1<<D0) ) >> D0 ));
		break;
		/* Send a pulse to the E pin to send the data to the LCD */
		/* Set the value of the E pin to be output high */
	case 5:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[E_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[E_4BITMODE].LCD_pin_number,\
				GPIO_HIGH);
		break;
		/* Set the value of the E pin to be output low */
	case 6:
		MGPIO_enuSetPinValue(arrayofLCDPinConfig[E_4BITMODE].LCD_port_number,\
				arrayofLCDPinConfig[E_4BITMODE].LCD_pin_number,\
				GPIO_LOW);

		entryCounter = 0;
		break;

#endif /* #if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE) */

	default:
		/* Do Nothing */
		break;
	}
}


/**
 *@brief : Process that initializes the LCD.
 *@param : void.
 *@return: void.
 */
static void LCD_initProcSM(void){

	static u8 entryCounter = 0;

#if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE)
	static u8 writeCommandSM_remainingStages = REMAINING_STAGES_4_BIT_MODE_CASE;

#elif (LCD_DATA_BITS_MODE == LCD_EIGHT_BITS_MODE)
	static u8 writeCommandSM_remainingStages = REMAINING_STAGES_8_BIT_MODE_CASE;

#endif  /* #if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE) */

	if(entryCounter == 0){
		entryCounter++;
	}

	u8 LOC_uint8counter;
	MGPIO_PIN_config_t LOC_LCDPinConfig;


	switch(entryCounter){

#if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE)

	/* Setting the LCD's pins as output low */
	case 1:

		/* Setting the pins as output low pins */
		for(LOC_uint8counter = D4_4BITMODE; LOC_uint8counter <= E_4BITMODE; LOC_uint8counter++){
			/* First enable the GPIOx peripheral to which the current LED is connected */
			switch (arrayofLCDPinConfig[LOC_uint8counter].LCD_port_number){
			case GPIO_PORTA:
				RCC_enuEnableAHB1Peripheral(AHB1_GPIOA);
				break;
			case GPIO_PORTB:
				RCC_enuEnableAHB1Peripheral(AHB1_GPIOB);
				break;
			case GPIO_PORTC:
				RCC_enuEnableAHB1Peripheral(AHB1_GPIOC);
				break;
			case GPIO_PORTD:
				RCC_enuEnableAHB1Peripheral(AHB1_GPIOD);
				break;
			case GPIO_PORTE:
				RCC_enuEnableAHB1Peripheral(AHB1_GPIOE);
				break;
			case GPIO_PORTH:
				RCC_enuEnableAHB1Peripheral(AHB1_GPIOH);
				break;
			default:
				/* The code won't reach here as We already validated the input LED's PORT */
				break;
			}

			LOC_LCDPinConfig.GPIOPort = arrayofLCDPinConfig[LOC_uint8counter].LCD_port_number;
			LOC_LCDPinConfig.GPIOPin = arrayofLCDPinConfig[LOC_uint8counter].LCD_pin_number;
			LOC_LCDPinConfig.GPIOSpeed = GPIO_HIGH_SPEED;
			LOC_LCDPinConfig.GPIOMode = GPIO_OUTPUT;
			LOC_LCDPinConfig.GPIO_OUTPUTTYPE = GPIO_PUSHPULL;

			/* Passing the configurations assigned in "LOC_LEDconfig" to the GPIO initialize function
			 * to initialize the pin */
			MGPIO_enuSetPinConfig(&LOC_LCDPinConfig);

			MGPIO_enuSetPinValue(arrayofLCDPinConfig[LOC_uint8counter].LCD_port_number,\
					arrayofLCDPinConfig[LOC_uint8counter].LCD_pin_number,\
					GPIO_HIGH);
		}

		entryCounter++;
		break;
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
		entryCounter++;
		break;
		/* When coming here again, 30 milliseconds will have been passed (Power on), write command */
	case 31:

		if (writeCommandSM_remainingStages > 0){
			LCD_writeCommandSM(LCD_TWO_LINES_FOUR_BITS_MODE_INIT1);
			writeCommandSM_remainingStages--;
		}
		else{
			writeCommandSM_remainingStages = REMAINING_STAGES_4_BIT_MODE_CASE;
			entryCounter += 5;
		}
		break;
		/* When coming here again 5 milliseconds will have been passed, write command */
	case 36:

		if (writeCommandSM_remainingStages > 0){
			LCD_writeCommandSM(LCD_TWO_LINES_FOUR_BITS_MODE_INIT2);
			writeCommandSM_remainingStages--;
		}
		else{
			writeCommandSM_remainingStages = REMAINING_STAGES_4_BIT_MODE_CASE;
			entryCounter++;
		}
		break;
		/* Function set stage */
	case 37:
		/* use 2-lines LCD + 4-bits Data Mode + 5x7 dot display Mode */

		if (writeCommandSM_remainingStages > 0){
			LCD_writeCommandSM(LCD_FourBitMode_2LineDisplay_5x7);
			writeCommandSM_remainingStages--;
		}
		else{
			writeCommandSM_remainingStages = REMAINING_STAGES_4_BIT_MODE_CASE;
			entryCounter++;
		}
		break;
		/* Display control stage */
	case 38:
		/* Turn the display ON, turn the cursor ON, and turn the blinking on */

		if (writeCommandSM_remainingStages > 0){
			LCD_writeCommandSM(LCD_DisplayON_CursorON_BlinkON);
			writeCommandSM_remainingStages--;
		}
		else{
			writeCommandSM_remainingStages = REMAINING_STAGES_4_BIT_MODE_CASE;
			entryCounter++;
		}
		break;
		/* Clear display stage */
	case 39:

		if (writeCommandSM_remainingStages > 0){
			LCD_writeCommandSM(LCD_ClearDisplay);
			writeCommandSM_remainingStages--;
		}
		else{
			writeCommandSM_remainingStages = REMAINING_STAGES_4_BIT_MODE_CASE;
			entryCounter += 2;
		}
		break;
		/* When coming here again 2 milliseconds will have been passed, enter Entry mode set stage */
	case 41:

		if (writeCommandSM_remainingStages > 0){
			LCD_writeCommandSM(LCD_EntryMode_CursorIncrement_ShiftOFF);
			writeCommandSM_remainingStages--;
		}
		else{
			writeCommandSM_remainingStages = REMAINING_STAGES_4_BIT_MODE_CASE;
			entryCounter++;
		}
		break;
		/* We reach the end of initialization, change to readyForRequest, enter the stateOperational
		 * state, and call the passed callback function */
	case 42:
		entryCounter = 0;
		userReq.state = readyForRequest;
		lcdState = stateOperational;
		initProc.callBack();
		break;

#elif (LCD_DATA_BITS_MODE == LCD_EIGHT_BITS_MODE)

		/* Setting the LCD's pins as output low */
	case 1:

		/* Setting the pins as an output low pins */
		for(LOC_uint8counter = D0; LOC_uint8counter <= E; LOC_uint8counter++){
			/* First enable the GPIOx peripheral to which the current LED is connected */
			switch (arrayofLCDPinConfig[LOC_uint8counter].LCD_port_number){
			case GPIO_PORTA:
				RCC_enuEnableAHB1Peripheral(AHB1_GPIOA);
				break;
			case GPIO_PORTB:
				RCC_enuEnableAHB1Peripheral(AHB1_GPIOB);
				break;
			case GPIO_PORTC:
				RCC_enuEnableAHB1Peripheral(AHB1_GPIOC);
				break;
			case GPIO_PORTD:
				RCC_enuEnableAHB1Peripheral(AHB1_GPIOD);
				break;
			case GPIO_PORTE:
				RCC_enuEnableAHB1Peripheral(AHB1_GPIOE);
				break;
			case GPIO_PORTH:
				RCC_enuEnableAHB1Peripheral(AHB1_GPIOH);
				break;
			default:
				/* The code won't reach here as We already validated the input LED's PORT */
				break;
			}

			LOC_LCDPinConfig.GPIOPort = arrayofLCDPinConfig[LOC_uint8counter].LCD_port_number;
			LOC_LCDPinConfig.GPIOPin = arrayofLCDPinConfig[LOC_uint8counter].LCD_pin_number;
			LOC_LCDPinConfig.GPIOSpeed = GPIO_HIGH_SPEED;
			LOC_LCDPinConfig.GPIOMode = GPIO_OUTPUT;
			LOC_LCDPinConfig.GPIO_OUTPUTTYPE = GPIO_PUSHPULL;

			/* Passing the configurations assigned in "LOC_LEDconfig" to the GPIO initialize function
			 * to initialize the pin */
			MGPIO_enuSetPinConfig(&LOC_LCDPinConfig);

			MGPIO_enuSetPinValue(arrayofLCDPinConfig[LOC_uint8counter].LCD_port_number,\
					arrayofLCDPinConfig[LOC_uint8counter].LCD_pin_number,\
					GPIO_HIGH);
		}

		entryCounter++;
		break;
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
		entryCounter++;
		break;
		/* When coming here again 30 milliseconds will have been passed (Power on), enter
		 * Function set stage */
	case 31:
		/* use 2-lines LCD + 8-bits Data Mode + 5x7 dot display Mode */

		if (writeCommandSM_remainingStages > 0){
			LCD_writeCommandSM(LCD_EightBitMode_2LineDisplay_5x7);
			writeCommandSM_remainingStages--;
		}
		else{
			writeCommandSM_remainingStages = REMAINING_STAGES_8_BIT_MODE_CASE;
			entryCounter++;
		}
		break;
		/* Display control stage */
	case 32:
		/* Turn the display ON, turn the cursor ON, and turn the blinking on */

		if (writeCommandSM_remainingStages > 0){
			LCD_writeCommandSM(LCD_DisplayON_CursorOFF_BlinkOFF);
			writeCommandSM_remainingStages--;
		}
		else{
			writeCommandSM_remainingStages = REMAINING_STAGES_8_BIT_MODE_CASE;
			entryCounter++;
		}
		break;
		/* Clear display stage */
	case 33:

		if (writeCommandSM_remainingStages > 0){
			LCD_writeCommandSM(LCD_ClearDisplay);
			writeCommandSM_remainingStages--;
		}
		else{
			writeCommandSM_remainingStages = REMAINING_STAGES_8_BIT_MODE_CASE;
			entryCounter += 2;
		}
		break;
		/* When coming here again 2 milliseconds will have been passed, enter Entry mode set stage */
	case 35:

		if (writeCommandSM_remainingStages > 0){
			LCD_writeCommandSM(LCD_EntryMode_CursorIncrement_ShiftOFF);
			writeCommandSM_remainingStages--;
		}
		else{
			writeCommandSM_remainingStages = REMAINING_STAGES_8_BIT_MODE_CASE;
			entryCounter += 2;
		}
		break;
		/* We reach the end of initialization, change to readyForRequest, enter the stateOperational
		 * state, and call the passed callback function */
	case 36:
		entryCounter = 0;
		userReq.state = readyForRequest;
		lcdState = stateOperational;
		initProc.callBack();
		break;

#endif  /* #if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE) */


	default:
		/* Do Nothing */
		break;
	}
}


/**
 *@brief : Process that writes one character at a time.
 *@param : void.
 *@return: void.
 */
static void LCD_writeProc(void){
	static u8 entryCounter = 0;

#if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE)
	static u8 writeDataSM_remainingStages = REMAINING_STAGES_4_BIT_MODE_CASE;

#elif (LCD_DATA_BITS_MODE == LCD_EIGHT_BITS_MODE)
	static u8 writeDataSM_remainingStages = REMAINING_STAGES_8_BIT_MODE_CASE;

#endif  /* #if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE) */

#if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE)

	/* Check if We reached the NULL character or not */
	if(userReq.string[entryCounter] != '\0'){

		/* Check if We finished all stages of the LCD_writeDataSM or not */
		if(writeDataSM_remainingStages > 0){
			LCD_writeDataSM(userReq.string[entryCounter]);
			writeDataSM_remainingStages--;
		}
		else{
			/* We finished the printing of one character */
			writeDataSM_remainingStages = REMAINING_STAGES_4_BIT_MODE_CASE;
			entryCounter++;
		}
	}
	else{
		/* We finished the Printing of the Whole string */
		entryCounter = 0;
		userReq.type = NULL;
		userReq.state = readyForRequest;
		writeProc.callBack();
	}

#elif (LCD_DATA_BITS_MODE == LCD_EIGHT_BITS_MODE)

	/* Check if We reached the NULL character or not */
	if(userReq.string[entryCounter] != '\0'){

		/* Check if We finished all stages of the LCD_writeDataSM or not */
		if(writeDataSM_remainingStages > 0){
			LCD_writeDataSM(userReq.string[entryCounter]);
			writeDataSM_remainingStages--;
		}
		else{
			/* We finished the printing of one character */
			writeDataSM_remainingStages = REMAINING_STAGES_8_BIT_MODE_CASE;
			entryCounter++;
		}
	}
	else{
		/* We finished the Printing of the Whole string */
		entryCounter = 0;
		userReq.type = NULL;
		userReq.state = readyForRequest;
		writeProc.callBack();
	}

#endif  /* #if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE) */

}


/**
 *@brief : Process that writes one character at a time.
 *@param : void.
 *@return: void.
 */
static void LCD_writeNumProc(void) {

#if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE)
	static u8 writeDataSM_remainingStages = REMAINING_STAGES_4_BIT_MODE_CASE;

#elif (LCD_DATA_BITS_MODE == LCD_EIGHT_BITS_MODE)
	static u8 writeDataSM_remainingStages = REMAINING_STAGES_8_BIT_MODE_CASE;

#endif  /* #if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE) */

#if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE)

	/* We will need it because we will send the number digit by digit so We have to invert it first
	 * to be printed then in a correct manner. */
	static u64 LOC_uint64InvertedImage = 0;

	/* Its usage is to check if the unit digit in the input number is zero or not */
	static u8 LOC_uint8ZeroInUnitsChecker = 0;
	
	/* Its usage is to check if the input number is zero */
	static u8 LOC_uint8NumberIsZeroFlag = 0;

	/* Its usage is to check if the input number is not zero */
	static u8 LOC_uint8NumberIsNotZeroFlag = 0;

	/* Its usage is to check if the input number is zero whether We finished printing it or not */
	static u8 LOC_uint8ZeroIsPrintedFlag = 0;

	/* Check if the input is zero */
	if((userReq.number == 0) && (LOC_uint8NumberIsZeroFlag == 0) && (LOC_uint8NumberIsNotZeroFlag != 1)) {
		LOC_uint8NumberIsZeroFlag = 1;
	}

	if (LOC_uint8NumberIsZeroFlag == 1 && LOC_uint8ZeroIsPrintedFlag == 0) {
		/* Check if the input is zero, print it directly */

		/* Check if We finished all stages of the LCD_writeDataSM or not */
		if(writeDataSM_remainingStages > 0) {
			LCD_writeDataSM(0 + 48);
			writeDataSM_remainingStages--;
		}
		else{
			/* We finished the printing of one digit */
			writeDataSM_remainingStages = REMAINING_STAGES_4_BIT_MODE_CASE;
			LOC_uint8ZeroIsPrintedFlag = 1;
		}
	} else if (userReq.number != 0) {

		LOC_uint8NumberIsNotZeroFlag = 1;

		/* Save an inverted image of the input in a local variable to b able to print it properly */
		LOC_uint64InvertedImage *= 10;
		LOC_uint64InvertedImage += userReq.number % 10;

		/* If the unit digit in the input number is zero then raise the Zero checker flag */
		if (LOC_uint64InvertedImage == 0) {
			LOC_uint8ZeroInUnitsChecker = 1;
		}

		userReq.number /= 10;

	} else if (LOC_uint64InvertedImage != 0) {
		/* Send the inverted number to be printed digit-by-digit */

		/* Check if We finished all stages of the LCD_writeDataSM or not */
		if(writeDataSM_remainingStages > 0) {
			LCD_writeDataSM((LOC_uint64InvertedImage % 10) + 48);
			writeDataSM_remainingStages--;
		}
		else{
			/* We finished the printing of one digit */
			writeDataSM_remainingStages = REMAINING_STAGES_4_BIT_MODE_CASE;
			LOC_uint64InvertedImage /= 10;
		}
	}
	/* If the Zero checker flag is set then print Zero as the last thing to be printed */
	else if (LOC_uint8ZeroInUnitsChecker == 1)
	{
		/* Check if We finished all stages of the LCD_writeDataSM or not */
		if(writeDataSM_remainingStages > 0) {
			LCD_writeDataSM(0 + 48);
			writeDataSM_remainingStages--;
		}
		else{
			/* We finished the printing of one digit */
			writeDataSM_remainingStages = REMAINING_STAGES_4_BIT_MODE_CASE;
			LOC_uint8ZeroInUnitsChecker = 0;
		}
	} else {
		/* We finished the Printing of the Whole number */
		if (LOC_uint8NumberIsZeroFlag == 1) {
			LOC_uint8NumberIsZeroFlag = 0;
		}
		if (LOC_uint8ZeroIsPrintedFlag == 1) {
			LOC_uint8ZeroIsPrintedFlag = 0;
		}
		userReq.type = reqNULL;
		userReq.state = readyForRequest;
		writeNumProc.callBack();
	}


#elif (LCD_DATA_BITS_MODE == LCD_EIGHT_BITS_MODE)


	/* We will need it because we will send the number digit by digit so We have to invert it first
	 * to be printed then in a correct manner. */
	static u64 LOC_uint64InvertedImage = 0;

	/* Its usage is to check if the unit digit in the input number is zero or not */
	static u8 LOC_uint8ZeroInUnitsChecker = 0;
	
	/* Its usage is to check if the input number is zero */
	static u8 LOC_uint8NumberIsZeroFlag = 0;

	/* Its usage is to check if the input number is not zero */
	static u8 LOC_uint8NumberIsNotZeroFlag = 0;

	/* Its usage is to check if the input number is zero whether We finished printing it or not */
	static u8 LOC_uint8ZeroIsPrintedFlag = 0;

	/* Check if the input is zero */
	if((userReq.number == 0) && (LOC_uint8NumberIsZeroFlag == 0)) {
		LOC_uint8NumberIsZeroFlag = 1;
	}

	if (LOC_uint8NumberIsZeroFlag == 1 && LOC_uint8ZeroIsPrintedFlag == 0) {
		/* Check if the input is zero, print it directly */

		/* Check if We finished all stages of the LCD_writeDataSM or not */
		if(writeDataSM_remainingStages > 0) {
			LCD_writeDataSM(0 + 48);
			writeDataSM_remainingStages--;
		}
		else{
			/* We finished the printing of one digit */
			writeDataSM_remainingStages = REMAINING_STAGES_8_BIT_MODE_CASE;
			LOC_uint8ZeroIsPrintedFlag = 1;
		}
	} else if (userReq.number != 0) {
		/* Save an inverted image of the input in a local variable to b able to print it properly */
		LOC_uint64InvertedImage *= 10;
		LOC_uint64InvertedImage += userReq.number % 10;

		/* If the unit digit in the input number is zero then raise the Zero checker flag */
		if (LOC_uint64InvertedImage == 0) {
			LOC_uint8ZeroInUnitsChecker = 1;
		}

		userReq.number /= 10;

	} else if (LOC_uint64InvertedImage != 0) {
		/* Send the inverted number to be printed digit-by-digit */

		/* Check if We finished all stages of the LCD_writeDataSM or not */
		if(writeDataSM_remainingStages > 0) {
			LCD_writeDataSM((LOC_uint64InvertedImage % 10) + 48);
			writeDataSM_remainingStages--;
		}
		else{
			/* We finished the printing of one digit */
			writeDataSM_remainingStages = REMAINING_STAGES_8_BIT_MODE_CASE;
			LOC_uint64InvertedImage /= 10;
		}
	}
	/* If the Zero checker flag is set then print Zero as the last thing to be printed */
	else if (LOC_uint8ZeroInUnitsChecker == 1)
	{
		/* Check if We finished all stages of the LCD_writeDataSM or not */
		if(writeDataSM_remainingStages > 0) {
			LCD_writeDataSM(0 + 48);
			writeDataSM_remainingStages--;
		}
		else{
			/* We finished the printing of one digit */
			writeDataSM_remainingStages = REMAINING_STAGES_8_BIT_MODE_CASE;
			LOC_uint8ZeroInUnitsChecker = 0;
		}
	} else {
		/* We finished the Printing of the Whole number */
		if (LOC_uint8NumberIsZeroFlag == 1) {
			LOC_uint8NumberIsZeroFlag = 0;
		}
		if (LOC_uint8ZeroIsPrintedFlag == 1) {
			LOC_uint8ZeroIsPrintedFlag = 0;
		}
		userReq.type = reqNULL;
		userReq.state = readyForRequest;
		writeNumProc.callBack();
	}

#endif  /* #if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE) */

}


/**
 *@brief : Process that cleans the screen.
 *@param : void.
 *@return: void.
 */
static void LCD_cleanProc(void){

#if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE)
	static u8 writeCommandSM_remainingStages = REMAINING_STAGES_4_BIT_MODE_CASE;

#elif (LCD_DATA_BITS_MODE == LCD_EIGHT_BITS_MODE)
	static u8 writeCommandSM_remainingStages = REMAINING_STAGES_8_BIT_MODE_CASE;

#endif  /* #if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE) */


#if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE)

	if(writeCommandSM_remainingStages > 0){
		LCD_writeCommandSM(LCD_ClearDisplay);
		writeCommandSM_remainingStages--;
	}
	else{
		/* We finished the printing of one character */
		writeCommandSM_remainingStages = REMAINING_STAGES_4_BIT_MODE_CASE;
		userReq.type = NULL;
		userReq.state = readyForRequest;
		clearProc.callBack();
	}

#elif (LCD_DATA_BITS_MODE == LCD_EIGHT_BITS_MODE)

	if(writeCommandSM_remainingStages > 0){
		LCD_writeCommandSM(LCD_ClearDisplay);
		writeCommandSM_remainingStages--;
	}
	else{
		/* We finished the printing of one character */
		writeCommandSM_remainingStages = REMAINING_STAGES_4_BIT_MODE_CASE;
		userReq.type = NULL;
		userReq.state = readyForRequest;
		clearProc.callBack();
	}

#endif  /* #if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE) */

}


/**
 *@brief : Process that set the cursor position.
 *@param : void.
 *@return: void.
 */
static void LCD_setCursorProc(void){

#if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE)
	static u8 writeCommandSM_remainingStages = REMAINING_STAGES_4_BIT_MODE_CASE;

#elif (LCD_DATA_BITS_MODE == LCD_EIGHT_BITS_MODE)
	static u8 writeCommandSM_remainingStages = REMAINING_STAGES_8_BIT_MODE_CASE;

#endif  /* #if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE) */


#if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE)

	if(writeCommandSM_remainingStages > 0){
		LCD_writeCommandSM(LCD_DDRAM_START_ADDRESS + userReq.cursorLocation);
		writeCommandSM_remainingStages--;
	}
	else{
		/* We finished the printing of one character */
		writeCommandSM_remainingStages = REMAINING_STAGES_4_BIT_MODE_CASE;
		userReq.type = NULL;
		userReq.state = readyForRequest;
		setCursorProc.callBack();
	}

#elif (LCD_DATA_BITS_MODE == LCD_EIGHT_BITS_MODE)

	if(writeCommandSM_remainingStages > 0){
		LCD_writeCommandSM(LCD_DDRAM_START_ADDRESS + userReq.cursorLocation);
		writeCommandSM_remainingStages--;
	}
	else{
		/* We finished the printing of one character */
		writeCommandSM_remainingStages = REMAINING_STAGES_4_BIT_MODE_CASE;
		userReq.type = NULL;
		userReq.state = readyForRequest;
		clearProc.callBack();
	}

#endif  /* #if (LCD_DATA_BITS_MODE == LCD_FOUR_BITS_MODE) */

}



/************************************************************************************/
/*								Functions' Implementation							*/
/************************************************************************************/


/**
 *@brief : Function that initializes the LCD.
 *@param : a callback function you want to be called after finishing your request.
 *@return: Error State.
 */
LCD_enuError_t LCD_enuInitAsync(void (*callBackFn)(void)){
	/* A local variable to assign the error state inside it and use only one return in the whole function
	 * through returning the value of this local variable.
	 * Initially we assume that everything is OK, if not its value will be changed according to a definite
	 * error type */
	LCD_enuError_t LOC_enuErrorStatus = LCD_enuOk;

	/* Check on the passed pointer if it's a NULL pointer or not */
	if((callBackFn == NULL_PTR) || (arrayofLCDPinConfig == NULL_PTR)){
		/* If the passed pointer is a NULL pointer We cannot access it, return an error */
		LOC_enuErrorStatus = LCD_enuNullPointer;
	}
	else if((lcdState == stateOff) && (userReq.state == readyForRequest)){
		initProc.callBack = callBackFn;
		userReq.state = busyWithRequest;
		lcdState = stateInitialization;
	}
	else{
		/* Do Nothing */
	}

	return LOC_enuErrorStatus;
}


/**
 *@brief : Function that returns to you the current state of the LCD.
 *@param : pointer inside which We will return to you the state of the LCD.
 *@return: Error State.
 */
LCD_enuError_t LCD_enuGetState(u8* State){
	/* A local variable to assign the error state inside it and use only one return in the whole function
	 * through returning the value of this local variable.
	 * Initially we assume that everything is OK, if not its value will be changed according to a definite
	 * error type */
	LCD_enuError_t LOC_enuErrorStatus = LCD_enuOk;

	/* Check on the passed pointer that it isn't a NULL pointer */
	if(State == NULL_PTR){
		/* The passed pointer is a NULL pointer, return an error */
		LOC_enuErrorStatus = LCD_enuNullPointer;
	}
	else{
		/* Return the current LCD's state */
		*State = lcdState;
	}

	return LOC_enuErrorStatus;
}


/**
 *@brief : Function that clears the screen.
 *@param : a callback function you want to be called after finishing your request.
 *@return: Error State.
 */
LCD_enuError_t LCD_enuClearScreenAsync(void (*callBackFn)(void)){
	/* A local variable to assign the error state inside it and use only one return in the whole function
	 * through returning the value of this local variable.
	 * Initially we assume that everything is OK, if not its value will be changed according to a definite
	 * error type */
	LCD_enuError_t LOC_enuErrorStatus = LCD_enuOk;

	/* Check on the Passed Pointer whether it is a NULL pointer or not */
	if(callBackFn == NULL_PTR){
		/* if the passed pointer is a NULL, return error */
		LOC_enuErrorStatus = LCD_enuNullPointer;
	}
	else if ((lcdState == stateOperational) && (userReq.state == readyForRequest)){
		clearProc.callBack = callBackFn;
		userReq.type = reqClearScreen;
		userReq.state = busyWithRequest;
	}
	else{
		/* Do Nothing */
	}

	return LOC_enuErrorStatus;
}


/**
 *@brief : Function that sets the cursor's position.
 *@param : The row and the column you want the cursor to go at, and a callback function
 *			you want to be called after finishing your request.
 *@return: Error State.
 */
LCD_enuError_t LCD_enuSetCursorAsync(LCD_enuRowNumber_t row, u8 column, void (*callBackFn)(void)){
	/* A local variable to assign the error state inside it and use only one return in the whole function
	 * through returning the value of this local variable.
	 * Initially we assume that everything is OK, if not its value will be changed according to a definite
	 * error type */
	LCD_enuError_t LOC_enuErrorStatus = LCD_enuOk;

	/* The compiler will generate the following warning:
	 * "comparison is always false due to limited range of data type [-Wtype-limits]"
	 * ignore it */
	if ((row < LCD_enuFirstRow) || (row > LCD_enuSecondRow)){
		LOC_enuErrorStatus = LCD_enuWrongRowNumber;
	}
	/* The compiler will generate the following warning:
	 * "comparison is always false due to limited range of data type [-Wtype-limits]"
	 * ignore it */
	else if ((column < LCD_enuColumn_1) || (column > LCD_enuColumn_40)){
		LOC_enuErrorStatus = LCD_enuWrongColumnNumber;
	}
	else if ((lcdState == stateOperational) && (userReq.state == readyForRequest)){

		/* Seems like the inputs are correct, then let us go to the required location */

		if (row == LCD_enuFirstRow){
			userReq.cursorLocation = column;
		}
		else{
			/* Then Copy_uint8X equals LCD_enuSecondRow */

			userReq.cursorLocation = LCD_CGRAM_START_ADDRESS + column;
		}

		setCursorProc.callBack = callBackFn;
		userReq.type = reqSetCursor;
		userReq.state = busyWithRequest;
	}
	else{
		/* Do Nothing */
	}

	return LOC_enuErrorStatus;
}


/**
 *@brief : Function that prints a string on the LCD.
 *@param : String you want to print, a callback function you want to be called after finishing
 *			your request.
 *@return: Error State.
 */
LCD_enuError_t LCD_enuWriteStringAsync(u8* string, void (*callBackFn)(void)){
	/* A local variable to assign the error state inside it and use only one return in the whole function
	 * through returning the value of this local variable.
	 * Initially we assume that everything is OK, if not its value will be changed according to a definite
	 * error type */
	LCD_enuError_t LOC_enuErrorStatus = LCD_enuOk;

	/* Check on the passed pointers whether NULL pointers or can be accessed */
	if((string == NULL_PTR) || (callBackFn == NULL_PTR)){
		/* The passed pointer is a NULL pointer, return an error */
		LOC_enuErrorStatus = LCD_enuNullPointer;
	}
	else if ((lcdState == stateOperational) && (userReq.state == readyForRequest)){
		writeProc.callBack = callBackFn;
		userReq.string = string;
		userReq.type = reqWriteString;
		userReq.state = busyWithRequest;
	}
	else{
		/* Do Nothing */
	}

	return LOC_enuErrorStatus;
}


/**
 *@brief : Function that prints a certain number on the LCD.
 *@param : Number you want to print.
 *@return: Error State.
 */
LCD_enuError_t LCD_enuWriteNumberAsync(u64 Copy_uint64Number, void (*callBackFn)(void)) {
	/* A local variable to assign the error state inside it and use only one return in the whole function
	 * through returning the value of this local variable.
	 * Initially we assume that everything is OK, if not its value will be changed according to a definite
	 * error type */
	LCD_enuError_t LOC_enuErrorStatus = LCD_enuOk;

	if ((lcdState == stateOperational) && (userReq.state == readyForRequest)) {
		writeNumProc.callBack = callBackFn;
		userReq.number = Copy_uint64Number;
		userReq.type = reqWriteNumber;
		userReq.state = busyWithRequest;
	} else {
		/* Do Nothing */
	}

	return LOC_enuErrorStatus;
}


/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/**
 *@brief : Runnable with periodicity 1 millisecond.
 *@param : void.
 *@return: void.
 */
void RUNNABLE_LCD(void){
	/* Jump to the current LCD's state */
	switch(lcdState){
	case stateInitialization:
		LCD_initProcSM();
		break;
	case stateOperational:
		if(userReq.state == busyWithRequest){
			switch(userReq.type){
			case reqClearScreen:
				LCD_cleanProc();
				break;
			case reqSetCursor:
				LCD_setCursorProc();
				break;
			case reqWriteString:
				LCD_writeProc();
				break;
			case reqWriteNumber:
                LCD_writeNumProc();
                break;
			default:
				/* Do Nothing */
				break;
			}
		}
		else{
			/* Do Nothing */
		}
		break;
	case stateOff:

		break;
	default:
		/* Do Nothing */
		break;
	}
}



#endif /* #if ((LCD_DATA_BITS_MODE != LCD_FOUR_BITS_MODE) && (LCD_DATA_BITS_MODE != LCD_EIGHT_BITS_MODE)) */
