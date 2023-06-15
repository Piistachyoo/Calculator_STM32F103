/*************************************************************************/
/* Author        : Omar Yamany                                    		 */
/* Project       : Calculator  	                             			 */
/* File          : calculator.c 			                             */
/* Date          : Jun 15, 2023                                          */
/* Version       : V1                                                    */
/* GitHub        : https://github.com/Piistachyoo             		     */
/*************************************************************************/

#include "calculator.h"

// since our operands are 32 bits, max value is 10 digit long -> 4294967295
#define ARRAY_MAX_SIZE	10
#define	VAR_MAX_VALUE	4294967295UL

static uint32 first_op, second_op, result; 		// Variables to hold the value of our operands and result
static uint8 operation;							// Variable to hold the operation sign (+,-,*,\)
static uint8 user_input[ARRAY_MAX_SIZE]; 		// Array to use as a buffer for user input
static uint8 user_input_index;					// Variable to hold the length of the array
static uint8 pressed_key;
static calculator_states_t calculator_states_id;
void (*pfCalculator_State_Handler)() = STATE_CALL(First_Operand);
extern uint8 USER_RESET_FLAG; 					// To be set to 1 if user wants to exit this mode
static uint8 double_check_before_quitting;
static uint8 result_string[ARRAY_MAX_SIZE];

/**=============================================
  * @Fn				- Flush_Array
  * @brief 			- This function copies elements of arr to destination and clears array elements
  * @param 		 	- arr: Pointer to the source array
  * @param 		 	- length: Length of data in the array
  * @param 		 	- destination: Destination variable to save array elements into
  * @retval 		- None
  * Note			- None
  */
static void Flush_Array(uint8 *arr, uint8 *length, uint32 *destination){
	*destination = 0;
	uint8 index = 0;
	uint8 max_length_array_flag = (*length==ARRAY_MAX_SIZE) ? 1 : 0;
	for(; index < *length; index++){
		*destination += arr[index];
		/* Don't multiply by 10 if variable reached 9 digits or it will overflow */
		if(ARRAY_MAX_SIZE > index){
			*destination *= 10;
		}
	}
	if(!max_length_array_flag){
		*destination /= 10;
	}
	*length = 0;
//	memset(arr, 0, ARRAY_MAX_SIZE);
}

/**=============================================
  * @Fn				- Calculate_Result
  * @brief 			- This function shall do the calculation and return the result
  * @param [in] 	- op1: First operand
  * @param [in] 	- op2: Second operand
  * @param [in] 	- operator: Operation sign (+,-,x,/)
  * @param [out] 	- None
  * @retval 		- Result of the calculation
  * Note			- In minus operation, it will always return a positive integer which will be the absolute difference
  * 				- If no operation is specified, it will return the first operand op1
  */
static uint32 Calculate_Result(uint32 op1, uint32 op2, uint8 operator){
	uint32 final_result;
	switch(operator){
	case '+':
		final_result = op1 + op2;
		break;
	case '-':
		final_result = (op1>op2) ? (op1-op2) : (op2-op1);
		break;
	case 'x':
		final_result = op1 * op2;
		break;
	case '/':
		final_result = (0==op2) ? 0 : (op1 / op2); // Check if op2 is 0 to avoid exception
		break;
	default:
		final_result = op1;
		break;
	}
	return final_result;
}

/**=============================================
  * @Fn				- ST_First_Operand
  * @brief 			- In this state, the system will store user entry in first operand array
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- This function will be called in First_Operand state
  */
