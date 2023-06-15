/*************************************************************************/
/* Author        : Omar Yamany                                    		 */
/* Project       : Calculator  	                             			 */
/* File          : calculator.h 			                             */
/* Date          : Jun 15, 2023                                          */
/* Version       : V1                                                    */
/* GitHub        : https://github.com/Piistachyoo             		     */
/*************************************************************************/
#ifndef CALCULATE_MODE_CALCULATOR_H_
#define CALCULATE_MODE_CALCULATOR_H_

//----------------------------------------------
// Section: Includes
//----------------------------------------------
#include <stdio.h>
#include <string.h>
#include "lcd_driver.h"
#include "keypad_driver.h"
#include "states.h"

//----------------------------------------------
// Section: User type definitions
//----------------------------------------------
typedef enum{
	First_Operand,
	Second_Operand,
	Result
}calculator_states_t;

//----------------------------------------------
// Section: Macros Configuration References
//----------------------------------------------

// @ref Module_Ref_Name_define


/*
 * =============================================
 * APIs Supported by "calculator"
 * =============================================
 */

/**=============================================
  * @Fn				- ST_First_Operand
  * @brief 			- In this state, the system will store user entry in first operand array
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- This function will be called in First_Operand state
  */
STATE_DEF(First_Operand);

/**=============================================
  * @Fn				- ST_Second_Operand
  * @brief 			- In this state, the system will store user entry in second operand array
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- This function will be called in Second_Operand state
  */
STATE_DEF(Second_Operand);

/**=============================================
  * @Fn				- ST_Result
  * @brief 			- In this state, the system will calculate the result and show it on LCD
  * @param [in] 	- None
  * @param [out] 	- None
  * @retval 		- None
  * Note			- This function will be called in Result state
  */
STATE_DEF(Result);

#endif /* CALCULATE_MODE_CALCULATOR_H_ */
