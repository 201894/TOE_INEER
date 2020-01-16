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
// 取弹步骤卡顿 处理 退回上一步骤
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
	/*  夹爪翻转 角度 与 弹气缸 自锁   角度超过中值时 气缸动作 否则恢复初始状态*/
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

/*  返回滑移电机位置状态 精度在误差范围内返回1 否则返回0 */
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
/*  返回翻转电机位置状态 精度在误差范围内返回1 否则返回0 */
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
																																					/* 气缸单独动作 */
/* 夹紧动作 */
static void clamp_Action(void)
{
		 logic_data.clampFlag = GPIO_PIN_SET;
}

/*卸荷动作*/
static void loose_Action(void)
{
		 logic_data.clampFlag = GPIO_PIN_RESET;		
}

/*机构 伸长动作*/
static void elongation_Action(void)
{
		 logic_data.lengthFlag = GPIO_PIN_SET;			
}

/* 机构 缩短动作*/
static void shorten_Action(void)
{
	logic_data.lengthFlag = GPIO_PIN_RESET;		
}

/* 弹开弹药箱动作*/
static void bounce_Action(void)
{
	logic_data.bounceFlag = GPIO_PIN_SET;			
}

/* 收缩气缸动作 */
static void shrink_Action(void)
{
	logic_data.bounceFlag = GPIO_PIN_RESET;			
}
																																					/*  电机 单独 动作*/
/* 翻转至中值  */
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
/* 翻转至中值  */
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
/* 翻转至中值  */
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

/* 翻  转至最小值*/
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
/* Flip 初始值  (上电位置)*/
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
/* 滑移至 初始值  (上电位置)*/
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
/*  滑移至 左弹药箱 */
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
/* 滑移至 中间弹药箱*/
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

/* 滑移至 右弹药箱*/
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

/* 滑移 检测 动作  */
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
			moto_ctrl[Slip].target -= SLIP_SPEED;				// 向左平移
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
																																					/* 组合动作 电机+ 电机*/
static uint8_t ToInitPosition_Action(void)
{
	slipToInitPosition_Action();
	flipToInitPosition_Action();	
}
																																					/* 组合动作 电机+气缸*/
/* 夹取弹药箱 */
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

/*  松开 弹药箱 至弹仓*/
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
 /*扔弹药箱*/
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
 /*扔弹药箱*/
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

