
/** @file bsp_can.c
 *  @version 4.0
 *  @date  June 19
 *
 *  @brief receive external can device message
 *
 */
#include "logic_handle_task.h"
#include "bsp_can.h"
#include "cmsis_os.h"
#include "can.h"
#include "detect_task.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#define ENCODER_ANGLE_RATIO               (8192.0f/360.0f)
#define REDUCTION_RATIO                   (36/1)
#define pi                                 			3.1415926.f

uint8_t 														TxData[8];
wl2data       											data2bytes;
wl4data       											data4bytes; 
moto_param    								MotoData[5];
CAN_TxHeaderTypeDef  	CAN_TxHeader;

/**
  * @brief   can filter initialization
  * @param   CAN_HandleTypeDef
  * @retval  None
  */  

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan)
{

  uint8_t RxData1[8],RxData2[8];
  CAN_RxHeaderTypeDef Can1Header,Can2Header;
  if(hcan->Instance == CAN1)
  {
		HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0,&Can1Header,RxData1);
    switch (Can1Header.StdId)
		{
	    case CAN_FLIP_M1_ID:
	    {
				encoder_data_handle(&MotoData[LeftFlip],RxData1);
				err_detector_hook(CAN_FLIP_LEFT_OFFLINE);
				g_fps[LeftFlip].cnt ++;			
	    }break; 				
			
	    case CAN_FLIP_M2_ID: 
	    {
				encoder_data_handle(&MotoData[RightFlip],RxData1);
				err_detector_hook(CAN_FLIP_RIGHT_OFFLINE);
				g_fps[RightFlip].cnt ++;			
	    }break; 				
	    case CAN_3508_SLIP_ID: 				
	    {
				encoder_data_handle(&MotoData[MidSlip],RxData1);
				err_detector_hook(CAN_SLIP_OFFLINE);
				g_fps[MidSlip].cnt ++;			
	    }break; 
			
	    default:
      {		 
      }break;
		}
  }
  if(hcan->Instance == CAN2)
  {
		HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0,&Can2Header,RxData2);
    switch (Can2Header.StdId)
		{
	    case CAN_UPLIFT_M1_ID:
			{
				encoder_data_handle(&MotoData[LeftUpLift],RxData2);
				err_detector_hook(CAN_UPLIFT_LEFT_OFFLINE);			
				g_fps[LeftUpLift].cnt ++;						
			}break;								
			
	    case CAN_UPLIFT_M2_ID:
			{
				encoder_data_handle(&MotoData[RightUpLift],RxData2);
				err_detector_hook(CAN_UPLIFT_RIGHT_OFFLINE);		
				g_fps[RightUpLift].cnt ++;					
			}break;			
			
      case CAN_MASTER_M1_ID:
			{
				data2bytes.c[0] = RxData2[0];
				data2bytes.c[1] = RxData2[1];		
				
				logic_data.raw_mode = data2bytes.d; 
				
				err_detector_hook(MODE_CTRL_OFFLINE);		
				g_fps[MasterID].cnt ++;						
			}break;
			
      case CAN_MASTER_M2_ID:
			{
			  
			}break;			
			default:
      { 
      }break;
    }
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
	   ptr->offset_angle = ptr->offset_ecd/ENCODER_ANGLE_RATIO;  		 
	 }
	 else
	 {
		 ptr->ecd      = (uint16_t)(RxData[0] << 8 | RxData[1]);  
		 if (ptr->ecd - ptr->last_ecd > 4096)
		 {
			 ptr->round_cnt--;
		 }
		 else if (ptr->ecd - ptr->last_ecd < -4096)
		 {
			 ptr->round_cnt++;
		 }
		 ptr->total_ecd = ptr->round_cnt * 8192 + ptr->ecd - ptr->offset_ecd;
		 /* total angle, unit is degree */
		 ptr->total_angle = ptr->total_ecd / ENCODER_ANGLE_RATIO; 
		 ptr->speed_rpm     = (int16_t)(RxData[2] << 8 | RxData[3]);
		 ptr->current = (int16_t)(RxData[2] << 8 | RxData[3]);
	 }
}

/**
  * @brief  send calculated current to motor
  * @param  3508 motor ESC id
  */
