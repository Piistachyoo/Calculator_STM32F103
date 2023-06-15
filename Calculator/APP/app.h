/*************************************************************************/
/* Author        : Omar Yamany                                    		 */
/* Project       : Calculator  	                             			 */
/* File          : app.h 			                          			 */
/* Date          : Jun 14, 2023                                          */
/* Version       : V1                                                    */
/* GitHub        : https://github.com/Piistachyoo             		     */
/*************************************************************************/
#ifndef APP_H_
#define APP_H_

//----------------------------------------------
// Section: Includes
//----------------------------------------------
#include "lcd_driver.h"
#include "keypad_driver.h"
#include "states.h"
#include "calculator.h"
#include "numbering.h"



//----------------------------------------------
// Section: User type definitions
//----------------------------------------------
typedef enum{
	MAIN_INIT,
	MAIN_SELECTION,
	MAIN_RUNNING,
	MAIN_STATES_MAX
}main_states_t;

typedef enum{
	USER_UNDEFINED,
	USER_CALCULATOR,
	USER_NUMBERING,
	USER_SELCTION_MAX
}user_selection_t;

/*
 * =============================================
 * APIs Supported by "main"
 * =============================================
 */

/**=============================================
  * @Fn				- clock_init
  * @brief 			- Initializes system clock
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- Initializing GPIOA and GPIOB for LCD and Keypad
  */
void clock_init(void);

/**=============================================
  * @Fn				- my_delay
  * @brief 			- This function will make a delay without using a timer
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- None
  */
void my_delay(int x);

/**=============================================
  * @Fn				- ST_MAIN_INIT
  * @brief 			- This function initializes clock, peripherals, LCD, and keypad
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- This function will be called in MAIN_INIT state
  */
STATE_DEF(MAIN_INIT);

/**=============================================
  * @Fn				- ST_MAIN_SELECTION
  * @brief 			- This function asks the user to choose between calculator mode and numbering systems mode
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- This function will be called in MAIN_SELECTION state
  */
STATE_DEF(MAIN_SELECTION);

/**=============================================
  * @Fn				- ST_MAIN_RUNNING
  * @brief 			- This function will pass control to either calculator mode or numbering system mode
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- This function will be called in MAIN_RUNNING state
  */
STATE_DEF(MAIN_RUNNING);


#endif /* APP_H_ */
