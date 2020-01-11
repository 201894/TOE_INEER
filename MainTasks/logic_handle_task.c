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
#include "STMGood.h"

#define LOGIC_TASK_PERIOD 5
#define SLIP_SPEED    		0.1f
#define THROW_FLAG    1
#define EXTRA_UP_POS    60.0f
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
static void BounceRotate_SelfLock(void)
{
	/*  ��צ��ת �Ƕ� �� ������ ����   �Ƕȳ�����ֵʱ ���׶��� ����ָ���ʼ״̬*/
		if( MotoData[LeftFlip].total_angle >= EVALVE_SELFLOCK_ANGLE){
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
		if(angle_accuracy(5.0f)){
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
		if(position_accuracy(1.0f)){
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
		if(position_accuracy(1.0f)){
			return 1;
		}
		else{
			return 0;
		}
}

/* ���� ��� ����  */
static uint8_t slipDetect_Action(void)
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
			moto_ctrl[Slip].target -= SLIP_SPEED;				
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
		if(angle_accuracy(1.0f)){
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

 /*��Զ ��ҩ��*/
static uint8_t throwFar_Action(void)
{
		moto_ctrl[Flip].target = MAX_R_ANGLE;
		if(angle_accuracy(20.0f)){
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
/*  ȡ���߼�  1   ��һ�� �������� ���� �ָ���ԭʼλ�� */
void first_row_fetch(void)
{
	
	 switch (logic_data.task_cnt)
	 {
		 case 0 :
		 {
			 	flipToMidAngle_Action();   //  ��ת��� ����ɨ��״̬
			  if(! logic_data.firstBoxFlag){					// �ж��Ƿ��ǵ�һ�䵯��   �ǵ�һ���������� ��������    
					if(slipToLeftPosition_Action()){  // ���Ƶ�� ����
							logic_data.firstBoxFlag = 1;  // ��ҩ��������־λ��1 							
							logic_data.task_cnt++;
					}
			  }
				else{
					if(slipToRightPosition_Action()){  // ���Ƶ�� ����
							logic_data.task_cnt++;
					}
				}
		 }break;
		 
		 case 1:
		 {
			 // ���� ���������Ƿ���Ҫ ɨ��
				if(slipDetect_Action()){        //  ����ʿ -���Ƶ��  ��� ģʽ
						logic_data.CalibratedMidAngle =  MotoData[MidSlip].total_angle - LEFT_S_POSITION; // ��¼��У׼��ĵ��λ����Ϣ
						logic_data.task_cnt++;
				}			
		 }break;
		 
		 case 2 :
		 {
			  if (fetch_Action()&&angle_accuracy(0.5f)){   // ��ȡ��ҩ�� ���� ��� ��ת������� 
						logic_data.task_cnt++;					
				}
		 }break;
		 
		 case 3:
		 {
				if(unload_Action()){     //  ж�� ��ҩ��
						logic_data.task_cnt = 0;	// ����������
				}
		 }break;
		 
		 default:
		 {
				
		 }break;		 
	 }	
	 
}

/*  ȡ���߼�  2  */
void second_row_fetch(void)
{
		logic_data.extraUpPos =500;
	 switch (logic_data.task_cnt)
	 {
		 case 0 :
		 {
			 	flipToMidAngle_Action();   //  ��ת��� ����ɨ��״̬
				if(logic_data.loop_cnt == 0){
					if(slipToMidPosition_Action() == 1){  // ���Ƶ�� 
							if(myDelay(10) == 1){
							logic_data.loop_cnt ++;														
								logic_data.task_cnt = 2;
																		}
					}						
				} 							
				else if(logic_data.loop_cnt == 1){
					elongation_Action();   //   ***  ���ȡ������ ȡ�ڶ���
					if(slipToMidPosition_Action() ==1){  // ���Ƶ�� ����
						if(myDelay(500)){
							logic_data.loop_cnt ++;								
						  logic_data.task_cnt = 2;				// ȡ��һ��֮�󲻼�����ʿ״̬	
						}							
					}						
				} 
//				else if(logic_data.loop_cnt == 2){
//					if(slipToMidPosition_Action() == 1){  // ���Ƶ��  
//						if(myDelay(10) == 1){
//							logic_data.loop_cnt ++;													
//			      	logic_data.task_cnt = 2;				// ȡ��һ��֮�󲻼�����ʿ״̬		
//					}													
//					}	
//				}					
//				else if(logic_data.loop_cnt == 3){
//					if(slipToRightPosition_Action() == 1){  // ���Ƶ�� ����
//								if(myDelay(10) == 1){
//							logic_data.loop_cnt ++;
//							logic_data.task_cnt = 2;				// ȡ��һ��֮�󲻼�����ʿ״̬				
//								}									
//					}
//				}
				else{
					if(slipToInitPosition_Action() == 1){  // ���Ƶ�� 	����ʼλ��		
							logic_data.task_cnt = 5;					
					}	
				}				
		 }break;
		 
		 case 1:
		 {
			 // ���� ���������Ƿ���Ҫ ɨ��
				if(slipDetect_Action() == 1){        //  ����ʿ -���Ƶ��  ��� ģʽ
					if(logic_data.loop_cnt == 1){
						logic_data.CalibratedMidAngle =  MotoData[MidSlip].total_angle - MID_S_POSITION; // ��¼��У׼��ĵ��λ����Ϣ
					}
						logic_data.boxNumber ++;				// ��ʵȡ����		
						logic_data.task_cnt++;
				}	
		 }break;
		 
		 case 2 :
		 {
			  if (fetch_Action()){   // ��ȡ��ҩ�� ���� ��� ��ת���
						if(myDelay(150) == 1){						
							logic_data.task_cnt++;		
						}
				}
		 }break; 
		 
		 case 3:
		 {
			#if  THROW_FLAG			 
					if(flipToMinAngle_Action() == 1){
						if(myDelay(150) == 1){							
							logic_data.task_cnt ++;
						}
					}										 
			#else
					if(unload_Action()){     //  ж�� ��ҩ��
						logic_data.task_cnt = 0;	// ����������
					}
			#endif	
				
		 }break;
		 case 4:
		 {
				if(throw_Action() == 1){
						logic_data.task_cnt = 0;	// ����������
				}	
		 }break;
		 case 5:
		 {
			 	logicDataReset();
				logic_data.endFlag = 1;				
		 }break;
		 default:
		 {
				
		 }break;		 
	 }	
	 
}

void fetch_mode_handle(void)
{
		if(logic_data.upLiftPosFlag){
			switch (logic_data.fetch_mode)
			{
				case 0:
				{
					logicDataReset();
				  ToInitPosition_Action();
				}break;
				case 1:
				{					
					if(!logic_data.endFlag){
						second_row_fetch();	
					}
					else{
						ToInitPosition_Action();
					}		
				}break;		
				case 2:
				{
					
				}break;	
				case 3:
				{
					
				}break;		
				 default:
				 {
						
				 }break;		
			}
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
		fetch_mode_handle();
		send_can_ms(CAN_SEND_M1_ID,(int16_t)(int16_t)moto_ctrl[Slip].target, \
		 (int16_t)moto_ctrl[Flip].target,0);

		taskEXIT_CRITICAL();
    osDelayUntil(&MainThreadLastWakeTime,LOGIC_TASK_PERIOD);			
  }
  /* USER CODE END pid_handle_task */
}

