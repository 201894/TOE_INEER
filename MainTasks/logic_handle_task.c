/** @file logic_handle_task.h
 *  @version 1.0
 *  @date  DEC 2019
 *
 *  @brief ALL MOTORS PID HANDLE
 *
 */
 
#include "logic_handle_task.h"
#include "pid_handle_task.h"
#include "bsp_can.h"
#include "cmsis_os.h"
#include "bsp_can.h"
#include "bsp_io.h"
#include "pid.h"
#include "string.h"
#include "math.h"
#include "user_lib.h"
#include "STMGood.h"

#define LOGIC_TASK_PERIOD 5
#define THRESHOLD_300MS  300
#define THRESHOLD_500MS  500
#define THRESHOLD_1000MS 1000
#define THRESHOLD_2000MS 2000
#define SLIP_SPEED    		0.6f
#define SLIP_ERROR       5.0f
#define FLIP_ERROR       42.0f
#define THROW_FLAG    1
moto_ctrl_t moto_ctrl[3]; 
logic_data_t logic_data;

static uint8_t myDelay(uint16_t millisec)
{
	static uint16_t ticks;
	ticks++;
	if(ticks >=(uint16_t) (millisec/LOGIC_TASK_PERIOD)){
		ticks = 0;
		return 1;
	}
	else{
		return 0;
	}
}
// ȡ�����迨�� ���� �˻���һ����
static uint8_t task_stall_detect(uint8_t cnt,uint16_t THRESHOLD)
{
	 static uint16_t ticks;
	 static uint8_t now_cnt;
	 static uint8_t last_cnt;	
		now_cnt = cnt;	
		ticks++;	
		if(ticks >= (uint16_t)(THRESHOLD/LOGIC_TASK_PERIOD)){
			ticks = 0;		
			return 1;
		}
		if(now_cnt != last_cnt){
		 ticks = 0;
		}
		last_cnt = cnt;
		return 0;
}
uint8_t flip_to_max(float error)
{
	if (fabs(MAX_R_ANGLE - MotoData[LeftFlip].total_angle)<=error){
		return 1; 
	}
	else{ 
		return 0;				
	}	
}
static void BounceRotate_SelfLock(void)
{
	/*  ��צ��ת �Ƕ� �� ������ ����   �Ƕȳ�����ֵʱ ���׶��� ����ָ���ʼ״̬*/
		if( MotoData[LeftFlip].total_angle <= EVALVE_SELFLOCK_ANGLE){
				logic_data.bounceSelfLockFlag = GPIO_PIN_SET;
		}
		else{
				logic_data.bounceSelfLockFlag = GPIO_PIN_RESET;
		}
}

static void BounceEvalve_Ctrl(void)
{
	#if  !THROW_FLAG
		BounceRotate_SelfLock();
	#endif	
		HAL_GPIO_WritePin(EVALVE_GPIO_PORT	, BOUNCE_CTRL, logic_data.bounceFlag | logic_data.bounceSelfLockFlag);
}

static void LengthEvalve_Ctrl(void)
{
		HAL_GPIO_WritePin(EVALVE_GPIO_PORT	, LENGTH_CTRL, logic_data.lengthFlag);
}

static void ClampEvalve_Ctrl(void)
{
		
		HAL_GPIO_WritePin(EVALVE_GPIO_PORT	, CLAMP_CTRL, logic_data.clampFlag);
}

static void GlobalEvalve_Ctrl(void)
{
	BounceEvalve_Ctrl();
	LengthEvalve_Ctrl();
	ClampEvalve_Ctrl();
}

/*  ���ػ��Ƶ��λ��״̬ ��������Χ�ڷ���1 ���򷵻�0 */
static uint8_t position_accuracy(float error)
{
	pid_out[Slip].errNow = moto_ctrl[Slip].target - MotoData[MidSlip].total_angle;
	if (fabs(pid_out[Slip].errNow)<=error){
		return 1;
	}
	else{
		return 0;
	}
}
/*  ���ط�ת���λ��״̬ ��������Χ�ڷ���1 ���򷵻�0 */
static uint8_t angle_accuracy(float error)
{
	pid_out[Flip].errNow = moto_ctrl[Flip].target - MotoData[LeftFlip].total_angle;
	if (fabs(pid_out[Flip].errNow)<=error){
		return 1; 
	}
	else{ 
		return 0;				
	}
}			
																																					/* ���׵������� */
