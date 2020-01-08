
/** @file bsp_can.h
 *  @version 4.0
 *  @date  June 2019
 *
 *  @brief receive external can device message
 *
 */

#ifndef  __BSP_CAN_H__
#define __BSP_CAN_H__

#include "stm32f1xx_hal.h"



typedef enum
{
	LeftFlip           = 0,  
	RightFlip         = 1,
	MidSlip           = 2,
	MasterID        = 3,
	MaxId             = 5,
}module_id;

typedef enum
{
  Flip          = 0,	
	Slip          = 1,  
}module_part_id;

typedef enum
{

  CAN_FLIP_M1_ID            = 0x201,	
	CAN_FLIP_M2_ID            = 0x202,
  CAN_SLIP_M1_ID             = 0x203, 	
	CAN_MASTER_M1_ID        = 0x301,  // 主机信息接收 id
	CAN_MASTER_M2_ID        = 0x302,
	CAN_SEND_M1_ID            = 0x311,
	CAN_SEND_M2_ID            = 0x312,		
} can_msg_id;

/* can receive motor parameter structure */
typedef struct
{
  uint16_t ecd;
	uint16_t offset_ecd;
  uint16_t last_ecd;
  uint16_t temp;	
  int16_t  speed_rpm;
  int16_t  current;
  int16_t  torque;	
  int32_t  round_cnt;
  int32_t  total_ecd;
  float  total_angle;
  uint8_t  init_flag;	
} moto_param;

typedef union{
	uint8_t c[2];
	int16_t d;
}wl2data;

typedef union
{
	uint8_t c[4];
	float f;
	uint32_t d;
}wl4data;

extern wl4data  data4bytes;  
extern moto_param    	MotoData[3];
void CanFilter_Init(CAN_HandleTypeDef* hcan);
void CAN_InitArgument(void);

void  CAN1_Send_Current(uint32_t id,int16_t cur1,int16_t cur2, int16_t cur3, int16_t cur4 );
void encoder_data_handle(moto_param* ptr,uint8_t RxData[8]);
void send_can_ms(uint32_t id,int16_t gy,int16_t gz,float angle);
#endif
