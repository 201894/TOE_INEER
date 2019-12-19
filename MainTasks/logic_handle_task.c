/** @file logic_handle_task.h
 *  @version 1.0
 *  @date  DEC 2019
 *
 *  @brief ALL MOTORS PID HANDLE
 *
 */
 
#include "logic_handle_task.h"
#include "pid_handle_task.h"
#include "cmsis_os.h"
#include "bsp_can.h"
#include "bsp_io.h"
#include "pid.h"
#include "STMGood.h"

#define LOGIC_TASK_PERIOD 5
moto_ctrl_t moto_ctrl[3]; 
logic_data_t logic_data;
void logic_handle_task(void const * argument)
{
  /* USER CODE BEGIN logic_handle_task */
	portTickType LogicHandleLastWakeTime;
  /* Infinite loop */
  for(;;)
  {
		LogicHandleLastWakeTime = xTaskGetTickCount();		
		taskENTER_CRITICAL();

		taskEXIT_CRITICAL();
    osDelayUntil(&LogicHandleLastWakeTime,LOGIC_TASK_PERIOD);			
  }
  /* USER CODE END pid_handle_task */
}
 
 

