
/** @file bsp_io.c
 *  @version 1.0
 *  @date  2019  12.19
 *
 *  @brief basic IO port operation
 *
 */
#include "logic_handle_task.h"
#include "bsp_io.h"
#include "oled.h"

void evalve_init(void)
{
		HAL_GPIO_WritePin(EVALVE_GPIO_PORT	, CLAMP_CTRL, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(EVALVE_GPIO_PORT	, LENGTH_CTRL, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(EVALVE_GPIO_PORT	, BOUNCE_CTRL, GPIO_PIN_RESET);
}

void led_init(void)
{


}

void GPIO_InitArgument(void)
{
	evalve_init();
	led_init();
	oled_init();
}

uint8_t LEFT_GNS(void)
{
	uint8_t i;
	float sum;
	float average;
	for(i=1;i<11;i++)
	{
		sum += Left_GNS;
		average = sum/i;
	}	
	if(average <= 0.2)
		return 1;
	else
		return 0;
}

uint8_t MID_GNS(void)
{
	uint8_t i;
	float sum;
	float average;
	for(i=1;i<11;i++)
	{
		sum += Mid_GNS;
		average = sum/i;
	}	
	if(average <= 0.2)
		return 1;
	else
		return 0;
}	
	
uint8_t RIGHT_GNS(void)
{
	uint8_t i;
	float sum;
	float average;
	for(i=1;i<11;i++)
	{
		sum += Right_GNS;
		average = sum/i;
	}	
	if(average <= 0.2)
		return 1;
	else
		return 0;
}

uint8_t FLIP_SWITCH(void)
{
	uint8_t i;
	float sum;
	float average;
	for(i=1;i<11;i++)
	{
		sum += Flip_SWITCH;
		average = sum/i;
	}	
	if(average <= 0.2)
		return 1;
	else
		return 0;
}

void GNS_STATE_DETECT(void)
{
/* 基恩士状态更新，防抖  */
			LEFT_GNS();	
			MID_GNS();
			RIGHT_GNS();	
			FLIP_SWITCH();
/* 爪子状态更新 获取  */	
		if(LEFT_GNS() == SET){
				if(MID_GNS() == SET){
						if(RIGHT_GNS() == SET){
							/*  1 1 1 */
							logic_data.clawState = ERROR_STATE;
						} 
						else{
							/*  1 1 0 */	
							logic_data.clawState = VV_SHIFT_TO_RIGHT;
						}
				}
				else{
						if(RIGHT_GNS() == SET){	
							/*  1 0 1 */						
							logic_data.clawState = SHIFT_TO_RIGHT;							
						}
						else{
							/*  1 0 0 */	
							logic_data.clawState = SHIFT_TO_RIGHT;							
						}						
				}
		}
		else{
				if(MID_GNS() == SET){
						if(RIGHT_GNS() == SET){
							/*  0 1 1 */						
							logic_data.clawState = VV_SHIFT_TO_LEFT;										
						}
						else{
							/*  0 1 0  */					
							logic_data.clawState = CORRECT_STATE;
						}
				}
				else{
						if(RIGHT_GNS() == SET){
							/*  0 0 1 */					
							logic_data.clawState = SHIFT_TO_LEFT;										
						}
						else{
							/*  0 0 0 */
							logic_data.clawState = SHIFT_TO_LEFT;									 
						}						
				}		
		}
}

