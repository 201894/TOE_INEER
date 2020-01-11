
/** @file bsp_can.c
 *  @version 4.0
 *  @date  June 19
 *
 *  @brief receive external can device message
 *
 */

#include "bsp_can.h"
#include "cmsis_os.h"
#include "minorThread.h"
#include "logic_handle_task.h"
#include "can.h"
#include <math.h>
#include <string.h>
#include <stdio.h>

#define ENCODER_ANGLE_RATIO               (8192.0f/360.0f)
#define REDUCTION_RATIO                   (19/1)
#define pi                                 			3.1415926.f

CanTxMsgTypeDef  				can1_tx;
CanRxMsgTypeDef  				can1_rx;
CanTxMsgTypeDef  				can2_tx;
CanRxMsgTypeDef  				can2_rx;
wl4data            						data4bytes,data4bytes1;    
wl2data            						data2bytes;   
moto_param    						MotoData[3];

/**
  * @brief   can filter initialization
  * @param   CAN_HandleTypeDef
  * @retval  None
  */  

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
  if(hcan->Instance == CAN1)
  {	
   switch (hcan->pRxMsg->StdId)
	 {
       case CAN_FLIP_M1_ID:{
				encoder_data_handle(&MotoData[LeftFlip],can1_rx.Data);			
				g_fps[LeftFlip].cnt ++;	
			} break;
       case CAN_FLIP_M2_ID:{
				encoder_data_handle(&MotoData[RightFlip],can1_rx.Data);				
				g_fps[RightFlip].cnt ++;						 
			} break;	
       case CAN_SLIP_M1_ID:{
				encoder_data_handle(&MotoData[MidSlip],can1_rx.Data);		
				g_fps[MidSlip].cnt ++;					 
			} break;			
       case CAN_MASTER_M1_ID:{				 
				logic_data.fetch_mode = can1_rx.Data[0];
				logic_data.upLiftPosFlag = can1_rx.Data[1];				 
				g_fps[MasterID].cnt ++;						 
			} break;	
       case CAN_MASTER_M2_ID:{
			 
			} break;						 
	   default:
	    {}
		break;
	 }
	__HAL_CAN_ENABLE_IT(&hcan1,CAN_IT_FMP0);
 }
}

/**
  * @brief     get motor rpm and calculate motor round_count/total_encoder/total_angle
  * @param     ptr: Pointer to a moto_measure_t structure
  * @attention this function should be called after get_moto_offset() function
  */

void encoder_data_handle(moto_param* ptr,uint8_t RxData[8])
{
   ptr->last_ecd = ptr->ecd;
	 if(ptr->init_flag == 0)
	 {
		 ptr->init_flag = 1;
		 ptr->round_cnt = 0;		 
	   ptr->offset_ecd = (uint16_t)(RxData[0] << 8 | RxData[1]);  	 
		 if(ptr->offset_ecd > 4096)
			 ptr->round_cnt ++;		
	 }
	 else
	 {
		 ptr->ecd      = (uint16_t)(RxData[0] << 8 | RxData[1]);  
		 if (ptr->ecd - ptr->last_ecd > 4096)
		 {
			 ptr->round_cnt --;
		 }
		 else if (ptr->ecd - ptr->last_ecd < -4096)
		 {
			 ptr->round_cnt++;
		 }
		 ptr->total_ecd = ptr->round_cnt * 8192 + ptr->ecd - ptr->offset_ecd;
		 /* total angle, unit is degree */
		 ptr->total_angle = ptr->total_ecd / (ENCODER_ANGLE_RATIO * REDUCTION_RATIO); 
		 ptr->speed_rpm     = (int16_t)(RxData[2] << 8 | RxData[3]);
		 ptr->current = (int16_t)(RxData[2] << 8 | RxData[3]);
	 }
}
/**
 * @brief CAN1 send msg functions using the can transmit_IT
 * @param 1. can_send_id
 *				2. data
 * @return None
 * @attention you can choose the suitable one to use according 
 *						to the data type you want to transmit
 */
