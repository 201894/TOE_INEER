/** @file logic_handle_task.h
 *  @version 1.0
 *  @date  DEC 2019
 *
 *  @brief ALL MOTORS PID HANDLE
 *
 */

#ifndef  __LOGIC_HANDLE_TASK_H__
#define __LOGIC_HANDLE_TASK_H__

#include "stm32f1xx_hal.h"

/* Global Mode Relevent*/
typedef enum
{
  SAFETY_MODE	=0,
  NORMAL_MODE,	
  AUTO_MODE,	
} ctrl_mode_e;
/* Claw State Revelent */
typedef enum
{
	
	VV_SHIFT_TO_RIGHT = 0, // 微微偏右
	VV_SHIFT_TO_LEFT,  // 微微偏左
	SHIFT_TO_RIGHT,  //  偏右
	SHIFT_TO_LEFT,	  // 偏左
	ERROR_STATE,
	CORRECT_STATE,
} claw_state_e;
typedef enum
{
	INIT_R_ANGLE = 0,						// 起始 角度
  MIN_R_ANGLE = 20,					// 卸荷 角度 
	EVALVE_SELFLOCK_ANGLE = 80,    // 气缸自锁角度
  MID_R_ANGLE = 90,			    // 扫描 角度 垂直
  MAX_R_ANGLE = 180,				// 夹取 角度
} flip_angle_e;

typedef enum
{
  LEFT_S_POSITION	= -300,
  MID_S_POSITION = 0,	
  POSITION_DIFFER = 300, 
  RIGHT_S_POSITION	= 300,	
	
} slip_position_e;

typedef struct
{
  uint8_t 	mode;
	uint8_t   flag;
  float   target;
  float   tempo;	
	
} moto_ctrl_t;

typedef struct
{
  uint8_t 	raw_mode;		
	uint8_t   task_cnt;       //  
	uint8_t   loop_cnt;
	uint8_t   endFlag;
	uint8_t   firstBoxFlag;  // 弹药箱 是否为第一箱 标志位 是 1 否 0
	uint8_t   boxNumber;   // 已取得 弹药箱 数量
 	float  CalibratedMidAngle;
  ctrl_mode_e 	global_mode;
	claw_state_e  clawState;
	GPIO_PinState clampFlag;	
	GPIO_PinState lengthFlag;	
	GPIO_PinState bounceFlag;
	GPIO_PinState bounceSelfLockFlag;	
} logic_data_t;

extern moto_ctrl_t moto_ctrl[3];
extern logic_data_t logic_data;
void moto_param_init(void);
void first_row_fetch(void);
void second_row_fetch(void);
void Main_Logic_Ctrl(void);
void slip_target_handle(void);
void flagClear(void);
#endif