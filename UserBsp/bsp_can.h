
/** @file bsp_can.h
 *  @version 4.0
 *  @date  June 2019
 *
 *  @brief receive external can device message
 *
 */

#ifndef  __BSP_CAN_H__
#define __BSP_CAN_H__

#include "stm32f4xx_hal.h"

typedef enum
{
  LeftUpLift        = 0,  //
  RightUpLift     = 1,	
	LeftFlip           = 2,  
	RightFlip         = 3,
	MidSlip           = 4,
	MasterID        = 5,
	MaxId             = 10,
}module_id;

typedef enum
{
  UpLift      = 0,  //
  Flip          = 1,	
	Slip          = 2,  
}module_part_id;

typedef enum
{

  CAN_UPLIFT_M1_ID          = 0x207,  //
  CAN_UPLIFT_M2_ID          = 0x208,	
	CAN_MASTER_M1_ID        = 0x301,  // 主机信息接收 id
	CAN_MASTER_M2_ID        = 0x302,
	CAN_SEND_M1_ID            = 0x311,
	CAN_SEND_M2_ID            = 0x312,	
} can_msg_id2;

typedef enum
{

  CAN_FLIP_M1_ID            = 0x201,	
	CAN_FLIP_M2_ID            = 0x202,
  CAN_3508_SLIP_ID          = 0x203, 	
} can_msg_id1;

typedef union
{
	uint8_t c[2];
	int16_t d;
	uint16_t ud;
}wl2data;

typedef union
{	
	uint8_t c[4];
	float f;
	uint32_t d;
}wl4data;

typedef struct
{
	float pit_speed;
	float angle_z;
	float yaw_speed;
	float yaw_speed_last;	
	float pit_speed_last;		
}gyro_param;


/* can receive motor parameter structure */
typedef struct
{
  uint16_t ecd;
  uint16_t last_ecd;
  uint16_t temp;	
  int16_t  speed_rpm;
  int16_t  current;
  int16_t  torque;	
  int32_t  round_cnt;
  int32_t  total_ecd;
  float    total_angle;
	float    offset_angle;
  uint16_t offset_ecd;
  uint8_t  init_flag;	
} moto_param;


void can_device_init(void);
void can_receive_start(void);
void CAN_InitArgument(void);

void send_chassis_ms(uint32_t id,uint8_t data[8]);
void encoder_data_handle(moto_param* ptr,uint8_t RxData[8]);

void send_can1_cur(uint32_t id,int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4);  //CAN 1
void send_can2_cur(uint32_t id,int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4);  //CAN 2
extern moto_param  MotoData[5];
#endif