void CAN1_Send_Current(uint32_t id,int16_t cur1,int16_t cur2, int16_t cur3, int16_t cur4 ){
	hcan1.pTxMsg->StdId = id;
	hcan1.pTxMsg->IDE = CAN_ID_STD;
	hcan1.pTxMsg->RTR = CAN_RTR_DATA;
	hcan1.pTxMsg->DLC = 0x08;	
	hcan1.pTxMsg->Data[0] = (unsigned char)(cur1 >> 8);
	hcan1.pTxMsg->Data[1] = (unsigned char)cur1;
	hcan1.pTxMsg->Data[2] = (unsigned char)(cur2 >> 8);
	hcan1.pTxMsg->Data[3] = (unsigned char)cur2;
	hcan1.pTxMsg->Data[4] = (unsigned char)(cur3 >> 8);
	hcan1.pTxMsg->Data[5] = (unsigned char)cur3;
	hcan1.pTxMsg->Data[6] = (unsigned char)(cur4 >> 8);
	hcan1.pTxMsg->Data[7] = (unsigned char)cur4;	
	HAL_CAN_Transmit_IT(&hcan1);
}
/**
  * @brief  send current which pid calculate to esc. message to calibrate 6020 gimbal motor esc
  * @param  current value corresponding motor(yaw/pitch/trigger)
  */

void send_can_ms(uint32_t id,int16_t slipPos,int16_t flipAngle,uint8_t upLiftFlag)
{
	  hcan1.pTxMsg->StdId = id;
	  hcan1.pTxMsg->IDE = CAN_ID_STD;
	  hcan1.pTxMsg->RTR = CAN_RTR_DATA;
	  hcan1.pTxMsg->DLC = 0x08;
	  data2bytes.d = slipPos;
	  hcan1.pTxMsg->Data[0] = data2bytes.c[0];
	  hcan1.pTxMsg->Data[1] = data2bytes.c[1];
	  data2bytes.d = flipAngle;	
	  hcan1.pTxMsg->Data[2] = data2bytes.c[0];
	  hcan1.pTxMsg->Data[3] = data2bytes.c[1];		
//	  data2bytes.d = extarUp;	
//	  hcan1.pTxMsg->Data[4] = data2bytes.c[0];
//	  hcan1.pTxMsg->Data[5] = data2bytes.c[1];		

	  HAL_CAN_Transmit_IT(&hcan1);	
}

void CanFilter_Init(CAN_HandleTypeDef* hcan)
{
  CAN_FilterConfTypeDef canfilter;
 	hcan->pTxMsg = &can1_tx;
	hcan->pRxMsg = &can1_rx;    
  //filtrate any ID you want here
  canfilter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  canfilter.FilterActivation = ENABLE;
  canfilter.FilterMode = CAN_FILTERMODE_IDLIST;
  canfilter.FilterScale = CAN_FILTERSCALE_16BIT;	
	canfilter.FilterNumber = 0U;
  canfilter.FilterIdHigh = ((uint16_t)CAN_FLIP_M1_ID)<<5 ;
  canfilter.FilterIdLow =  ((uint16_t)CAN_FLIP_M2_ID)<<5 ;
  canfilter.FilterMaskIdHigh =  ((uint16_t)CAN_SLIP_M1_ID)<<5 ;
  canfilter.FilterMaskIdLow =  ((uint16_t)CAN_MASTER_M1_ID)<<5 ;
	HAL_CAN_ConfigFilter(hcan, &canfilter);
	
  canfilter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  canfilter.FilterActivation = ENABLE;
  canfilter.FilterMode = CAN_FILTERMODE_IDLIST;
  canfilter.FilterScale = CAN_FILTERSCALE_16BIT;	
	canfilter.FilterNumber = 1U;
  canfilter.FilterIdHigh = ((uint16_t)CAN_MASTER_M2_ID)<<5 ;
  canfilter.FilterIdLow =  ((uint16_t)0)<<5 ;
  canfilter.FilterMaskIdHigh =  ((uint16_t)0)<<5 ;
  canfilter.FilterMaskIdLow =  ((uint16_t)0)<<5 ;
	HAL_CAN_ConfigFilter(hcan, &canfilter);	
}

void CAN_InitArgument(void)
{
			CanFilter_Init(&hcan1);
			__HAL_CAN_ENABLE_IT(&hcan1,CAN_IT_FMP0);
			__HAL_CAN_ENABLE_IT(&hcan1,CAN_IT_TME);
}
