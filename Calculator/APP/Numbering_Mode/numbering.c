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
#define LCD_MAX_COL			17 // Max number of columns of my 16x2 LCD to decrement from when writing from right side

void (*pf_Numbering_State_Handler)(void) = STATE_CALL(Decimal_Mode);
static numbering_states_t numbering_state_id = numbering_states_max;
static uint8 Decimal_Number[DECIMAL_MAX_SIZE];  // Array to hold any decimal number
static uint8 Decimal_Length;					// Contains length of number in "Decimal_Number" array
static uint8 Octal_Number[OCTAL_MAX_SIZE];  	// Array to hold any octal number
static uint8 Octal_Length;						// Contains length of number in "Octal_Number" array
static uint8 Hexa_Number[HEXA_MAX_SIZE];  		// Array to hold any hexadecimal number
static uint8 Hexadecimal_Length;				// Contains length of number in "Hexa_Number" array
static uint8 Binary_Number[BINARY_MAX_SIZE];  	// Array to hold any binary number
static uint8 Binary_Length;						// Contains length of number in "Binary_Number" array
static uint8 pressed_key;
static uint8 double_check_before_quitting;

/**=============================================
  * @Fn				- PowerOf
  * @brief 			- This function calculates the result of "base" power of "power"
  * @param [in] 	- base: Base number
  * @param [in] 	- power: Exponend number
  * @retval 		- Result of "base" power of "power"
  * Note			- None
  */
static uint32 PowerOf(uint8 base, uint8 power){
	uint32 result = 1;
	uint8 index;
	for(index = 0; index < power; index++){
		result *= base;
	}
	return result;
}

/**=============================================
  * @Fn				- Print_Array_LCD
  * @brief 			- This function will print an array on the LCD
  * @param [in] 	- Array: Pointer to the array containing digits
  * @param [in] 	- Length: Length of valid digits in the array
  * @retval 		- None
  * Note			- Supports arrays containing hexadecimal digits (>=10)
  */
static void Print_Array_LCD(uint8 *Array, uint8 Length){
	uint8 index;
	for(index = 0; index < Length; index++){
		if((0 <= Array[index]) && (10 > Array[index])){
			LCD_Send_Char(Array[index]+48);
		}
		else{
			switch(Array[index]){
			case 10:
				LCD_Send_Char('A');
				break;
			case 11:
				LCD_Send_Char('B');
				break;
			case 12:
				LCD_Send_Char('C');
				break;
			case 13:
				LCD_Send_Char('D');
				break;
			case 14:
				LCD_Send_Char('E');
				break;
			case 15:
				LCD_Send_Char('F');
				break;
			}
		}
	}
}

/**=============================================
  * @Fn				- Flush_Array
  * @brief 			- This function will save array elements into a variable
  * @param [in] 	- arr: Pointer to the array containing digits
  * @param [in] 	- length: Length of valid digits in the array
  * @retval 		- Extracted number out of the array
  * Note			- Can't be used if array elements can contain 2 digit number (>=10) EX: Hexadecimal numbers
  */
static uint32 Flush_Array(uint8 *arr, uint8 length){
	uint32 result = 0;
	uint8 index;
	for(index = 0; index < length; index++){
		result += arr[index];
		result *= 10;
	}
	result /= 10;
	return result;
}

/**=============================================
  * @Fn				- Integer_To_Array_Reversed
  * @brief 			- This function will save an array to an array in reversed order
  * @param [in] 	- source_int: Integer variable that contains the decimal value
  * @param [out] 	- Dest_Array: Pointer to the destination array to save digits into
  * @retval 		- Length of valid elements in array
  * Note			- You must use function Reverse_Array after this if you want the values to be normal not reversed
  */
static uint8 Integer_To_Array_Reversed(uint32 source_int, uint8 *Dest_Array){
	uint8 index = 0;
	while(0 != source_int){
		Dest_Array[index] = source_int % 10;
		source_int /= 10;
		index++;
	}
	return index;
}

