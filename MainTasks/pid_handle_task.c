/** @file pid_handle_task.h
 *  @version 1.0
 *  @date  DEC 2019
 *
 *  @brief ALL MOTORS PID HANDLE
 *
 */
 
#include "pid_handle_task.h"
#include "logic_handle_task.h"
#include "minorThread.h"
#include "cmsis_os.h"
#include "bsp_can.h"
#include "bsp_io.h"
#include "pid.h"
#include "STMGood.h"

#define     PID_TASK_PERIOD 		5
#define     SPD_RATIO   0.2f
/*���� ��� PID ����*/
#define  SLIP_MAX_SPD                  	500 // 
#define  SLIP_ANG_ERRORLLIM     	3000 // 
#define  SLIP_MAX_CURRENT        		15000 // 
#define  SLIP_SPD_ERRORLLIM       	6000 // 
#define  SLIP_ANG_KP                    	30.0f // 
#define  SLIP_SPD_KP                    		40.0f // 
#define  SLIP_SPD_KI                      	0.0f // 
#define  SLIP_SPD_KD                    		0.0f // 
/*��ת ��� PID ����*/
#define  FLIP_MAX_SPD                  	500 // 
#define  FLIP_ANG_ERRORLLIM     	3000 // 
#define  FLIP_MAX_CURRENT        		15000 // 
#define  FLIP_SPD_ERRORLLIM       	6000 // 
#define  FLIP_ANG_KP                    	30.0f // 
#define  FLIP_SPD_KP                    		40.0f // 
#define  FLIP_SPD_KI                      	0.0f // 
#define  FLIP_SPD_KD                    		0.0f // 
//#define     LeftUpLift       0
//#define     RightUpLift      1
//#define     LeftFlip         2
//#define     RightFlip       3
//#define     MidSlip          4
//#define    UpLift    0
//#define    Flip      1
//#define    Slip      2
extern uint16_t OLED_ADC_flag;
void pidThread(void const * argument)
{
  /* USER CODE BEGIN pid_handle_task */
	portTickType PidHandleLastWakeTime;
  /* Infinite loop */
  for(;;)
  {
		PidHandleLastWakeTime = xTaskGetTickCount();		
		taskENTER_CRITICAL();
	#if 0
	    pid_adjust(&pid_out[Flip],_kp,_ki,_kd);
	    pid_adjust(&pid_in[LeftFlip],_kkp,_kki,_kkd);
	    pid_adjust(&pid_in[RightFlip],_kkp,_kki,_kkd);	  
	    pid_out[Flip].MaxOut = maxout1;
	    pid_in[LeftFlip].MaxOut = maxout2;			
			pid_in[RightFlip].MaxOut = maxout2;		
	#endif	
	#if 1
	    pid_adjust(&pid_out[Slip],30,0,0);
	    pid_adjust(&pid_in[MidSlip],40,0,0);
	    pid_out[Slip].MaxOut = 400;
	    pid_in[MidSlip].MaxOut = 15000;			
	#endif			
		/* ̧����� �� ��ת��� ���Ʋ��� ��		
		    ʹ��һ��������⻷���ֵ����Ϊ�ڻ����Ƶ�Ŀ��ֵ
		*/
		
		/* ��ת���PID����    �⻷   ����Ŀ��λ��           --        �������λ��  */
		pid_ast(&pid_out[Flip],moto_ctrl[Flip].target,MotoData[LeftFlip].total_angle);
		/* ��ת���PID����    �ڻ�   �⻷�����             --        ��������ٶ�  */		
		pid_ast(&pid_in[LeftFlip],pid_out[Flip].ctrOut,MotoData[LeftFlip].speed_rpm * SPD_RATIO); 
		pid_ast(&pid_in[RightFlip],-pid_out[Flip].ctrOut,MotoData[RightFlip].speed_rpm * SPD_RATIO);
		/* ���Ƶ��PID����    �⻷   ����Ŀ��λ��           --        �������λ��  */
		pid_ast(&pid_out[Slip],moto_ctrl[Slip].target,MotoData[MidSlip].total_angle); 
		/* ���Ƶ��PID����    �ڻ�   �⻷�����             --        �������λ��  */	
		pid_ast(&pid_in[MidSlip],pid_out[Slip].ctrOut,MotoData[MidSlip].speed_rpm * SPD_RATIO); 
		if(r_fps[MidSlip].fps >= MOTO_FPS_THRESHOLD)
		{
		/* ����� ��� �ջ� ����ֵ ����    ��ת��� 201 202    ���Ƶ�� 203 */		
				CAN1_Send_Current(0x200,(int16_t)pid_in[LeftFlip].ctrOut,\
														(int16_t)pid_in[RightFlip].ctrOut,\
														(int16_t)pid_in[MidSlip].ctrOut,\
														 0 );
		}
		taskEXIT_CRITICAL();
    osDelayUntil(&PidHandleLastWakeTime,PID_TASK_PERIOD);			
  }
  /* USER CODE END pid_handle_task */
}
 
 
void PID_InitArgument(void)
{
 /* errILim  maxOut  kp ki kd  */
  PID_struct_init(&pid_out[Flip], 0, FLIP_MAX_SPD, FLIP_ANG_KP, 0, 0); 
  PID_struct_init(&pid_out[Slip], 0, SLIP_MAX_SPD, SLIP_ANG_KP, 0, 0);	
  PID_struct_init(&pid_in[LeftFlip],     FLIP_SPD_ERRORLLIM, FLIP_MAX_CURRENT, FLIP_SPD_KP, FLIP_SPD_KI, FLIP_SPD_KD);
  PID_struct_init(&pid_in[RightFlip],   FLIP_SPD_ERRORLLIM, FLIP_MAX_CURRENT, FLIP_SPD_KP, FLIP_SPD_KI, FLIP_SPD_KD);
  PID_struct_init(&pid_in[MidSlip],     SLIP_SPD_ERRORLLIM, SLIP_MAX_CURRENT, SLIP_SPD_KP, SLIP_SPD_KI, SLIP_SPD_KD); 	
}
