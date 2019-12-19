/** @file logic_handle_task.h
 *  @version 1.0
 *  @date  DEC 2019
 *
 *  @brief ALL MOTORS PID HANDLE
 *
 */

#ifndef  __LOGIC_HANDLE_TASK_H__
#define __LOGIC_HANDLE_TASK_H__

#include "stm32f4xx_hal.h"

typedef enum
{
  SAFETY_MODE	=0,
  NORMAL_MODE,	
  AUTO_MODE,	
} ctrl_mode_e;

typedef enum
{
  MIN_Z_POSITION	=0,
  MID_Z_POSITION,	
  MAX_Z_POSITION,	
} uplift_position_e;

typedef enum
{
  MIN_R_ANGLE	=0,
  MID_R_ANGLE,	
  MAX_R_ANGLE,	
} flip_angle_e;

typedef enum
{
  MIN_Y_POSITION	=0,
  MID_Y_POSITION,	
  MAX_Y_POSITION,	
} slip_position_e;

typedef struct
{
  uint8_t 	mode;
	uint8_t   flag;
  float     target;
  float     tempo;	
	
} moto_ctrl_t;

typedef struct
{
  uint8_t 	raw_mode;		
  uint8_t 	global_mode;
	
} logic_data_t;

extern moto_ctrl_t moto_ctrl[3];
extern logic_data_t logic_data;
void moto_param_init(void);

void flagClear(void);
#endif