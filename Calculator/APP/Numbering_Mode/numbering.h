/*************************************************************************/
/* Author        : Omar Yamany                                    		 */
/* Project       : Calculator  	                             			 */
/* File          : numbering.h 			                           		 */
/* Date          : Jun 15, 2023                                          */
/* Version       : V1                                                    */
/* GitHub        : https://github.com/Piistachyoo             		     */
/*************************************************************************/
#ifndef NUMBERING_MODE_NUMBERING_H_
#define NUMBERING_MODE_NUMBERING_H_

//----------------------------------------------
// Section: Includes
//----------------------------------------------
#include "lcd_driver.h"
#include "keypad_driver.h"
#include "states.h"
#include <string.h>

//----------------------------------------------
// Section: User type definitions
//----------------------------------------------
typedef enum{
	Decimal_Mode,
	Octal_Mode,
	Binary_Mode,
	Hexadecimal_Mode,
	numbering_states_max
}numbering_states_t;

extern void (*pf_Numbering_State_Handler)(void);
extern uint8 USER_RESET_FLAG; // if 1, then user wants to restart the app

/*
 * =============================================
 * APIs Supported by "numbering"
 * =============================================
 */

/**=============================================
  * @Fn				- ST_Decimal_Mode
  * @brief 			- In this state, the number on the screen is displayed in decimal format
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- Initial state
  */
STATE_DEF(Decimal_Mode);

/**=============================================
  * @Fn				- ST_Octal_Mode
  * @brief 			- In this state, the number on the screen is displayed in octal format
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- None
  */
STATE_DEF(Octal_Mode);

/**=============================================
  * @Fn				- ST_Binary_Mode
  * @brief 			- In this state, the number on the screen is displayed in binary format
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- None
  */
STATE_DEF(Binary_Mode);

/**=============================================
  * @Fn				- ST_Hexadecimal_Mode
  * @brief 			- In this state, the number on the screen is displayed in hexadecimal format
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- None
  */
STATE_DEF(Hexadecimal_Mode);

#endif /* NUMBERING_MODE_NUMBERING_H_ */