void send_can1_cur(uint32_t id,int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4)
{
	  CAN_TxHeader.StdId    = id;
	  CAN_TxHeader.IDE      = CAN_ID_STD;
	  CAN_TxHeader.RTR      = CAN_RTR_DATA;
	  CAN_TxHeader.DLC      = 0x08;
	  TxData[0] = iq1 >> 8;
	  TxData[1] = iq1;
	  TxData[2] = iq2 >> 8;
	  TxData[3] = iq2;
	  TxData[4] = iq3 >> 8;
	  TxData[5] = iq3;
	  TxData[6] = iq4 >> 8;
	  TxData[7] = iq4;
    HAL_CAN_AddTxMessage(&hcan1,&CAN_TxHeader,TxData,(uint32_t *)CAN_TX_MAILBOX0);
}

void send_can2_cur(uint32_t id,int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4)
{
	  CAN_TxHeader.StdId    = id;
	  CAN_TxHeader.IDE      = CAN_ID_STD;
	  CAN_TxHeader.RTR      = CAN_RTR_DATA;
	  CAN_TxHeader.DLC      = 0x08;
//	  TxData[0] = iq1 >> 8;
//	  TxData[1] = iq1;
//	  TxData[2] = iq2 >> 8;
//	  TxData[3] = iq2;
	  TxData[4] = iq3 >> 8;
	  TxData[5] = iq3;
	  TxData[6] = iq4 >> 8;
	  TxData[7] = iq4;
    HAL_CAN_AddTxMessage(&hcan2,&CAN_TxHeader,TxData,(uint32_t *)CAN_TX_MAILBOX0);
}
void send_chassis_ms(uint32_t id,uint8_t data[8])
{
	  CAN_TxHeader.StdId    = id;
	  CAN_TxHeader.IDE      = CAN_ID_STD;
	  CAN_TxHeader.RTR     = CAN_RTR_DATA;
	  CAN_TxHeader.DLC     = 0x08;
	  TxData[0] = data[0];
	  TxData[1] = data[1];
	  TxData[2] = data[2];
	  TxData[3] = data[3];
	  TxData[4] = data[4];
	  TxData[5] = data[5];
	  TxData[6] = data[6];
	  TxData[7] = data[7];
    HAL_CAN_AddTxMessage(&hcan1,&CAN_TxHeader,TxData,(uint32_t *)CAN_TX_MAILBOX0);
}

void can_device_init(void)
{
  CAN_FilterTypeDef  can_filter;
	
	can_filter.FilterActivation     = ENABLE;
	can_filter.FilterBank         	= 0U;
	can_filter.FilterIdHigh         = 0x0000;
	can_filter.FilterIdLow          = 0x0000;
	can_filter.FilterMaskIdHigh     = 0x0000;
	can_filter.FilterMaskIdLow      = 0x0000;
	can_filter.FilterFIFOAssignment = CAN_FilterFIFO0;
	can_filter.FilterMode           = CAN_FILTERMODE_IDMASK;
	can_filter.FilterScale          = CAN_FILTERSCALE_32BIT;
	can_filter.SlaveStartFilterBank = 14;
	HAL_CAN_ConfigFilter(&hcan1, &can_filter);
  //while (HAL_CAN_ConfigFilter(&hcan1, &can_filter) != HAL_OK);
	
  can_filter.FilterActivation     = ENABLE;	
  can_filter.FilterBank         	= 14U;
	can_filter.FilterMode					  = CAN_FILTERMODE_IDLIST;//列表模式
	can_filter.FilterScale					= CAN_FILTERSCALE_16BIT;//16位宽
	can_filter.FilterFIFOAssignment	= CAN_FILTER_FIFO0;
	can_filter.FilterIdHigh 				= ((uint16_t)CAN_UPLIFT_M1_ID)<<5 ;
	can_filter.FilterIdLow 					= ((uint16_t)CAN_UPLIFT_M2_ID)<<5;
	can_filter.FilterMaskIdHigh 		= ((uint16_t)CAN_MASTER_M1_ID)<<5;
	can_filter.FilterMaskIdLow 			= ((uint16_t)CAN_MASTER_M2_ID)<<5;
  HAL_CAN_ConfigFilter(&hcan2, &can_filter);	
  //while (HAL_CAN_ConfigFilter(&hcan2, &can_filter) != HAL_OK);
}

void can_receive_start(void)
{

	HAL_CAN_Start(&hcan1);
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);	
	HAL_CAN_Start(&hcan2);
	HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);

}

void CAN_InitArgument(void)
{
		can_device_init();
		can_receive_start();
}