/**=============================================
  * @Fn				- Reverse_Array
  * @brief 			- This function will reverse elements in an array of given size
  * @param 	 		- arr: Pointer to the array to be reversed
  * @param [in] 	- length: Length of valid array elements
  * @param [out] 	- None
  * @retval 		- None
  * Note			- None
  */
static void Reverse_Array(uint8 *arr, uint8 length){
	uint8 temp, index;
	uint8 *pStart = arr;
	uint8 *pEnd = (arr + length);
	for(index = 0; index <= length/2; index++){
		temp = *pStart;
		*pStart = *pEnd;
		*pEnd = temp;
		pStart++;
		pEnd--;
	}
}

/**=============================================
  * @Fn				- DecToBin
  * @brief 			- This function shall convert decimal number from "Decimal_Number" array to octal number and save it in "Octal_Number" array
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- None
  */
static void DecToOct(){
	uint32 decimal_value = 0;

	/* If Decimal_Number array contains no valid numbers */
	if(0 == Decimal_Length){
		memset(Octal_Number, 0, OCTAL_MAX_SIZE);
		Octal_Length = 0;
	}
	else{
		/* Save elements of "Decimal_Number" array to "decimal_value" variable */
		decimal_value = Flush_Array(Decimal_Number, Decimal_Length);

		/* Clear "Decimal_Number" array and its counter */
		memset(Decimal_Number, 0, DECIMAL_MAX_SIZE);
		Decimal_Length = 0;

		/* Clear "Octal_Number" array and its counter */
		memset(Octal_Number, 0, OCTAL_MAX_SIZE);
		Octal_Length = 0;

		/* Convert "decimal_value" variable to octal and save result in "Octal_Number" array */
		while(0 != decimal_value){
			Octal_Number[Octal_Length] += decimal_value % 8;
			decimal_value /= 8;
			Octal_Length++;
		}

		/* Check if there is a number in "Octal_Number" array */
		if(0 != Octal_Length){
			/* Reverse "Octal_Number" array */
			Reverse_Array(Octal_Number, (Octal_Length-1));
		}
		else{ /* Do Nothing */ }
	}
}

/**=============================================
  * @Fn				- OctToDec
  * @brief 			- This function shall convert octal number from "Octal_Number" array to decimal number and save it in "Decimal_Number" array
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- None
  */
static void OctToDec(){
	uint32 octal_value = 0;
	uint32 decimal_value = 0;
	uint8 index = 0;
	/* If Octal_Number array contains no valid numbers */
	if(0 == Octal_Length){
		memset(Decimal_Number, 0, DECIMAL_MAX_SIZE);
		Decimal_Length = 0;
	}
	else{
		/* Save elements of "Octal_Number" array to "octal_value" variable */
		octal_value = Flush_Array(Octal_Number, Octal_Length);

		/* Clear "Octal_Number" array and its counter */
		memset(Octal_Number, 0, OCTAL_MAX_SIZE);
		Octal_Length = 0;

		/* Clear "Decimal_Number" array and its counter */
		memset(Decimal_Number, 0, DECIMAL_MAX_SIZE);
		Decimal_Length = 0;

		/* Convert "octal_value" variable to decimal and save result in "Decimal_Number" array */
		while(0 != octal_value){
			decimal_value += ((octal_value % 10) * PowerOf(8, index));
			index++;
			octal_value /= 10;
		}

		/* Save "decimal_value" in "Decimal_Number" Array */
		Decimal_Length = Integer_To_Array_Reversed(decimal_value, Decimal_Number);

		/* Check if there is a number in "Decimal_Number" array */
		if(0 != Decimal_Length){
			/* Reverse "Decimal_Number" array */
			Reverse_Array(Decimal_Number, (Decimal_Length-1));
		}
		else{ /* Do Nothing */ }
	}
}