/* �н����� */
static void clamp_Action(void)
{
		 logic_data.clampFlag = GPIO_PIN_SET;
}

/*ж�ɶ���*/
static void loose_Action(void)
{
		 logic_data.clampFlag = GPIO_PIN_RESET;		
}

/*���� �쳤����*/
static void elongation_Action(void)
{
		 logic_data.lengthFlag = GPIO_PIN_SET;			
}

/* ���� ���̶���*/
static void shorten_Action(void)
{
	logic_data.lengthFlag = GPIO_PIN_RESET;		
}

/* ������ҩ�䶯��*/
static void bounce_Action(void)
{
	logic_data.bounceFlag = GPIO_PIN_SET;			
}

/* �������׶��� */
static void shrink_Action(void)
{
	logic_data.bounceFlag = GPIO_PIN_RESET;			
}
																																					/*  ��� ���� ����*/
/* ��ת����ֵ  */
static uint8_t flipToMaxAngle_Action(void)
{
		moto_ctrl[Flip].target = MAX_R_ANGLE;	
		if(angle_accuracy(1.0f)){
			return 1;
		}
		else{
			return 0;
		}
}
/* ��ת����ֵ  */
static uint8_t flipToBoAngle_Action(void)
{
		moto_ctrl[Flip].target = EVALVE_SELFLOCK_ANGLE - 10;	
		if(angle_accuracy(1.0f)){
			return 1;
		}
		else{
			return 0;
		}
}
/* ��ת����ֵ  */
static uint8_t flipToMidAngle_Action(void)
{
		moto_ctrl[Flip].target = MID_R_ANGLE;	
		if(angle_accuracy(1.0f)){
			return 1;
		}
		else{
			return 0;
		}
}

/* ��  ת����Сֵ*/
static uint8_t flipToMinAngle_Action(void)
{
		moto_ctrl[Flip].target = MIN_R_ANGLE;	
		if(angle_accuracy(30.0f)){
			return 1;
		}
		else{
			return 0;
		}
}
/* Flip ��ʼֵ  (�ϵ�λ��)*/
static uint8_t flipToInitPosition_Action(void)
{
		moto_ctrl[Flip].target = 0;	
		if(position_accuracy(1.0f)){
			return 1;
		}
		else{
			return 0;
		}
}
/* ������ ��ʼֵ  (�ϵ�λ��)*/
static uint8_t slipToInitPosition_Action(void)
{
		moto_ctrl[Slip].target = MID_S_POSITION;	
		if(position_accuracy(1.0f)){
			return 1;
		}
		else{
			return 0;
		}
}
/*  ������ ��ҩ�� */
static uint8_t slipToLeftPosition_Action(void)
{ 
		moto_ctrl[Slip].target = LEFT_S_POSITION + logic_data.CalibratedMidAngle;	
		if(position_accuracy(2.0f)){
			return 1;
		}
		else{
			return 0;
		}
}
/* ������ �м䵯ҩ��*/
static uint8_t slipToMidPosition_Action(void)
{
		moto_ctrl[Slip].target = MID_S_POSITION + logic_data.CalibratedMidAngle;	
		if(position_accuracy(2.0f)){
			return 1;
		}
		else{
			return 0;
		}
}

/* ������ �ҵ�ҩ��*/
static uint8_t slipToRightPosition_Action(void)
{
		moto_ctrl[Slip].target = RIGHT_S_POSITION + logic_data.CalibratedMidAngle;	
		if(position_accuracy(2.0f)){
			return 1;
		}
		else{
			return 0;
		}
}

/* ���� ��� ����  */
uint8_t slipDetect_Action(void)
{
  switch (logic_data.clawState)
	{		
		case ERROR_STATE:{
			
			return 0;			
		}break;
		
		case CORRECT_STATE:{
			moto_ctrl[Slip].target += 0;
			return 1;			
		}break;  
		
		case SHIFT_TO_LEFT:{
			moto_ctrl[Slip].target += SLIP_SPEED;
			return 0;			
		}break;  
		
		case SHIFT_TO_RIGHT:{
			moto_ctrl[Slip].target -= SLIP_SPEED;				// ����ƽ��
			return 0;
		}break;		
		
		case VV_SHIFT_TO_LEFT:{
			moto_ctrl[Slip].target += 0.5*SLIP_SPEED;				
			return 0;			
		}break;
		
		case VV_SHIFT_TO_RIGHT:{
			moto_ctrl[Slip].target -= 0.5*SLIP_SPEED;	
			return 0;			
		}break;		
	}
}
																																					/* ��϶��� ���+ ���*/
