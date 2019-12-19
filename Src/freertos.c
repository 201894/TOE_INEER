/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "bsp_uart.h"
#include "STMGood.h"
#include "bsp_can.h"
#include "pid.h"
#include "detect_task.h"
#include "logic_handle_task.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId PID_TASKHandle;
osThreadId LOGICTASKHandle;
osThreadId DETECTASKHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void debug_task(void const * argument);
void pid_handle_task(void const * argument);
void logic_handle_task(void const * argument);
void detect_task(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);

/* USER CODE BEGIN 4 */
__weak void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, debug_task, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of PID_TASK */
  osThreadDef(PID_TASK, pid_handle_task, osPriorityIdle, 0, 256);
  PID_TASKHandle = osThreadCreate(osThread(PID_TASK), NULL);

  /* definition and creation of LOGICTASK */
  osThreadDef(LOGICTASK, logic_handle_task, osPriorityIdle, 0, 256);
  LOGICTASKHandle = osThreadCreate(osThread(LOGICTASK), NULL);

  /* definition and creation of DETECTASK */
  osThreadDef(DETECTASK, detect_task, osPriorityIdle, 0, 128);
  DETECTASKHandle = osThreadCreate(osThread(DETECTASK), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_debug_task */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_debug_task */
void debug_task(void const * argument)
{
  /* USER CODE BEGIN debug_task */
  /* Infinite loop */
  for(;;)
  {
		/*
		typedef enum
		{
			LeftUpLift        = 0,  //
			RightUpLift       = 1,	
			LeftFlip          = 2,  
			RightFlip         = 3,
			MidSlip           = 4,
			MaxId             = 10,
		}module_id;
		*/		
    #if 1
			printf("##FPS REVENLENT : ##\r\n");				
			printf("FPS[LeftUpLift] 		= %d \r\n",g_fps[0].fps);
			printf("FPS[RightUpLift] 	= %d \r\n",g_fps[1].fps);		
			printf("FPS[LeftFlip] 				= %d \r\n",g_fps[2].fps);
			printf("FPS[RightFlip] 			= %d \r\n",g_fps[3].fps);
			printf("FPS[MidSlip] 				= %d \r\n",g_fps[4].fps);
			printf("FPS[MasterID] 				= %d \r\n",g_fps[MasterID].fps);		
		#endif
    #if 1
			printf("MOTO ECD REVENLENT : ##\r\n");				
			printf("MotoData[LeftUpLift] 		= %d \r\n",MotoData[LeftUpLift].ecd);
			printf("MotoData[RightUpLift] 	= %d \r\n",MotoData[RightUpLift].ecd);		
			printf("MotoData[LeftFlip] 				= %d \r\n",MotoData[LeftFlip].ecd);		
			printf("MotoData[RightFlip] 		= %d \r\n",MotoData[RightFlip].ecd);		
			printf("MotoData[MidSlip] 				= %d \r\n",MotoData[MidSlip].ecd);					
		#endif		
		
    #if 0 
			printf("UPLIFT PID REVENLENT : ##\r\n");				
			printf("TargetAngle  	= %.2f\r\n",moto_ctrl[UpLift].target);	
			printf("CurrentAngle 	= %.2f\r\n",MotoData[LeftUpLift].total_angle);	
			printf("AngleError  		= #%.2f\r\n",pid_out[UpLift].errNow);	
			printf("FirstCtrOut  		= %.2f\r\n",pid_out[UpLift].ctrOut);				
			printf("CurrentSpd 		= %df\r\n",MotoData[LeftUpLift].speed_rpm);	 // MotoData[RightUpLift].speed_rpm
			printf("SpdError  		    = %.2f\r\n",pid_in[LeftUpLift].ctrOut);			
			printf("FinalCtrOut  		= %d\r\n",(int16_t)pid_in[LeftUpLift].ctrOut);					
		#endif			
    #if 0
			printf("FLIP PID REVENLENT : ##\r\n");				
			printf("TargetAngle  	= %.2f\r\n",moto_ctrl[Flip].target);	
			printf("CurrentAngle 	= %.2f\r\n",MotoData[LeftFlip].total_angle);	
			printf("AngleError  		= #%.2f\r\n",pid_out[Flip].errNow);	
			printf("FirstCtrOut  		= %.2f\r\n",pid_out[Flip].ctrOut);				
			printf("CurrentSpd 		= %df\r\n",MotoData[LeftFlip].speed_rpm);	 // MotoData[RightUpLift].speed_rpm
			printf("SpdError  		    = %.2f\r\n",pid_in[LeftFlip].ctrOut);			
			printf("FinalCtrOut  		= %d\r\n",(int16_t)pid_in[LeftFlip].ctrOut);					
		#endif				
    #if 0
			printf("SLIP PID REVENLENT : ##\r\n");				
			printf("TargetAngle  	= %.2f\r\n",moto_ctrl[Slip].target);	
			printf("CurrentAngle 	= %.2f\r\n",MotoData[MidSlip].total_angle);	
			printf("AngleError  		= #%.2f\r\n",pid_out[Slip].errNow);	
			printf("FirstCtrOut  		= %.2f\r\n",pid_out[Slip].ctrOut);				
			printf("CurrentSpd 		= %df\r\n",MotoData[MidSlip].speed_rpm);	 // MotoData[RightUpLift].speed_rpm
			printf("SpdError  		    = %.2f\r\n",pid_in[MidSlip].ctrOut);			
			printf("FinalCtrOut  		= %d\r\n",(int16_t)pid_in[MidSlip].ctrOut);					
		#endif			
    osDelay(200);
  }
  /* USER CODE END debug_task */
}

/* USER CODE BEGIN Header_pid_handle_task */
/**
* @brief Function implementing the PID_TASK thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_pid_handle_task */
__weak void pid_handle_task(void const * argument)
{
  /* USER CODE BEGIN pid_handle_task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END pid_handle_task */
}

/* USER CODE BEGIN Header_logic_handle_task */
/**
* @brief Function implementing the LOGICTASK thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_logic_handle_task */
__weak void logic_handle_task(void const * argument)
{
  /* USER CODE BEGIN logic_handle_task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END logic_handle_task */
}

/* USER CODE BEGIN Header_detect_task */
/**
* @brief Function implementing the DETECTASK thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_detect_task */
__weak void detect_task(void const * argument)
{
  /* USER CODE BEGIN detect_task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END detect_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
