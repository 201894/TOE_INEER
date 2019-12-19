
/** @file bsp_io.h
 *  @version 1.0
 *  @date Dec 2019
 *
 *  @brief basic IO port operation
 *
 *  HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
 */
#ifndef __BSP_IO_H__
#define __BSP_IO_H__
#include "stm32f4xx_hal.h"
#include  "gpio.h"
// GN4 REVELENT 
#define LeftGN     HAL_GPIO_ReadPin(INA_GPIO_Port, INA_Pin)
#define MiddleGN   HAL_GPIO_ReadPin(INB_GPIO_Port, INB_Pin)
#define RightGN    HAL_GPIO_ReadPin(INC_GPIO_Port, INC_Pin)
// LED REVELENT 
#define LED_G_ON   HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_RESET)
#define LED_G_OFF  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_14, GPIO_PIN_SET)
#define LED_R_ON   HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_RESET)
#define LED_R_OFF  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_SET)
#define DETECT_FLOW_LED_ON(i) 		flow_led_on(i)
#define DETECT_FLOW_LED_OFF(i) 	flow_led_off(i)
// EVALVE REVELENT
#define EVALVE_GPIO_PORT   	GPIOH
#define HOOK_PIN											GPIO_PIN_2		
#define GATE_PIN 											GPIO_PIN_3
	/*		SparePin £ºPH4 PH5			*/
void busser_task(void);
void evalve_init(void);
void GPIO_InitArgument(void);
void flow_led(void);
void flow_led_on(uint16_t num);
void flow_led_off(uint16_t num);
#endif