static uint8_t ToInitPosition_Action(void)
{
	slipToInitPosition_Action();
	flipToInitPosition_Action();	
}
																																					/* ��϶��� ���+����*/
/* ��ȡ��ҩ�� */
static uint8_t fetch_Action(void)
{
		moto_ctrl[Flip].target = MAX_R_ANGLE;
		if(angle_accuracy(FLIP_ERROR)){
				clamp_Action();
			 return 1;
		}
		else{
			return 0;
		}
}

/*  �ɿ� ��ҩ�� ������*/
static uint8_t  unload_Action(void)
{
		moto_ctrl[Flip].target = MIN_R_ANGLE;  
		if(angle_accuracy(1.0f)){			
			loose_Action();
			return 1;
		}			
		else{
			return 0;
		}
}
 /*�ӵ�ҩ��*/
static uint8_t throw_Action(void)
{
		moto_ctrl[Flip].target = MID_R_ANGLE;
		if(angle_accuracy(40.0f)){
			loose_Action();
			return 1;
		}
		else{
			return 0;
		}
}
 /*�ӵ�ҩ��*/
static uint8_t throwFar_Action(void)
{
		moto_ctrl[Flip].target = MAX_R_ANGLE;
		if(angle_accuracy(100.0f)){
			loose_Action();
			return 1;
		}
		else{
			return 0;
		}
}
static void logicDataReset(void)
{
		memset(&logic_data, 0, sizeof(logic_data_t));	
}

/*  ȡ���߼�  2  */
void fetch_ctrl(void)
{
	 switch (logic_data.task_cnt)
	 {		 
		 // ȡ��ģʽ�ж� 
		 case INIT_TASK:   // 0
		 {			 
			 if((slipToInitPosition_Action() == SET)&&flipToInitPosition_Action()){
				 logic_data.task_cnt++;
			 }
		 }break;
		 case SLIP_TASK1: // 1
		 {
				fetch_mode_handle1();
				if(task_stall_detect(logic_data.task_cnt,THRESHOLD_2000MS)){
					logic_data.task_cnt --;					 
				}				 				
		 }break;
		 // ��һ�䵯���ȡ �жϴ�����״̬ �ƶ�צ������ȷλ��
		 case DETECT_TASK:  // 2
		 {
				if(slipDetect_Action() == SET){
					switch (logic_data.fetch_mode)
					{
						case FOUR_BOX_FETCH:  // ����ȡ�� 4��
						{		
							logic_data.CalibratedMidAngle = MotoData[MidSlip].total_angle - MID_S_POSITION;
						}break;
						case THREE_BOX_FETCH:
						case TWO_BOX_FETCH:  // 
						{
							logic_data.CalibratedMidAngle = MotoData[MidSlip].total_angle - LEFT_S_POSITION;							
						}break;				
					}
					logic_data.task_cnt ++;				
				}					
//				 if(task_stall_detect(logic_data.task_cnt,THRESHOLD_2000MS)){
//					logic_data.task_cnt --;					 
//				 }
		 }break;
		 // ��ȡ��ҩ�� ���� 
		 case CLAMP_TASK:    //3
		 {
			  if (fetch_Action() == SET){   	
						if(angle_accuracy(1.0f)){
							logic_data.upLiftSelfLockFlag = 1;								
							logic_data.task_cnt++;		
						}
				}
//			 if(task_stall_detect(logic_data.task_cnt,THRESHOLD_2000MS)){
//				logic_data.task_cnt --;					 
//			 }
		 }break;							
		 case UPLIFT_TASK:  // 5
		 {
				if(logic_data.upLiftPosMaxFlag == SET){					
						logic_data.task_cnt ++;
				}
		 }break;		
		 // ��ת���ȡ��   �ӵ�ҩ�� / ����ҩ��
		 case UPLOAD_TASK:  // 6
		 {
				if(flipToMinAngle_Action() == SET){									
//						logic_data.task_cnt ++;
				}
//				if(flip_to_max(90.0f) == RESET){
//					logic_data.upLiftSelfLockFlag = 0;
//				}
			 if(task_stall_detect(logic_data.task_cnt,THRESHOLD_2000MS)){
				logic_data.task_cnt --;					 
			 }
		 }break;
		 case SLIP_TASK2: // 7
		 {
				fetch_mode_handle2();
			  if(task_stall_detect(logic_data.task_cnt,THRESHOLD_2000MS)){
					logic_data.task_cnt --;					 
			  }
		 }break;
		 // ����ҩ���������
		 case THROW_TASK:   // 8
		 {
			 if(!logic_data.quitFlag){
					#if THROW_FLAG
						if(throwFar_Action() == SET){
								logic_data.task_cnt = CLAMP_TASK;	// ����������
						}
					#else
						loose_Action();
						logic_data.task_cnt = CLAMP_TASK;	// ����������				
				 #endif
				}
			 else{
					#if THROW_FLAG
						if(throw_Action() == SET){
								logic_data.task_cnt ++;	// ����������
						}
					#else
						loose_Action();
						shorten_Action();						
						if(flipToBoAngle_Action()==SET){
							logic_data.task_cnt ++;	// ����������				
						}
				 #endif
			 }
			 if(task_stall_detect(logic_data.task_cnt,THRESHOLD_2000MS)){
				logic_data.task_cnt --;					 
			 }
		 }break; 
		 // ��־λ���� ������־λ��SETλ
		 case QUIT_TASK:   // 9
		 {
//			 	logicDataReset();
				logic_data.endFlag = 1;				
		 }break;
		 
		 default:
		 {				
			ToInitPosition_Action();		 
		 }break;		 
	 }	 
}

