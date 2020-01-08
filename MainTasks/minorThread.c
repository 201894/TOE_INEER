
/** @file detect_task.c
 *  @version 1.0
 *  @date Feb 2019
 *
 *  @brief detect module offline or online task
 *
 *
 */

#include "minorThread.h"
#include "cmsis_os.h"
#include "bsp_io.h"
#include "freertos.h"
#include "oled.h"
#include "adc.h"
#include "string.h"
#include "iwdg.h"

#define DETECT_TASK_PERIOD 100
UBaseType_t detect_stack_surplus;

/* detect task global parameter */
global_err_t g_err;
global_fps_t g_fps[MaxId];
global_fps_t r_fps[MaxId];
/* detect task static parameter */
static offline_dev_t offline_dev[MODE_CTRL_OFFLINE + 1];
/**
  * @brief     initialize detector error_list
  * @usage     used before detect loop in detect_task() function
  */
void detector_init(void)
{
	 g_err.err_now = NULL;
	 g_err.list[BOTTOM_DEVICE].dev    = NULL;
	 g_err.list[BOTTOM_DEVICE].enable = 0;

	 /* initialize device error type and offline timeout value */
	 for (uint8_t i = CAN_UPLIFT_LEFT_OFFLINE; i < ERROR_LIST_LENGTH; i++)
	 {		  
	  if (i <= MODE_CTRL_OFFLINE)
		{
		  offline_dev[i].set_timeout = 200; //ms
		  offline_dev[i].last_time   = 0;
		  offline_dev[i].delta_time  = 0;		  
		  g_err.list[i].dev  = &offline_dev[i];
		  g_err.list[i].type = DEV_OFFLINE;
		}
		else if (i == BULLET_JAM)
		{
		  g_err.list[i].dev  = NULL;
		  g_err.list[i].type = DEV_RUNNING_ERR;
		}
		else if (i <= GIMBAL_CONFIG_ERR)
		{
		  g_err.list[i].dev  = NULL;
		  g_err.list[i].type = SYS_CONFIG_ERR;
		}
	 }
		
	  /* initialize device error detect priority and enable byte */	  	
	  
	  g_err.list[CAN_FLIP_LEFT_OFFLINE].err_exist  = 0;
	  g_err.list[CAN_FLIP_LEFT_OFFLINE].pri        = 4;
	  g_err.list[CAN_FLIP_LEFT_OFFLINE].enable     = 1;
	  
	  g_err.list[CAN_FLIP_RIGHT_OFFLINE].err_exist  = 0;
	  g_err.list[CAN_FLIP_RIGHT_OFFLINE].pri        = 3;
	  g_err.list[CAN_FLIP_RIGHT_OFFLINE].enable     = 1;

	  g_err.list[CAN_SLIP_OFFLINE].err_exist = 0;
	  g_err.list[CAN_SLIP_OFFLINE].pri       = 2;
	  g_err.list[CAN_SLIP_OFFLINE].enable    = 1;
	  
	  g_err.list[MODE_CTRL_OFFLINE].err_exist    = 0;
	  g_err.list[MODE_CTRL_OFFLINE].pri          = 1;
	  g_err.list[MODE_CTRL_OFFLINE].enable       = 1;
}  
/**
  * @brief     record the detected module return time to judge offline
  * @param     err_id: module id
  * @retval    None
  * @usage     used in CAN/usart.. rx interrupt callback
  */
void err_detector_hook(int err_id)
{
  if (g_err.list[err_id].enable)
      g_err.list[err_id].dev->last_time = HAL_GetTick();
}

void detector_param_init(void)
{
  detector_init();
  g_err.beep_ctrl = 0;  
}

/**
  * @brief     according to the interval time
  * @param     err_id: module id
  * @retval    None
  * @usage     used in CAN/usart.. rx interrupt callback
  */
