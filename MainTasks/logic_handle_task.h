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
	
	VV_SHIFT_TO_RIGHT = 0, // ΢΢ƫ��
	VV_SHIFT_TO_LEFT,  // ΢΢ƫ��
	SHIFT_TO_RIGHT,  //  ƫ��
	SHIFT_TO_LEFT,	  // ƫ��
	ERROR_STATE,
	CORRECT_STATE,
} claw_state_e;
typedef enum
{
	INIT_R_ANGLE = 0,						// ��ʼ �Ƕ�
  MIN_R_ANGLE = 20,					// ж�� �Ƕ� 
	EVALVE_SELFLOCK_ANGLE = 80,    // ���������Ƕ�
  MID_R_ANGLE = 90,			    // ɨ�� �Ƕ� ��ֱ
  MAX_R_ANGLE = 180,				// ��ȡ �Ƕ�
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
	uint8_t   firstBoxFlag;  // ��ҩ�� �Ƿ�Ϊ��һ�� ��־λ �� 1 �� 0
	uint8_t   boxNumber;   // ��ȡ�� ��ҩ�� ����
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