/**=============================================
  * @Fn				- DecToBin
  * @brief 			- This function shall convert decimal number from "Decimal_Number" array to binary number and save it in "Binary_Number" array
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- None
  */
static void DecToBin(){
	uint32 decimal_value = 0;

	/* If Decimal_Number array contains no valid numbers */
	if(0 == Decimal_Length){
		memset(Binary_Number, 0, BINARY_MAX_SIZE);
		Binary_Length = 0;
	}
	else{
		/* Save elements of "Decimal_Number" array to "decimal_value" variable */
		decimal_value = Flush_Array(Decimal_Number, Decimal_Length);

		/* Clear "Decimal_Number" array and its counter */
		memset(Decimal_Number, 0, DECIMAL_MAX_SIZE);
		Decimal_Length = 0;

		/* Clear "Binary_Number" array and its counter */
		memset(Binary_Number, 0, BINARY_MAX_SIZE);
		Binary_Length = 0;

		/* Convert "decimal_value" variable to binary and save result in "Binary_Number" array */
		while(0 != decimal_value){
			Binary_Number[Binary_Length] += decimal_value % 2;
			decimal_value /= 2;
			Binary_Length++;
		}

		/* Check if there is a number in "Binary_Number" array */
		if(0 != Binary_Length){
			/* Reverse "Binary_Number" array */
			Reverse_Array(Binary_Number, (Binary_Length-1));
		}
		else{ /* Do Nothing */ }
	}
}

/**=============================================
  * @Fn				- BinToDec
  * @brief 			- This function shall convert binary number from "Binary_Number" array to decimal number and save it in "Decimal_Number" array
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- None
  */
static void BinToDec(){
	uint32 decimal_value = 0;
	uint8 index = 0;
	uint8 *pArrIndex = NULL;
	/* If Binary_Number array contains no valid numbers */
	if(0 == Binary_Length){
		memset(Decimal_Number, 0, DECIMAL_MAX_SIZE);
		Decimal_Length = 0;
	}
	else{
		/* Clear "Decimal_Number" array and its counter */
		memset(Decimal_Number, 0, DECIMAL_MAX_SIZE);
		Decimal_Length = 0;

		/* Pointer to the last digit in "Hexa_Number" array */
		pArrIndex = (Binary_Number + (Binary_Length - 1));

		/* Convert "Binary_Number" array to decimal and save result in "decimal_value" variable */
		while(pArrIndex >= Binary_Number){
			decimal_value += (*pArrIndex * PowerOf(2, index));
			index++;
			pArrIndex--;
		}

		/* Clear "Hexa_Number" array and its counter */
		memset(Binary_Number, 0, BINARY_MAX_SIZE);
		Binary_Length = 0;

		/* Save "decimal_value" in "Decimal_Number" Array */
		Decimal_Length = Integer_To_Array_Reversed(decimal_value, Decimal_Number);

		/* Check if there is a number in "Decimal_Number" array */
		if(0 != Decimal_Length){
			/* Reverse "Decimal_Number" array */
			Reverse_Array(Decimal_Number, (Decimal_Length-1));
		}
		else{ /* Do Nothing */ }
	}
}

/**=============================================
  * @Fn				- DecToHex
  * @brief 			- This function shall convert decimal number from "Decimal_Number" array to hexadecimal number and save it in "Hexa_Number" array
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- None
  */