/*  取弹逻辑  2  */
void fetch_ctrl(void)
{
	 switch (logic_data.task_cnt)
	 {		 
		 // 取弹模式判断 
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
		 // 第一箱弹丸获取 判断传感器状态 移动爪子至正确位置
		 case DETECT_TASK:  // 2
		 {
				if(slipDetect_Action() == SET){
					switch (logic_data.fetch_mode)
					{
						case FOUR_BOX_FETCH:  // 开局取弹 4箱
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
		 // 夹取弹药箱 动作 
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
		 // 翻转电机取弹   扔弹药箱 / 弹弹药箱
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
		 // 弹弹药箱后续动作
		 case THROW_TASK:   // 8
		 {
			 if(!logic_data.quitFlag){
					#if THROW_FLAG
						if(throwFar_Action() == SET){
								logic_data.task_cnt = CLAMP_TASK;	// 任务步骤重置
						}
					#else
						loose_Action();
						logic_data.task_cnt = CLAMP_TASK;	// 任务步骤重置				
				 #endif
				}
			 else{
					#if THROW_FLAG
						if(throw_Action() == SET){
								logic_data.task_cnt ++;	// 任务步骤重置
						}
					#else
						loose_Action();
						shorten_Action();						
						if(flipToBoAngle_Action()==SET){
							logic_data.task_cnt ++;	// 任务步骤重置				
						}
				 #endif
			 }
			 if(task_stall_detect(logic_data.task_cnt,THRESHOLD_2000MS)){
				logic_data.task_cnt --;					 
			 }
		 }break; 
		 // 标志位清零 自锁标志位置SET位
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
				case FOUR_BOX_FETCH:  // 开局取弹 4箱
				{
					 if(logic_data.loop_cnt == 1){
						elongation_Action();   //   ***  伸出取弹机构 取第二排
						if(slipToLeftPosition_Action() ==SET){  // 滑移电机 右移
							if(myDelay(100) == SET){
								logic_data.loop_cnt ++;
								logic_data.task_cnt  ++;				// 取第一箱之后不检测基恩士状态	
							}
						}
					}
					else if(logic_data.loop_cnt == 2){						
						if(slipToMidPosition_Action() == SET){  // 滑移电机  
								logic_data.loop_cnt ++;													
								logic_data.task_cnt ++;				// 取第一箱之后不检测基恩士状态

						}	
					}
					else if(logic_data.loop_cnt == 3){
						if(slipToMidPosition_Action() == SET){  // 滑移电机 右移
								logic_data.loop_cnt ++;
								logic_data.task_cnt ++;			// 取第一箱之后不检测基恩士状态			
						  	logic_data.quitFlag = 1;							
						}									
					}	

				}break;	
				case TWO_BOX_FETCH:  // 开局4分钟  第一排 2箱
				{
					 if(logic_data.loop_cnt == 1){
							if(slipToRightPosition_Action() ==SET){  // 滑移电机 右移
								if(myDelay(10) == SET){
									logic_data.loop_cnt ++;								
									logic_data.task_cnt = CLAMP_TASK;				// 取第一箱之后不检测基恩士状态	
								}							
							}						
						} 	
						else{
							if(slipToInitPosition_Action() == SET){  // 滑移电机 	至初始位置		
									logic_data.task_cnt = QUIT_TASK;					
							}	
						}		
				}break;		
				case THREE_BOX_FETCH:   // 小资源岛 弹药 三箱
				{							
					  if(logic_data.loop_cnt == 1){
							if(slipToMidPosition_Action() ==SET){  // 滑移电机 右移
								if(myDelay(10) == SET){
									logic_data.loop_cnt ++;								
									logic_data.task_cnt = CLAMP_TASK;				// 取第一箱之后不检测基恩士状态	
								}							
							}
						}
						else if(logic_data.loop_cnt == 2){
							if(slipToRightPosition_Action() ==SET){  // 滑移电机 右移
								if(myDelay(10) == SET){
									logic_data.loop_cnt ++;								
									logic_data.task_cnt = CLAMP_TASK;				// 取第一箱之后不检测基恩士状态	
								}							
							}						
						}							
						else{
							if(slipToInitPosition_Action() == SET){  // 滑移电机 	至初始位置		
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
				case FOUR_BOX_FETCH:  // 开局取弹 4箱
				{
						flipToMidAngle_Action();   //  翻转电机 进入扫描状态
						if(slipToMidPosition_Action() == SET){  // 滑移电机 
								logic_data.loop_cnt ++;
								logic_data.task_cnt ++;
						}
				}break;		
				case TWO_BOX_FETCH:  // 开局4分钟  第一排 2箱
				{
					if(logic_data.endFlag == RESET){					
						flipToMidAngle_Action();   //  翻转电机 进入扫描状态
						if(logic_data.loop_cnt == 0){
							if(slipToLeftPosition_Action() == SET){  // 滑移电机 
									if(myDelay(10) == SET){
										logic_data.loop_cnt ++;														
										logic_data.task_cnt ++;
								}
							}
						}
						else if(logic_data.loop_cnt == 1){
							if(slipToRightPosition_Action() ==SET){  // 滑移电机 右移
								if(myDelay(10) == SET){
									logic_data.loop_cnt ++;								
									logic_data.task_cnt = CLAMP_TASK;				// 取第一箱之后不检测基恩士状态	
								}							
							}						
						} 	
						else{
							if(slipToInitPosition_Action() == SET){  // 滑移电机 	至初始位置		
									logic_data.task_cnt = QUIT_TASK;					
							}	
						}		
					}
					else{
						ToInitPosition_Action();					
					}
				}break;	
				case THREE_BOX_FETCH:   // 小资源岛 弹药 三箱
				{
					if(logic_data.endFlag == RESET){					
						flipToMidAngle_Action();   //  翻转电机 进入扫描状态
						if(logic_data.loop_cnt == 0){
							if(slipToLeftPosition_Action() == SET){  // 滑移电机 
									if(myDelay(10) == SET){
									logic_data.loop_cnt ++;														
										logic_data.task_cnt = CLAMP_TASK;
								}
							}						
						} 							
						else if(logic_data.loop_cnt == 1){
							if(slipToMidPosition_Action() ==SET){  // 滑移电机 右移
								if(myDelay(10) == SET){
									logic_data.loop_cnt ++;								
									logic_data.task_cnt = CLAMP_TASK;				// 取第一箱之后不检测基恩士状态	
								}							
							}						
						}
						else if(logic_data.loop_cnt == 2){
							if(slipToRightPosition_Action() ==SET){  // 滑移电机 右移
								if(myDelay(10) == SET){
									logic_data.loop_cnt ++;
									logic_data.task_cnt = CLAMP_TASK;				// 取第一箱之后不检测基恩士状态	
								}
							}
						}
						else{
							if(slipToInitPosition_Action() == SET){  // 滑移电机 	至初始位置		
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

