/** @file pid_handle_task.h
 *  @version 1.0
 *  @date  DEC 2019
 *
 *  @brief ALL MOTORS PID HANDLE
 *
 */
 
#include "pid_handle_task.h"
#include "logic_handle_task.h"
#include "cmsis_os.h"
#include "bsp_can.h"
#include "bsp_io.h"
#include "pid.h"
#include "STMGood.h"

#define     PID_TASK_PERIOD 		5
//#define     LeftUpLift       0
//#define     RightUpLift      1
//#define     LeftFlip         2
//#define     RightFlip       3
//#define     MidSlip          4
//#define    UpLift    0
//#define    Flip      1
//#define    Slip      2
void pid_handle_task(void const * argument)
{
  /* USER CODE BEGIN pid_handle_task */
	portTickType PidHandleLastWakeTime;
  /* Infinite loop */
  for(;;)
  {
		PidHandleLastWakeTime = xTaskGetTickCount();		
		taskENTER_CRITICAL();
	#if 1
	    pid_adjust(&pid_out[UpLift],_kp,_ki,_kd);
	    pid_adjust(&pid_in[LeftUpLift],_kkp,_kki,_kkd);
	    pid_adjust(&pid_in[RightUpLift],_kkp,_kki,_kkd);	  
	    pid_out[UpLift].MaxOut = maxout1;
	    pid_in[LeftUpLift].MaxOut = maxout2;			
			pid_in[RightUpLift].MaxOut = maxout2;		
	#endif
	#if 0
	    pid_adjust(&pid_out[Flip],_kp,_ki,_kd);
	    pid_adjust(&pid_in[LeftFlip],_kkp,_kki,_kkd);
	    pid_adjust(&pid_in[RightFlip],_kkp,_kki,_kkd);	  
	    pid_out[Flip].MaxOut = maxout1;
	    pid_in[LeftFlip].MaxOut = maxout2;			
			pid_in[RightFlip].MaxOut = maxout2;		
	#endif	
	#if 0
	    pid_adjust(&pid_out[Slip],_kp,_ki,_kd);
	    pid_adjust(&pid_in[MidSlip],_kkp,_kki,_kkd);
	    pid_out[Slip].MaxOut = maxout1;
	    pid_in[MidSlip].MaxOut = maxout2;			
	#endif			
		/* 抬升电机 及 翻转电机 控制策略 ：		
		    使用一个电机的外环输出值，作为内环控制的目标值
		*/
		
		/* 抬升电机PID控制    外环   理论目标位置           --        电机反馈位置  */
		pid_ast(&pid_out[UpLift],moto_ctrl[UpLift].target,MotoData[LeftUpLift].total_angle); 
		/* 抬升电机PID控制    内环   外环输出量             --        电机反馈速度  */
		pid_ast(&pid_in[LeftUpLift],pid_out[UpLift].ctrOut,MotoData[LeftUpLift].speed_rpm); 
		pid_ast(&pid_in[RightUpLift],-pid_out[UpLift].ctrOut,MotoData[RightUpLift].speed_rpm);
		/* 翻转电机PID控制    外环   理论目标位置           --        电机反馈位置  */
		pid_ast(&pid_out[Flip],moto_ctrl[Flip].target,MotoData[LeftFlip].total_angle);
		/* 翻转电机PID控制    内环   外环输出量             --        电机反馈速度  */		
		pid_ast(&pid_in[LeftFlip],pid_out[Flip].ctrOut,MotoData[LeftFlip].speed_rpm); 
		pid_ast(&pid_in[RightFlip],-pid_out[Flip].ctrOut,MotoData[RightFlip].speed_rpm);
		/* 滑移电机PID控制    外环   理论目标位置           --        电机反馈位置  */
		pid_ast(&pid_out[Slip],moto_ctrl[Slip].target,MotoData[MidSlip].total_angle); 
		/* 滑移电机PID控制    内环   外环输出量             --        电机反馈位置  */	
		pid_ast(&pid_in[MidSlip],pid_out[Slip].ctrOut,MotoData[MidSlip].speed_rpm); 
		/* 计算得 电机 闭环 电流值 发送    翻转电机 201 202    滑移电机 203 */		
		send_can1_cur(0x200,(int16_t)pid_in[LeftFlip].ctrOut,\
												(int16_t)pid_in[RightFlip].ctrOut,\
												(int16_t)pid_in[MidSlip].ctrOut,\
												 0 );
		/* 计算得 电机 闭环 电流值 发送  抬升电机 207 208*/		
		send_can2_cur(0x1ff, 0, 0, (int16_t)pid_in[RightUpLift].ctrOut, (int16_t)pid_in[LeftUpLift].ctrOut);	
	
		taskEXIT_CRITICAL();
    osDelayUntil(&PidHandleLastWakeTime,PID_TASK_PERIOD);			
  }
  /* USER CODE END pid_handle_task */
}
 
 
void PID_InitArgument(void)
{
	                                   /* errILim  maxOut  kp ki kd  */
  PID_struct_init(&pid_out[UpLift],       	0,      0,  0, 0, 0);
  PID_struct_init(&pid_out[Flip],       		0,      0,  0, 0, 0); 
  PID_struct_init(&pid_out[Slip],       	  0,      0,  0, 0, 0);	
	
  PID_struct_init(&pid_in[LeftUpLift],      	0,      0,  0, 0, 0); 
  PID_struct_init(&pid_in[RightUpLift],       0,      0,  0, 0, 0);
  PID_struct_init(&pid_in[LeftFlip],       		0,      0,  0, 0, 0);
  PID_struct_init(&pid_in[RightFlip],       	0,      0,  0, 0, 0);
  PID_struct_init(&pid_in[MidSlip],       		0,      0,  0, 0, 0); 	
}
