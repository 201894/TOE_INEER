
/** @file detect_task.h
 *  @version 1.1
 *  @date Feb 2019
 *
 *  @brief detect module offline or online task
 *
 *
 */

#ifndef __MINORTHREAD_H__
#define __MINORTHREAD_H__

#include "stm32f1xx_hal.h"
#include "bsp_can.h"
/* detect task period time (ms) */

/* FPS THRESHOLD*/
#define  MOTO_FPS_THRESHOLD       900
typedef enum
{
  BOTTOM_DEVICE            	= 0,
  CAN_UPLIFT_LEFT_OFFLINE  	= 1,
  CAN_UPLIFT_RIGHT_OFFLINE 	= 2,
  CAN_FLIP_LEFT_OFFLINE    	= 3,
  CAN_FLIP_RIGHT_OFFLINE   	= 4,
  CAN_SLIP_OFFLINE         	= 5,
  MODE_CTRL_OFFLINE        	= 7,
  BULLET_JAM               	= 8,
  CHASSIS_CONFIG_ERR   		 	= 9,
  GIMBAL_CONFIG_ERR   	 		= 10,
  ERROR_LIST_LENGTH    			= 11,
} err_id_e;

typedef enum
{
  DEV_OFFLINE     = 0,
  DEV_RUNNING_ERR = 1,
  SYS_CONFIG_ERR  = 2,
} err_type_e;

typedef struct
{
  uint16_t set_timeout;
  uint16_t delta_time;
  uint32_t last_time;
} offline_dev_t;

typedef struct
{
  /* enable the device error detect */
  uint8_t  enable;
  /* device error exist flag */
  uint8_t  err_exist;
  /* device error priority */
  uint8_t  pri;
  /* device error type */
  uint8_t  type;
  /* the pointer of device offline param */
  offline_dev_t *dev;
} err_dev_t;

typedef struct
{
  /* the pointer of the highest priority error device */
  err_dev_t *err_now;
  err_id_e  err_now_id;
  /* all device be detected list */
  err_dev_t list[ERROR_LIST_LENGTH];
  /* error alarm relevant */
  uint16_t err_count;
  uint16_t beep_tune;
  uint16_t beep_ctrl;
} global_err_t;

typedef struct
{
  /* error alarm relevant */
  uint16_t fps;
  uint16_t cnt;
  uint16_t beep_ctrl;
	
} global_fps_t;

extern global_err_t g_err;
extern global_fps_t r_fps[MaxId];
extern global_fps_t g_fps[MaxId];
void detector_init(void);
void OLED_CTRL(void);
void err_detector_hook(int err_id);
void detector_param_init(void);
static void module_offline_callback(void);
static void module_offline_detect(void);
static void module_fps_detect(void);
static void module_fps_clear(void);

#endif