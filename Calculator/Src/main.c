/*************************************************************************/
/* Author        : Omar Yamany                                    		 */
/* Project       : STM32F103C8T6_Drivers                               	 */
/* File          : main.c                           				 	 */
/* Date          : Jun 8, 2023                                           */
/* Version       : V1                                                    */
/* GitHub        : https://github.com/Piistachyoo             		     */
/*************************************************************************/

#include "app.h"

static void (*pfMain_State_Handler)(void) = NULL;
static void (*pfMain_User_Selection)(void) = NULL;
static main_states_t main_state_id;
static user_selection_t user_selection_flag = USER_UNDEFINED;

int main(void)
{
	/* Initial state is MAIN_INIT */
	pfMain_State_Handler = STATE_CALL(MAIN_INIT);
	pfMain_User_Selection = STATE_CALL(MAIN_SELECTION);
	while(1){
		pfMain_State_Handler();

	}
}

/**=============================================
  * @Fn				- clock_init
  * @brief 			- Initializes system clock
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- Initializing GPIOA and GPIOB for LCD and Keypad
  */
void clock_init(){
	RCC_GPIOA_CLK_EN();
	RCC_GPIOB_CLK_EN();
}

/**=============================================
  * @Fn				- MAIN_INIT
  * @brief 			- This function initializes clock, peripherals, LCD, and keypad
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- This function will be called in MAIN_INIT state
  */
STATE_DEF(MAIN_INIT){
	/* State Name */
	main_state_id = MAIN_INIT;

	/* State Action */
	/* Initialize peripherals */
	clock_init();
	LCD_Init();
	keypad_init();

	/* State transition */
	pfMain_State_Handler = STATE_CALL(MAIN_SELECTION);
}

/**=============================================
  * @Fn				- MAIN_SELECTION
  * @brief 			- This function asks the user to choose between calculator mode and numbering systems mode
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- This function will be called in MAIN_SELECTION state
  */
STATE_DEF(MAIN_SELECTION){
	/* State Name */
	main_state_id = MAIN_SELECTION;

	/* State Action */
	/* Ask the user to check between calculator mode and numbering system mode */
	uint8 pressed_key = 0;
	LCD_Send_Command(LCD_CLEAR_DISPLAY);
	LCD_Send_string_Pos((uint8*)"<<Calculator>>", LCD_FIRST_ROW, 2);
	LCD_Send_string_Pos((uint8*)"Select calc mode", LCD_SECOND_ROW, 0);
	MCAL_STK_Delay1ms(2500);
	LCD_Send_Command(LCD_CLEAR_DISPLAY);
	LCD_Send_string_Pos((uint8*)"1:Calculator", LCD_FIRST_ROW, 1);
	LCD_Send_string_Pos((uint8*)"2:Numbering Mode", LCD_SECOND_ROW, 1);

	/* Event Check */
	/* POLL untill user presses 1 or 2 */
	while((1 != pressed_key) && (2 != pressed_key)){
		pressed_key = keypad_Get_Pressed_Key();
	}

	user_selection_flag = pressed_key;

	LCD_Send_Command(LCD_CLEAR_DISPLAY);

	pfMain_State_Handler = STATE_CALL(MAIN_RUNNING);
}

/**=============================================
  * @Fn				- MAIN_RUNNING
  * @brief 			- This function will pass control to either calculator mode or numbering system mode
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- This function will be called in MAIN_RUNNING state
  */
STATE_DEF(MAIN_RUNNING){
	/* State Name */
	main_state_id = MAIN_RUNNING;

	/* State Action */
	if(USER_CALCULATOR == user_selection_flag){
		pfCalculator_State_Handler();
	}
	else if(USER_NUMBERING == user_selection_flag){
		pf_Numbering_State_Handler();
	}
	else{
		pfMain_State_Handler = STATE_CALL(MAIN_SELECTION);
	}

	/* Event Check */
	/* If user wants to reset, go back to selection state */
	if(1 == USER_RESET_FLAG){
		USER_RESET_FLAG = 0;
		pfMain_State_Handler = STATE_CALL(MAIN_SELECTION);
	}
}
