/*************************************************************************/
/* Author        : Omar Yamany                                    		 */
/* Project       : STM32F103C8T6_Drivers  	                             */
/* File          : states.h 			                         	     */
/* Date          : Jun 14, 2023                                          */
/* Version       : V1                                                    */
/* GitHub        : https://github.com/Piistachyoo             		     */
/*************************************************************************/
#ifndef STATES_H_
#define STATES_H_

#define STATE_DEF(_VA_ARGS_)	void ST_##_VA_ARGS_(void)
#define STATE_CALL(_VA_ARGS_)	ST_##_VA_ARGS_

#endif /* STATES_H_ */
