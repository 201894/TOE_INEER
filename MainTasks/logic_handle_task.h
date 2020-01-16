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
  NULL_FETCH	 = 0,
  FOUR_BOX_FETCH,	// 资源岛 开局 四箱
  THREE_BOX_FETCH,	// 小资源岛 三箱
  TWO_BOX_FETCH,		// 资源岛 四分钟 两箱
} fetch_mode_e;
typedef enum
{
	INIT_TASK = 0,
	SLIP_TASK1,
	DETECT_TASK,
	CLAMP_TASK,	
	UPLIFT_TASK,		
	UPLOAD_TASK,
	SLIP_TASK2,
	THROW_TASK,
	QUIT_TASK,
}fetch_task_cnt;

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
  MIN_R_ANGLE = -20,					// 卸荷 角度 
	EVALVE_SELFLOCK_ANGLE = -110,    // 气缸自锁角度
  MID_R_ANGLE = -90,			    // 扫描 角度 垂直
  MAX_R_ANGLE = -182,				// 夹取 角度
} flip_angle_e;

typedef enum
{
  LEFT_S_POSITION	= -220,
  MID_S_POSITION = 0,	
  POSITION_DIFFER = 220, 
  RIGHT_S_POSITION	= 220,	
	
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
  uint8_t 	fetch_mode;	
	uint8_t 	last_fetch_mode;	
	uint8_t   upLiftPosFlag;
	uint8_t   upLiftPosMaxFlag;
	uint8_t   task_cnt;       //  
	uint8_t   loop_cnt;
	uint8_t   quitFlag;
	uint8_t   endFlag;
	uint8_t   firstBoxFlag;  // 弹药箱 是否为第一箱 标志位 是 1 否 0
	uint8_t   boxNumber;   // 已取得 弹药箱 数量
	uint8_t   upLiftSelfLockFlag;
 	float  CalibratedMidAngle;
  fetch_mode_e 	global_mode;
	claw_state_e  clawState;
	GPIO_PinState clampFlag;	
	GPIO_PinState lengthFlag;	
	GPIO_PinState bounceFlag;
	GPIO_PinState bounceSelfLockFlag;	
} logic_data_t;

extern moto_ctrl_t moto_ctrl[3];
extern logic_data_t logic_data;
uint8_t slipDetect_Action(void);
uint8_t flip_to_max(float error);
void moto_param_init(void);
void fetch_ctrl(void);
void fetch_mode_handle1(void);
void fetch_mode_handle2(void);
void slip_target_handle(void);
void flagClear(void);
#endif