STATE_DEF(First_Operand){
	/* State Name */
	calculator_states_id = First_Operand;

	/* State Action */
	pressed_key = keypad_Get_Pressed_Key();

	/* User entered a number */
	if((0 <= pressed_key) && (10 > pressed_key)){
		double_check_before_quitting = 0; // Clear flag
		/* Validate that we are not writing outside array boundaries */
		if(user_input_index < ARRAY_MAX_SIZE){
			LCD_Send_Char(pressed_key+48);
			user_input[user_input_index] = pressed_key;
			user_input_index++;
		}
		else{ /* Do Nothing */ }
	}
	/* User entered a operation sign */
	else if(('+' == pressed_key) || ('-' == pressed_key) || ('x' == pressed_key) || ('/' == pressed_key)){
		double_check_before_quitting = 0; // Clear flag
		operation = pressed_key;
		LCD_Send_Char(pressed_key);
		/* Save the buffer array in first_op variable */
		Flush_Array(user_input, &user_input_index, &first_op);
		pfCalculator_State_Handler = STATE_CALL(Second_Operand);
	}
	else if('=' == pressed_key){
		double_check_before_quitting = 0; // Clear flag
		Flush_Array(user_input, &user_input_index, &first_op);
		pfCalculator_State_Handler = STATE_CALL(Result);
	}
	/* User pressed clear */
	else if('C' == pressed_key){
		LCD_Send_Command(LCD_CLEAR_DISPLAY);
		first_op = 0;
		second_op = 0;
		operation = 0;
		result = 0;
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
  * @Fn				- ST_Second_Operand
  * @brief 			- In this state, the system will store user entry in second operand array
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- This function will be called in Second_Operand state
  */
STATE_DEF(Second_Operand){
	/* State Name */
	calculator_states_id = Second_Operand;

	/* State Action */
	pressed_key = keypad_Get_Pressed_Key();

	/* User entered a number */
	if((0 <= pressed_key) && (10 > pressed_key)){
		double_check_before_quitting = 0; // Clear flag
		/* Validate that we are not writing outside array boundaries */
		if(user_input_index < ARRAY_MAX_SIZE){
			LCD_Send_Char(pressed_key+48);
			user_input[user_input_index] = pressed_key;
			user_input_index++;
		}
		else{ /* Do Nothing */ }
	}
	/* User entered a operation sign */
	else if(('+' == pressed_key) || ('-' == pressed_key) || ('x' == pressed_key) || ('/' == pressed_key)){
		double_check_before_quitting = 0; // Clear flag
		Flush_Array(user_input, &user_input_index, &second_op);
		result = Calculate_Result(first_op, second_op, operation);
		sprintf((char*)result_string, "%lu", result);
		operation = pressed_key;
		first_op = result;
		LCD_Send_string_Pos((uint8*)"ANS:            ", LCD_SECOND_ROW, 1);
		LCD_Send_string_Pos(result_string, LCD_SECOND_ROW, 6);
		LCD_Send_string_Pos((uint8*)"                ", LCD_FIRST_ROW, 1);
		LCD_Send_string_Pos((uint8*)"ANS", LCD_FIRST_ROW, 1);
		LCD_Send_Char(pressed_key);
	}
	/* User Pressed = */
	else if('=' == pressed_key){
		double_check_before_quitting = 0; // Clear flag
		Flush_Array(user_input, &user_input_index, &second_op);
		pfCalculator_State_Handler = STATE_CALL(Result);
	}
	/* User pressed clear */
	else if('C' == pressed_key){
		double_check_before_quitting = 1; // flag for first operand state that user pressed C
		first_op = 0;
		second_op = 0;
		operation = 0;
		result = 0;
		LCD_Send_Command(LCD_CLEAR_DISPLAY);
		pfCalculator_State_Handler = STATE_CALL(First_Operand);
	}
	else{ /* Do Nothing */ }
}

/**=============================================
  * @Fn				- ST_Result
  * @brief 			- In this state, the system will calculate the result and show it on LCD
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- This function will be called in Result state
  */
STATE_DEF(Result){
	/* State Name */
	calculator_states_id = Result;

	/* State Action */
	result = Calculate_Result(first_op, second_op, operation);
	sprintf((char*)result_string, "%lu", result);
	LCD_Send_string_Pos((uint8*)"ANS:            ", LCD_SECOND_ROW, 1);
	LCD_Send_string_Pos(result_string, LCD_SECOND_ROW, 6);

	do{
		pressed_key = keypad_Get_Pressed_Key();
	}while('F' == pressed_key);

	if((0 <= pressed_key) && (10 > pressed_key)){
		/* Validate that we are not writing outside array boundaries */
		if(user_input_index < ARRAY_MAX_SIZE){
			LCD_Send_Command(LCD_CLEAR_DISPLAY);
			LCD_Send_Char(pressed_key+48);
			user_input[user_input_index] = pressed_key;
			user_input_index++;
			pfCalculator_State_Handler = STATE_CALL(First_Operand);
		}
		else{ /* Do Nothing */ }
	}
	/* User entered a operation sign */
	else if(('+' == pressed_key) || ('-' == pressed_key) || ('x' == pressed_key) || ('/' == pressed_key)){
		LCD_Send_string_Pos((uint8*)"                ", LCD_FIRST_ROW, 1);
		operation = pressed_key;
		LCD_Send_string_Pos((uint8*)"ANS", LCD_FIRST_ROW, 1);
		first_op = result;
		LCD_Send_Char(operation);
		pfCalculator_State_Handler = STATE_CALL(Second_Operand);
	}
	/* User Pressed = */
	else if('=' == pressed_key){
//		Flush_Array(user_input, &user_input_index, &second_op);
		pfCalculator_State_Handler = STATE_CALL(Result);
	}
	/* User pressed clear */
	else if('C' == pressed_key){
		double_check_before_quitting = 1; // flag for first operand state that user pressed C
		first_op = 0;
		second_op = 0;
		operation = 0;
		result = 0;
		LCD_Send_Command(LCD_CLEAR_DISPLAY);
		pfCalculator_State_Handler = STATE_CALL(First_Operand);
	}
	else{ /* Do Nothing */ }
}