void fetch_mode_handle2(void)
{
			switch (logic_data.fetch_mode)
			{
				case FOUR_BOX_FETCH:  // ����ȡ�� 4��
				{
					 if(logic_data.loop_cnt == 1){
						elongation_Action();   //   ***  ���ȡ������ ȡ�ڶ���
						if(slipToLeftPosition_Action() ==SET){  // ���Ƶ�� ����
							if(myDelay(100) == SET){
								logic_data.loop_cnt ++;
								logic_data.task_cnt  ++;				// ȡ��һ��֮�󲻼�����ʿ״̬	
							}
						}
					}
					else if(logic_data.loop_cnt == 2){						
						if(slipToMidPosition_Action() == SET){  // ���Ƶ��  
								logic_data.loop_cnt ++;													
								logic_data.task_cnt ++;				// ȡ��һ��֮�󲻼�����ʿ״̬

						}	
					}
					else if(logic_data.loop_cnt == 3){
						if(slipToMidPosition_Action() == SET){  // ���Ƶ�� ����
								logic_data.loop_cnt ++;
								logic_data.task_cnt ++;			// ȡ��һ��֮�󲻼�����ʿ״̬			
						  	logic_data.quitFlag = 1;							
						}									
					}	

				}break;	
				case TWO_BOX_FETCH:  // ����4����  ��һ�� 2��
				{
					 if(logic_data.loop_cnt == 1){
							if(slipToRightPosition_Action() ==SET){  // ���Ƶ�� ����
								if(myDelay(10) == SET){
									logic_data.loop_cnt ++;								
									logic_data.task_cnt = CLAMP_TASK;				// ȡ��һ��֮�󲻼�����ʿ״̬	
								}							
							}						
						} 	
						else{
							if(slipToInitPosition_Action() == SET){  // ���Ƶ�� 	����ʼλ��		
									logic_data.task_cnt = QUIT_TASK;					
							}	
						}		
				}break;		
				case THREE_BOX_FETCH:   // С��Դ�� ��ҩ ����
				{							
					  if(logic_data.loop_cnt == 1){
							if(slipToMidPosition_Action() ==SET){  // ���Ƶ�� ����
								if(myDelay(10) == SET){
									logic_data.loop_cnt ++;								
									logic_data.task_cnt = CLAMP_TASK;				// ȡ��һ��֮�󲻼�����ʿ״̬	
								}							
							}
						}
						else if(logic_data.loop_cnt == 2){
							if(slipToRightPosition_Action() ==SET){  // ���Ƶ�� ����
								if(myDelay(10) == SET){
									logic_data.loop_cnt ++;								
									logic_data.task_cnt = CLAMP_TASK;				// ȡ��һ��֮�󲻼�����ʿ״̬	
								}							
							}						
						}							
						else{
							if(slipToInitPosition_Action() == SET){  // ���Ƶ�� 	����ʼλ��		
									logic_data.task_cnt = QUIT_TASK;					
							}							
						}						
				}break;				
			}				

}
void fetch_mode_handle1(void)
{
		if(logic_data.upLiftPosFlag){
			switch (logic_data.fetch_mode)
			{
				case NULL_FETCH:
				{
					ToInitPosition_Action();				
				}break;
				case FOUR_BOX_FETCH:  // ����ȡ�� 4��
				{
						flipToMidAngle_Action();   //  ��ת��� ����ɨ��״̬
						if(slipToMidPosition_Action() == SET){  // ���Ƶ�� 
								logic_data.loop_cnt ++;
								logic_data.task_cnt ++;
						}
				}break;		
				case TWO_BOX_FETCH:  // ����4����  ��һ�� 2��
				{
					if(logic_data.endFlag == RESET){					
						flipToMidAngle_Action();   //  ��ת��� ����ɨ��״̬
						if(logic_data.loop_cnt == 0){
							if(slipToLeftPosition_Action() == SET){  // ���Ƶ�� 
									if(myDelay(10) == SET){
										logic_data.loop_cnt ++;														
										logic_data.task_cnt ++;
								}
							}
						}
						else if(logic_data.loop_cnt == 1){
							if(slipToRightPosition_Action() ==SET){  // ���Ƶ�� ����
								if(myDelay(10) == SET){
									logic_data.loop_cnt ++;								
									logic_data.task_cnt = CLAMP_TASK;				// ȡ��һ��֮�󲻼�����ʿ״̬	
								}							
							}						
						} 	
						else{
							if(slipToInitPosition_Action() == SET){  // ���Ƶ�� 	����ʼλ��		
									logic_data.task_cnt = QUIT_TASK;					
							}	
						}		
					}
					else{
						ToInitPosition_Action();					
					}
				}break;	
				case THREE_BOX_FETCH:   // С��Դ�� ��ҩ ����
				{
					if(logic_data.endFlag == RESET){					
						flipToMidAngle_Action();   //  ��ת��� ����ɨ��״̬
						if(logic_data.loop_cnt == 0){
							if(slipToLeftPosition_Action() == SET){  // ���Ƶ�� 
									if(myDelay(10) == SET){
									logic_data.loop_cnt ++;														
										logic_data.task_cnt = CLAMP_TASK;
								}
							}						
						} 							
						else if(logic_data.loop_cnt == 1){
							if(slipToMidPosition_Action() ==SET){  // ���Ƶ�� ����
								if(myDelay(10) == SET){
									logic_data.loop_cnt ++;								
									logic_data.task_cnt = CLAMP_TASK;				// ȡ��һ��֮�󲻼�����ʿ״̬	
								}							
							}						
						}
						else if(logic_data.loop_cnt == 2){
							if(slipToRightPosition_Action() ==SET){  // ���Ƶ�� ����
								if(myDelay(10) == SET){
									logic_data.loop_cnt ++;
									logic_data.task_cnt = CLAMP_TASK;				// ȡ��һ��֮�󲻼�����ʿ״̬	
								}
							}
						}
						else{
							if(slipToInitPosition_Action() == SET){  // ���Ƶ�� 	����ʼλ��		
									logic_data.task_cnt = QUIT_TASK;					
							}
						}
					}
					else{
						ToInitPosition_Action();					
					}						
				}break;	
				default:
				{
					ToInitPosition_Action();		 
				}break;		 				
			}
			logic_data.last_fetch_mode = logic_data.fetch_mode;
	  }	
}

void mainThread(void const * argument)
{
  /* USER CODE BEGIN logic_handle_task */
	portTickType MainThreadLastWakeTime;
  /* Infinite loop */
  for(;;)
  {
		MainThreadLastWakeTime = xTaskGetTickCount();		
		taskENTER_CRITICAL();
		/*GNS MX REVELENT*/
		GlobalEvalve_Ctrl();
    if(logic_data.fetch_mode != NULL_FETCH){
			if(logic_data.endFlag == RESET){
				fetch_ctrl();
			}
			else{
				ToInitPosition_Action();					
			}
		}
		else{
					logicDataReset();
					ToInitPosition_Action();			
		}
		send_can_ms(CAN_SEND_M1_ID,(int16_t)moto_ctrl[Slip].target, \
		 (int16_t)moto_ctrl[Flip].target,logic_data.upLiftSelfLockFlag);

		taskEXIT_CRITICAL();
    osDelayUntil(&MainThreadLastWakeTime,LOGIC_TASK_PERIOD);			
  }
  /* USER CODE END pid_handle_task */
}

