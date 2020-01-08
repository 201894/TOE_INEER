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
#include "bsp_can.h"
#include "bsp_io.h"
#include "bsp_uart.h"
#include "minorThread.h"
#include "logic_handle_task.h"
#include "pid.h"
#include "oled.h"
#include "adc.h"
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
osThreadId PidThreadHandle;
osThreadId MainTaskHandle;
osThreadId MinorTakHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void debugThread(void const * argument);
void pidThread(void const * argument);
void mainThread(void const * argument);
void minorThread(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

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
  osThreadDef(defaultTask, debugThread, osPriorityIdle, 0, 256);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of PidThread */
  osThreadDef(PidThread, pidThread, osPriorityIdle, 0, 256);
  PidThreadHandle = osThreadCreate(osThread(PidThread), NULL);

  /* definition and creation of MainTask */
  osThreadDef(MainTask, mainThread, osPriorityIdle, 0, 256);
  MainTaskHandle = osThreadCreate(osThread(MainTask), NULL);

  /* definition and creation of MinorTak */
  osThreadDef(MinorTak, minorThread, osPriorityIdle, 0, 128);
  MinorTakHandle = osThreadCreate(osThread(MinorTak), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_debugThread */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_debugThread */
void debugThread(void const * argument)
{
  /* USER CODE BEGIN debugThread */
  /* Infinite loop */
  for(;;)
  {

#if 0
		printf("##FPS REVENLENT : ##\r\n");		
		printf("fps[LeftFlip] = %d\r\n",r_fps[LeftFlip].fps);
		printf("fps[RightFlip] = %d\r\n",r_fps[RightFlip].fps);	
		printf("fps[MasterID] = %d\r\n",r_fps[MasterID].fps);
		printf("fps[MidSlip] = %d\r\n",r_fps[MidSlip].fps);    							
#endif		
#if 0
		printf("# MOTO ECD REVENLENT #: \r\n");				
		//			printf("MotoData[LeftFlip] = %d \r\n",MotoData[LeftFlip].ecd);		
		//			printf("MotoData[RightFlip] = %d \r\n",MotoData[RightFlip].ecd);		
		printf("MotoData[MidSlip] = %d \r\n",MotoData[MidSlip].ecd);						
#endif	
#if 0
		printf("# LOGIC DATA REVENLENT #: \r\n");				
		//			printf("MotoData[LeftFlip] = %d \r\n",MotoData[LeftFlip].ecd);		
		//			printf("MotoData[RightFlip] = %d \r\n",MotoData[RightFlip].ecd);		
		printf("Raw_mode = %d \r\n",logic_data.raw_mode);	
		printf("moto[Slip].target = %.3f\r\n",moto_ctrl[Slip].target);			
#endif			
#if 0
		printf("SLIP PID REVENLENT : ##\r\n");				
		printf("TargetAngle = %.2f\r\n",moto_ctrl[Slip].target);	
		printf("CurrentAngle = %.2f\r\n",MotoData[MidSlip].total_angle);	
		printf("AngleError = #%.2f\r\n",pid_out[Slip].errNow);	
		printf("FirstCtrOut = %.2f\r\n",pid_out[Slip].ctrOut);				
		printf("CurrentSpd = %d\r\n",MotoData[MidSlip].speed_rpm);	 // MotoData[RightUpLift].speed_rpm
		printf("SpdError = %.2f\r\n",pid_in[MidSlip].errNow);			
		printf("FinalCtrOut = %d\r\n",(int16_t)pid_in[MidSlip].ctrOut);					
#endif				

#if 1
		printf("# FLIP PID REVENLENT : #\r\n");				
		printf("TargetAngle = %.2f\r\n",moto_ctrl[Flip].target);	
		printf("CurrentAngle = %.2f\r\n",MotoData[LeftFlip].total_angle);	
		printf("AngleError = %.2f\r\n",pid_out[Flip].errNow);	
		printf("FirstCtrOut = %.2f\r\n",pid_out[Flip].ctrOut);				
		printf("CurrentSpd = %d\r\n",MotoData[LeftFlip].speed_rpm);	 // MotoData[RightUpLift].speed_rpm
		printf("SpdError = %.2f\r\n",pid_in[LeftFlip].errNow);			
		printf("LeftFlipCtrOut = %d\r\n",(int16_t)pid_in[LeftFlip].ctrOut);
		printf("RightFlipCtrOut = %d\r\n",(int16_t)pid_in[RightFlip].ctrOut);		
#endif				

		LED_G_TOG; 


		HAL_GPIO_WritePin(EVALVE_GPIO_PORT	, CLAMP_CTRL, OLED_ADC_flag);

    osDelay(200);
  }
  /* USER CODE END debugThread */
}

/* USER CODE BEGIN Header_pidThread */
/**
* @brief Function implementing the PidThread thread.
* @param argument: Not used
* @retval None
*/
extern uint8_t canFlag;

/* USER CODE END Header_pidThread */
__weak void pidThread(void const * argument)
{
  /* USER CODE BEGIN pidThread */
  /* Infinite loop */
  for(;;)
  {
  }
  /* USER CODE END pidThread */
}

/* USER CODE BEGIN Header_mainThread */
/**
* @brief Function implementing the MainTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_mainThread */
__weak void mainThread(void const * argument)
{
  /* USER CODE BEGIN mainThread */
  /* Infinite loop */
  for(;;)
  {

    osDelay(1000);
  }
  /* USER CODE END mainThread */
}

/* USER CODE BEGIN Header_minorThread */
/**
* @brief Function implementing the MinorTak thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_minorThread */
__weak void minorThread(void const * argument)
{
  /* USER CODE BEGIN minorThread */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END minorThread */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