uint32_t detect_time_last;
int detect_time_ms;
void minorThread(void const *argu)
{
  LED_R_OFF;
  LED_G_OFF;
  uint32_t detect_wake_time = osKernelSysTick();
		
  while(1)
  {
    detect_time_ms = HAL_GetTick() - detect_time_last;
    detect_time_last = HAL_GetTick();
    /* module offline detect */
    module_offline_detect();
    if (g_err.err_now != NULL)
    {
      LED_R_ON;
      module_offline_callback();
    }
    else
    {
      g_err.beep_ctrl = 0;
//      LED_G_ON;
    }
		OLED_CTRL();
    module_fps_detect();	
    module_fps_clear();
		HAL_IWDG_Refresh(&hiwdg);
   // detect_stack_surplus = uxTaskGetStackHighWaterMark(NULL);    
    osDelayUntil(&detect_wake_time, DETECT_TASK_PERIOD);
  }
}
  
static void module_offline_detect(void)
{
  int max_priority = 0;
  int err_cnt      = 0;
  for (uint8_t id = CAN_UPLIFT_LEFT_OFFLINE; id <= MODE_CTRL_OFFLINE; id++)
  {
    g_err.list[id].dev->delta_time = HAL_GetTick() - g_err.list[id].dev->last_time;
    if (g_err.list[id].enable 
        && (g_err.list[id].dev->delta_time > g_err.list[id].dev->set_timeout))
    {
      g_err.list[id].err_exist = 1; //this module is offline
      err_cnt++;
      if (g_err.list[id].pri > max_priority)
      {
        max_priority     = g_err.list[id].pri;
        g_err.err_now    = &(g_err.list[id]);
        g_err.err_now_id = (err_id_e)id;
		  
      }
    }
    else
    {
      g_err.list[id].err_exist = 0;
    }
  }

  if (!err_cnt)
  {
    g_err.err_now    = NULL;
    g_err.err_now_id = BOTTOM_DEVICE;
  }
}

static void module_offline_callback(void)
{
	
}

uint8_t canFlag=0;
static void module_fps_detect(void)
{
	for (int i = 0; i < MaxId; i++)
	{
		g_fps[i].fps = g_fps[i].cnt * 1000/DETECT_TASK_PERIOD;
	}
	if(g_fps[0].fps>400)
		canFlag = 1;
}

static void module_fps_clear(void)
{
	  for (int i = 0; i < MaxId; i++)
		{
			memcpy(&r_fps[i], &g_fps[i], sizeof(global_fps_t));								
			memset(&g_fps[i], 0, sizeof(global_fps_t));					
		}
}

void OLED_CTRL(void)
{
		OLED_Button();
		oled_clear(Pen_Clear);		
		switch (OLED_ADC_flag)
		{
			case 0 :
			{
				oled_LOGO();			
			}break;
			case 1 :
			{
				/*OLED PRINTF FPS REVELENT*/

				oled_printf(1,2,"RFLIP FPS");
				oled_shownum(1,14,r_fps[RightFlip].fps,0x00,4);	

				oled_printf(2,2,"LFLIP FPS");				
			  oled_shownum(2,14,r_fps[LeftFlip].fps,0x00,4);	
				
				oled_printf(3,2,"MSLIP FPS");	
				oled_shownum(3,14,r_fps[MidSlip].fps,0x00,4);					
				
				oled_printf(4,2,"MARID FPS");			
				oled_shownum(4,14,r_fps[MasterID].fps,0x00,4);	
					
			}break;
			case 2 :
			{
				/*OLED PRINTF GNS STATE REVELENT*/

				oled_printf(1,3,"LEFT_GNS");
				oled_shownum(1,14,LEFT_GNS(),0x00,4);	

				oled_printf(2,3,"MIDD_GNS");				
			  oled_shownum(2,14,MID_GNS(),0x00,4);	
				
				oled_printf(3,3,"RIGT_GNS");	
				oled_shownum(3,14,RIGHT_GNS(),0x00,4);						
			}break;
			case 3 :
			{
				
			}break;			
			case 4 :
			{
				
			}break;			
			case 5 :
			{
				
			}break;						
		}
				
    oled_refresh_gram();	
}



