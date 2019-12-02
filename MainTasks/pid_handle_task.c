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

#define     PID_TASK_PERIOD 5
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
		
		
		/* ̧����� �� ��ת��� ���Ʋ��� ��		
		    ʹ��һ��������⻷���ֵ����Ϊ�ڻ����Ƶ�Ŀ��ֵ
		*/
		
		/* ̧�����PID����    �⻷   ����Ŀ��λ��           --        �������λ��  */
		pid_ast(&pid_out[UpLift],moto_ctrl[UpLift].target,MotoParam[LeftUpLift].total_angle); 
		/* ̧�����PID����    �ڻ�   �⻷�����             --        ��������ٶ�  */
		pid_ast(&pid_in[LeftUpLift],pid_out[UpLift].ctrOut,MotoParam[LeftUpLift].speed_rpm); 
		pid_ast(&pid_in[RightUpLift],-pid_out[UpLift].ctrOut,MotoParam[RightUpLift].speed_rpm);
		/* ��ת���PID����    �⻷   ����Ŀ��λ��           --        �������λ��  */
		pid_ast(&pid_out[Flip],moto_ctrl[Flip].target,MotoParam[LeftFlip].total_angle);
		/* ��ת���PID����    �ڻ�   �⻷�����             --        ��������ٶ�  */		
		pid_ast(&pid_in[LeftFlip],pid_out[Flip].ctrOut,MotoParam[LeftFlip].speed_rpm); 
		pid_ast(&pid_in[RightFlip],-pid_out[Flip].ctrOut,MotoParam[RightFlip].speed_rpm);
		/* ���Ƶ��PID����    �⻷   ����Ŀ��λ��           --        �������λ��  */
		pid_ast(&pid_out[Slip],moto_ctrl[Slip].target,MotoParam[MidSlip].total_angle); 
		/* ���Ƶ��PID����    �ڻ�   �⻷�����             --        �������λ��  */	
		pid_ast(&pid_in[MidSlip],pid_out[Slip].ctrOut,MotoParam[MidSlip].speed_rpm); 
		/* ����� ��� �ջ� ����ֵ ����   ̧����� 201 202 ��ת��� 203 204 */		
		send_ctrl_cur(0x200,(int16_t)pid_in[LeftUpLift].ctrOut,\
		                    (int16_t)pid_in[RightUpLift].ctrOut,\
		                    (int16_t)pid_in[LeftFlip].ctrOut,\
												(int16_t)pid_in[RightFlip].ctrOut);
		/* ����� ��� �ջ� ����ֵ ����   ���Ƶ�� 205*/		
		send_ctrl_cur(0x1ff,(int16_t)pid_in[MidSlip].ctrOut,	0,0,0);	
	
		taskEXIT_CRITICAL();
    osDelayUntil(&PidHandleLastWakeTime,PID_TASK_PERIOD);			
  }
  /* USER CODE END pid_handle_task */
}
 
 
void pid_param_init(void)
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