static void DecToHex(){
	uint32 decimal_value = 0;

	/* If Decimal_Number array contains no valid numbers */
	if(0 == Decimal_Length){
		memset(Hexa_Number, 0, HEXA_MAX_SIZE);
		Hexadecimal_Length = 0;
	}
	else{
		/* Save elements of "Decimal_Number" array to "decimal_value" variable */
		decimal_value = Flush_Array(Decimal_Number, Decimal_Length);

		/* Clear "Decimal_Number" array and its counter */
		memset(Decimal_Number, 0, DECIMAL_MAX_SIZE);
		Decimal_Length = 0;

		/* Clear "Hexa_Number" array and its counter */
		memset(Hexa_Number, 0, HEXA_MAX_SIZE);
		Hexadecimal_Length = 0;

		/* Convert "decimal_value" variable to hexadecimal and save result in "Hexa_Number" array */
		while(0 != decimal_value){
			Hexa_Number[Hexadecimal_Length] += decimal_value % 16;
			decimal_value /= 16;
			Hexadecimal_Length++;
		}

		/* Check if there is a number in "Hexa_Number" array */
		if(0 != Hexadecimal_Length){
			/* Reverse "Hexa_Number" array */
			Reverse_Array(Hexa_Number, (Hexadecimal_Length-1));
		}
		else{ /* Do Nothing */ }
	}
}

/**=============================================
  * @Fn				- HexToDec
  * @brief 			- This function shall convert hexadecimal number from "Hexa_Number" array to decimal number and save it in "Decimal_Number" array
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- None
  */
