/*************************************************************************/
/* Author        : Omar Yamany                                    		 */
/* Project       : Calculator  	                             			 */
/* File          : numbering.c 			                           		 */
/* Date          : Jun 15, 2023                                          */
/* Version       : V1                                                    */
/* GitHub        : https://github.com/Piistachyoo             		     */
/*************************************************************************/

#include "numbering.h"

// since our operands are 32 bits, max value is 10 digit long -> 4294967295
#define ARRAY_MAX_SIZE		16 // Max is 16 binary bits
#define DECIMAL_MAX_SIZE	5  // Max is 65535 which is 5 decimal digits
#define OCTAL_MAX_SIZE		6  // Max is 177777 which is 6 octal digits
#define BINARY_MAX_SIZE		16 // Max is 1111 1111 1111 1111 which is 16 binary bits
#define HEXA_MAX_SIZE		4  // Max is 0xFFFF which is 4 hexadecimal bits
#define LCD_MAX_COL			17

void (*pf_Numbering_State_Handler)(void) = STATE_CALL(Decimal_Mode);
static numbering_states_t numbering_state_id = numbering_states_max;
static uint8 user_input[ARRAY_MAX_SIZE]; 		// Array to use as a buffer for user input
static uint8 user_input_index;					// Variable to hold the length of the array
static uint8 pressed_key;

/**=============================================
  * @Fn				- ST_Decimal_Mode
  * @brief 			- In this state, the number on the screen is displayed in decimal format
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- Initial state
  */
STATE_DEF(Decimal_Mode){
	/* State Name */
	if(Decimal_Mode != numbering_state_id){
		numbering_state_id = Decimal_Mode;
		LCD_Send_string_Pos((uint8*)"BINARY", LCD_SECOND_ROW, (LCD_MAX_COL-6));
		LCD_Set_Cursor(LCD_FIRST_ROW, 1);
	}

	/* State Action */
	pressed_key = keypad_Get_Pressed_Key();
	if((0 <= pressed_key) && (10 > pressed_key)){
		/* Validate that user is inputting a 5 digit decimal */
		if(DECIMAL_MAX_SIZE >= user_input_index){
			LCD_Send_Char(pressed_key);
			user_input[user_input_index] = pressed_key;
			user_input_index++;
		}
		else{ /* Do Nothing */ }
	}
	else if('x' == pressed_key){
		/* Go to octal mode */

	}
	else if('-' == pressed_key){
		/* Go to binary mode */
	}
	else if('+' == pressed_key){
		/* Go to hexadecimal mode */

	}
	else if('C' == pressed_key){
		/* Clear screen, or exit if pressed twice in a row */
	}
	else{ /* Do Nothing */ }
}

/**=============================================
  * @Fn				- ST_Octal_Mode
  * @brief 			- In this state, the number on the screen is displayed in octal format
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- None
  */
STATE_DEF(Octal_Mode){
	/* State Name */
	numbering_state_id = Decimal_Mode;

	/* State Action */
	pressed_key = keypad_Get_Pressed_Key();
	if((0 <= pressed_key) && (10 > pressed_key)){

	}
	else if('x' == pressed_key){
		/* Go to octal mode */

	}
	else if('-' == pressed_key){
		/* Go to binary mode */
	}
	else if('+' == pressed_key){
		/* Go to hexadecimal mode */

	}
	else if('C' == pressed_key){
		/* Clear screen, or exit if pressed twice in a row */
	}
	else{ /* Do Nothing */ }
}

/**=============================================
  * @Fn				- ST_Binary_Mode
  * @brief 			- In this state, the number on the screen is displayed in binary format
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- None
  */
STATE_DEF(Binary_Mode){
	/* State Name */
	numbering_state_id = Decimal_Mode;

	/* State Action */
	pressed_key = keypad_Get_Pressed_Key();
	if((0 <= pressed_key) && (10 > pressed_key)){

	}
	else if('x' == pressed_key){
		/* Go to octal mode */

	}
	else if('-' == pressed_key){
		/* Go to binary mode */
	}
	else if('+' == pressed_key){
		/* Go to hexadecimal mode */

	}
	else if('C' == pressed_key){
		/* Clear screen, or exit if pressed twice in a row */
	}
	else{ /* Do Nothing */ }
}

/**=============================================
  * @Fn				- ST_Hexadecimal_Mode
  * @brief 			- In this state, the number on the screen is displayed in hexadecimal format
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- None
  */
STATE_DEF(Hexadecimal_Mode){
	/* State Name */
	numbering_state_id = Hexadecimal_Mode;

	/* State Action */
	pressed_key = keypad_Get_Pressed_Key();
	if((0 <= pressed_key) && (10 > pressed_key)){

	}
	else if('x' == pressed_key){
		/* Go to octal mode */

	}
	else if('-' == pressed_key){
		/* Go to binary mode */
	}
	else if('+' == pressed_key){
		/* Go to hexadecimal mode */

	}
	else if('C' == pressed_key){
		/* Clear screen, or exit if pressed twice in a row */
	}
	else{ /* Do Nothing */ }
}
