/** @file bsp_io.h
 *  @version 1.0
 *  @date Jan 2019
 *
 *  @brief basic IO port operation
 *
 *  HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
 */
#ifndef __BSP_IO_H__
#define __BSP_IO_H__
#include "stm32f1xx_hal.h"
#include  "gpio.h"

// GNS REVELENT
#define Left_OMRON HAL_GPIO_ReadPin(GNS_GPIO_PORT, S_IN3)
#define Mid_OMRON HAL_GPIO_ReadPin(GNS_GPIO_PORT, S_IN1)
#define Right_OMRON HAL_GPIO_ReadPin(GNS_GPIO_PORT, S_IN4)
#define Flip_SWITCH HAL_GPIO_ReadPin(GNS_GPIO_PORT, S_IN4)
// EVALVE REVELENT
#define	 EVALVE_GPIO_PORT GPIOA
#define CLAMP_CTRL	POWER1_CTRL		
#define LENGTH_CTRL POWER3_CTRL
#define BOUNCE_CTRL POWER2_CTRL
// LED REVELENT
#define 	LED_G_ON   HAL_GPIO_WritePin(GPIOB	, GPIO_PIN_0, GPIO_PIN_RESET)
#define 	LED_G_OFF HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET)
#define 	LED_R_ON   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET)
#define 	LED_R_OFF HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET)
#define LED_G_TOG  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0)
#define LED_R_TOG  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1)

void evalve_init(void);
//void evalve_ctrl(void);
void led_init();
uint8_t LEFT_OMRON(void);
uint8_t MID_OMRON(void);
uint8_t RIGHT_OMRON(void);
uint8_t FLIP_SWITCH(void);
void OMRON_STATE_DETECT(void);
void GPIO_InitArgument(void);

#endif