static void HexToDec(){
	uint32 decimal_value = 0;
	uint8 index = 0;
	uint8 *pArrIndex = NULL;
	/* If Hexa_Number array contains no valid numbers */
	if(0 == Hexadecimal_Length){
		memset(Decimal_Number, 0, DECIMAL_MAX_SIZE);
		Decimal_Length = 0;
	}
	else{
		/* Clear "Decimal_Number" array and its counter */
		memset(Decimal_Number, 0, DECIMAL_MAX_SIZE);
		Decimal_Length = 0;

		/* Pointer to the last digit in "Hexa_Number" array */
		pArrIndex = (Hexa_Number + (Hexadecimal_Length - 1));

		/* Convert "Hexa_Number" array to decimal and save result in "decimal_value" variable */
		while(pArrIndex >= Hexa_Number){
			decimal_value += (*pArrIndex * PowerOf(16, index));
			index++;
			pArrIndex--;
		}

		/* Clear "Hexa_Number" array and its counter */
		memset(Hexa_Number, 0, HEXA_MAX_SIZE);
		Hexadecimal_Length = 0;

		/* Save "decimal_value" in "Decimal_Number" Array */
		Decimal_Length = Integer_To_Array_Reversed(decimal_value, Decimal_Number);

		/* Check if there is a number in "Decimal_Number" array */
		if(0 != Decimal_Length){
			/* Reverse "Decimal_Number" array */
			Reverse_Array(Decimal_Number, (Decimal_Length-1));
		}
		else{ /* Do Nothing */ }
	}
}


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
		LCD_Send_string_Pos((uint8*)"DECIMAL", LCD_SECOND_ROW, (LCD_MAX_COL-7));
		LCD_Set_Cursor(LCD_FIRST_ROW, Decimal_Length + 1);
	}

	/* State Action */
	pressed_key = keypad_Get_Pressed_Key();
	if((0 <= pressed_key) && (10 > pressed_key)){
		double_check_before_quitting = 0; // Clear flag
		/* Validate that user is inputting a 5 digit decimal */
		if(DECIMAL_MAX_SIZE > Decimal_Length){
			LCD_Send_Char(pressed_key+48);
			Decimal_Number[Decimal_Length] = pressed_key;
			Decimal_Length++;
		}
		else{ /* Do Nothing */ }
	}
	else if('x' == pressed_key){
		double_check_before_quitting = 0; // Clear flag
		/* Go to octal mode */
		DecToOct();
		LCD_Send_Command(LCD_CLEAR_DISPLAY);
		Print_Array_LCD(Octal_Number, Octal_Length);
		pf_Numbering_State_Handler = STATE_CALL(Octal_Mode);
	}
	else if('-' == pressed_key){
		double_check_before_quitting = 0; // Clear flag
		/* Go to binary mode */
		DecToBin();
		LCD_Send_Command(LCD_CLEAR_DISPLAY);
		Print_Array_LCD(Binary_Number, Binary_Length);
		pf_Numbering_State_Handler = STATE_CALL(Binary_Mode);
	}
	else if('+' == pressed_key){
		double_check_before_quitting = 0; // Clear flag
		/* Go to hexadecimal mode */
		DecToHex();
		LCD_Send_Command(LCD_CLEAR_DISPLAY);
		LCD_Send_String((uint8*)"0x");
		Print_Array_LCD(Hexa_Number, Hexadecimal_Length);
		pf_Numbering_State_Handler = STATE_CALL(Hexadecimal_Mode);
	}
	else if('C' == pressed_key){
		/* Clear screen, or exit if pressed twice in a row */
		LCD_Send_Command(LCD_CLEAR_DISPLAY);
		memset(Decimal_Number, 0, DECIMAL_MAX_SIZE);
		Decimal_Length = 0;
		if(1 == double_check_before_quitting){
			USER_RESET_FLAG = 1;
		}
		else{
			double_check_before_quitting = 1;
		}
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
	if(Octal_Mode != numbering_state_id){
		numbering_state_id = Octal_Mode;
		LCD_Send_string_Pos((uint8*)"OCTAL", LCD_SECOND_ROW, (LCD_MAX_COL-5));
		LCD_Set_Cursor(LCD_FIRST_ROW, Octal_Length + 1);
	}

	/* State Action */
	pressed_key = keypad_Get_Pressed_Key();
	if((0 <= pressed_key) && (8 > pressed_key)){
		/* Validate that user is inputting a 6 digit octal */
		if(OCTAL_MAX_SIZE > Octal_Length){
			LCD_Send_Char(pressed_key+48);
			Octal_Number[Octal_Length] = pressed_key;
			Octal_Length++;
		}
		else{ /* Do Nothing */ }
	}
	else if('/' == pressed_key){
		/* Go to decimal mode */
		OctToDec();
		LCD_Send_Command(LCD_CLEAR_DISPLAY);
		Print_Array_LCD(Decimal_Number, Decimal_Length);
		pf_Numbering_State_Handler = STATE_CALL(Decimal_Mode);
	}
	else if('-' == pressed_key){
		/* Go to binary mode */
		OctToDec();
		DecToBin();
		LCD_Send_Command(LCD_CLEAR_DISPLAY);
		Print_Array_LCD(Binary_Number, Binary_Length);
		pf_Numbering_State_Handler = STATE_CALL(Binary_Mode);
	}
	else if('+' == pressed_key){
		/* Go to hexadecimal mode */
		OctToDec();
		DecToHex();
		LCD_Send_Command(LCD_CLEAR_DISPLAY);
		LCD_Send_String((uint8*)"0x");
		Print_Array_LCD(Hexa_Number, Hexadecimal_Length);
		pf_Numbering_State_Handler = STATE_CALL(Hexadecimal_Mode);
	}
	else if('C' == pressed_key){
		/* Clear screen, or exit if pressed twice in a row */
		double_check_before_quitting = 1; // flag for decimal state that user pressed C
		memset(Octal_Number, 0, OCTAL_MAX_SIZE);
		Octal_Length = 0;
		LCD_Send_Command(LCD_CLEAR_DISPLAY);
		pf_Numbering_State_Handler = STATE_CALL(Decimal_Mode);
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
	if(Binary_Mode != numbering_state_id){
		numbering_state_id = Binary_Mode;
		LCD_Send_string_Pos((uint8*)"BINARY", LCD_SECOND_ROW, (LCD_MAX_COL-6));
		LCD_Set_Cursor(LCD_FIRST_ROW, Binary_Length + 1);
	}

	/* State Action */
	pressed_key = keypad_Get_Pressed_Key();
	if((0 <= pressed_key) && (2 > pressed_key)){
		/* Validate that user is inputting a 16 digit binary */
		if(BINARY_MAX_SIZE > Binary_Length){
			LCD_Send_Char(pressed_key+48);
			Binary_Number[Binary_Length] = pressed_key;
			Binary_Length++;
		}
		else{ /* Do Nothing */ }
	}
	else if('x' == pressed_key){
		/* Go to octal mode */
		BinToDec();
		DecToOct();
		LCD_Send_Command(LCD_CLEAR_DISPLAY);
		Print_Array_LCD(Octal_Number, Octal_Length);
		pf_Numbering_State_Handler = STATE_CALL(Octal_Mode);
	}
	else if('/' == pressed_key){
		/* Go to decimal mode */
		BinToDec();
		LCD_Send_Command(LCD_CLEAR_DISPLAY);
		Print_Array_LCD(Decimal_Number, Decimal_Length);
		pf_Numbering_State_Handler = STATE_CALL(Decimal_Mode);
	}
	else if('+' == pressed_key){
		/* Go to hexadecimal mode */
		BinToDec();
		DecToHex();
		LCD_Send_Command(LCD_CLEAR_DISPLAY);
		LCD_Send_String((uint8*)"0x");
		Print_Array_LCD(Hexa_Number, Hexadecimal_Length);
		pf_Numbering_State_Handler = STATE_CALL(Hexadecimal_Mode);
	}
	else if('C' == pressed_key){
		/* Clear screen, or exit if pressed twice in a row */
		double_check_before_quitting = 1; // flag for decimal state that user pressed C
		memset(Binary_Number, 0, BINARY_MAX_SIZE);
		Binary_Length = 0;
		LCD_Send_Command(LCD_CLEAR_DISPLAY);
		pf_Numbering_State_Handler = STATE_CALL(Decimal_Mode);
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
	if(Hexadecimal_Mode != numbering_state_id){
		numbering_state_id = Hexadecimal_Mode;
		LCD_Send_string_Pos((uint8*)"HEXA", LCD_SECOND_ROW, (LCD_MAX_COL-4));
		LCD_Set_Cursor(LCD_FIRST_ROW, Hexadecimal_Length + 3);
	}

	/* State Action */
	pressed_key = keypad_Get_Pressed_Key();
	if((0 <= pressed_key) && (10 > pressed_key)){
		/* Validate that user is inputting a 4 digit hexadecimal */
		if(HEXA_MAX_SIZE > Hexadecimal_Length){
			LCD_Send_Char(pressed_key+48);
			Hexa_Number[Hexadecimal_Length] = pressed_key;
			Hexadecimal_Length++;
		}
		else{ /* Do Nothing */ }
	}
	else if('x' == pressed_key){
		/* Go to octal mode */
		HexToDec();
		DecToOct();
		LCD_Send_Command(LCD_CLEAR_DISPLAY);
		Print_Array_LCD(Octal_Number, Octal_Length);
		pf_Numbering_State_Handler = STATE_CALL(Octal_Mode);
	}
	else if('-' == pressed_key){
		/* Go to binary mode */
		HexToDec();
		DecToBin();
		LCD_Send_Command(LCD_CLEAR_DISPLAY);
		Print_Array_LCD(Binary_Number, Binary_Length);
		pf_Numbering_State_Handler = STATE_CALL(Binary_Mode);
	}
	else if('/' == pressed_key){
		/* Go to decimal mode */
		HexToDec();
		LCD_Send_Command(LCD_CLEAR_DISPLAY);
		Print_Array_LCD(Decimal_Number, Decimal_Length);
		pf_Numbering_State_Handler = STATE_CALL(Decimal_Mode);
	}
	else if('C' == pressed_key){
		/* Clear screen, or exit if pressed twice in a row */
		double_check_before_quitting = 1; // flag for decimal state that user pressed C
		memset(Hexa_Number, 0, HEXA_MAX_SIZE);
		Hexadecimal_Length = 0;
		LCD_Send_Command(LCD_CLEAR_DISPLAY);
		pf_Numbering_State_Handler = STATE_CALL(Decimal_Mode);
	}
	else{ /* Do Nothing */ }
}
