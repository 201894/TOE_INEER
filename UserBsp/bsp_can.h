
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
	CAN_MASTER_M1_ID          = 0x301,  // ������Ϣ���� id
	CAN_MASTER_M2_ID          = 0x302,
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
#define FILTER_BUF 5
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
  float      total_angle;
  float      stir_angle;
  float      angle_offset;
  uint16_t offset_ecd;
  uint32_t msg_cnt;
  int32_t  ecd_raw_rate;
  int32_t  rate_buf[FILTER_BUF];
  uint8_t  buf_cut;
  int32_t  filter_rate;
	
} moto_param;

typedef __packed struct 
{  
	uint8_t game_type : 4; /*1 ���״�ʦ��   
												  *2  ������   
												  *3  ICRA	*/
	uint8_t game_progress : 4;/*0  δ��ʼ���� 
														 *1  ׼���׶�  
														 *2  �Լ�׶�
														 *3  5s����ʱ 
														 *4  ��ս��
														 *5  ����������  */
	uint16_t stage_remain_time; /* ��ǰ�׶�ʣ��ʱ�� s*/
} ext_game_state_t; 

typedef __packed struct{ 
	uint8_t winner; /* 0 ƽ��
									 * 1 �췽ʤ��
									 * 2 ����ʤ�� */
}ext_game_result_t;

typedef __packed struct{
	uint16_t robot_legion; /* �췽 λ0��ʼ Ӣ�ۡ����̡�����1������2������3�����С��ڱ������� 
												  * ���� λ8��ʼ Ӣ�ۡ����̡�����1������2������3�����С��ڱ������� */
}ext_game_robot_survivors_t;

typedef __packed struct{
	uint32_t event_type; /* ���� ����Ҫ�����ֲ� */
}ext_event_data_t;

typedef __packed struct { 
	uint8_t supply_projectile_id;/* ����վID    
															  * 1��1�Ų����� 
															  * 2��2�Ų����� */
	uint8_t supply_robot_id;/* ����������ID   
													 * 0 �� �޻����˲��� 
													 * 1 �� ��Ӣ��
													 * 2 �� �칤��
													 * 3/4/5 �� �첽��
													 * 11 �� ��Ӣ��
													 * 12 �� ������
													 * 13/14/15 �� ������		*/
	uint8_t supply_projectile_step;/* �����ڿ���״̬ 
																  * 0���ر�
																  * 1���ӵ�׼����
																  * 2���ӵ����� */
	uint8_t supply_projectile_num;  /* ��������  50/100/150/200 */
} ext_supply_projectile_action_t;

typedef __packed struct 
{  
	uint8_t robot_id;/* ������ID 
									  * 1�� ��Ӣ��
									  * 2�� �칤��
									  * 3/4/5���첽��
									  * 6�� �����
									  * 7�� ���ڱ�
										  * 11����Ӣ��
										  * 12��������
										  * 13/14/15��������		
										  * 16  ������
										  * 17  ���ڱ�*/
	uint8_t robot_level; /* �����˵ȼ� 1/2/3*/
	uint16_t remain_HP; /* ������ʣ��Ѫ�� */
	uint16_t max_HP; /* ����������Ѫ�� */
	uint16_t shooter_heat0_cooling_rate; /* ������17mmǹ��ÿ����ȴֵ */
	uint16_t shooter_heat0_cooling_limit; /* ������17mmǹ���������� */
	uint16_t shooter_heat1_cooling_rate;	/* ������42mmǹ��ÿ����ȴֵ */
	uint16_t shooter_heat1_cooling_limit; /* ������42mmǹ���������� */
	uint8_t mains_power_gimbal_output : 1; /* ��̨��Դ������ */
	uint8_t mains_power_chassis_output : 1; /* ���̵�Դ������ */
	uint8_t mains_power_shooter_output : 1; /* ����Ħ���ֵ�Դ������ */
} ext_game_robot_state_t; 

//0x0103	���󲹸�վ���� �Կ���δ����
typedef __packed struct {
	uint8_t supply_projectile_id;
	uint8_t supply_robot_id;
	uint8_t supply_num; 
} ext_supply_projectile_booking_t;

typedef __packed struct 
{   
	uint16_t chassis_volt; /* ���������ѹ mV */
	uint16_t chassis_current; /* ����������� mA*/
	float chassis_power; /* ����������� W */
	uint16_t chassis_power_buffer; /* ���̹��ʻ��� J */
	uint16_t shooter_heat0; /* 17mmǹ������ */
	uint16_t shooter_heat1; /* 42mmǹ������ */

/*	                        /s                          /s
level     17mm      cd            42mm    cd
  1         240         40            200       20  
  2         360         60            300       40
  3	     480         80            400       60
*/	
	
} ext_power_heat_data_t; 

typedef __packed struct {
	float x; 
	float y; 
	float z; 
	float yaw; 
} ext_game_robot_pos_t;

//0x0204 ����������
typedef __packed struct { 
	uint8_t power_rune_buff; /* bit0  ��Ѫ״̬ 
													  * bit1  ������ȴ�ӱ�
													  * bit2  �����ӳ�
													  * bit3  �����ӳ� 
													  * ���ౣ�� */
}ext_buff_musk_t;

//0x0206 �˺�״̬
typedef __packed struct {
	uint8_t armor_id : 4; /* װ���˺�ʱΪװ��ID ����Ϊ0 */
	uint8_t hurt_type : 4; /* 0 װ���˺� 
												  * 1 ģ�����߿�Ѫ 
												  * 2 ǹ�ڳ�������Ѫ 
												  * 3 ���̳����ʿ�Ѫ */
} ext_robot_hurt_t;

//0x0207 ʵʱ�����Ϣ
typedef __packed struct {
	uint8_t bullet_type; /* �ӵ����� 17mmΪ1 42mmΪ2*/
	uint8_t bullet_freq; /* ��Ƶ */
	float bullet_speed; /* ���� */
} ext_shoot_data_t;

//0x0301 �����˼佻������
typedef __packed struct { 
	uint16_t data_cmd_id; /* ���ݶε�����ID */
	uint16_t send_ID; /* �����ߵ�ID ����1�� ��IDΪ 1*/
	uint16_t receiver_ID; /* �����ߵ�ID */
}ext_student_interactive_header_data_t;

typedef __packed struct{
	float data1; /* �Զ��帡������ 1 */ 
	float data2; /* �Զ��帡������ 2 */
	float data3; /* �Զ��帡������ 3 */
	uint8_t masks; /* �Զ����λ���� λ0-5 �ֱ����������������ָʾ�� */
} client_custom_data_t;

//������֮��ͨ������ 
/* ���ͻ�����֮��ͨ����Ϣʱ�� ����ID Ϊ0x0200-0x02FF 
															������ID Ϊ���ͻ����˵�ID 
															������ID Ϊ���ܻ����˵�ID 
															���ݶε��ֽ���ҪС��113 */
typedef __packed struct
{
	uint8_t data[112];
} robot_interactive_data_t;

extern moto_param    MotoData[5];
extern wl4data   data4bytes;
extern wl2data   data2bytes;
extern ext_game_state_t               ext_game_state;
extern ext_game_robot_state_t     ext_game_robot_state;
extern ext_power_heat_data_t      ext_power_heat_data;
extern ext_game_robot_pos_t       ext_game_robot_pos;
extern ext_buff_musk_t                 ext_buff_musk;
extern ext_shoot_data_t                ext_shoot_data;
extern ext_robot_hurt_t                 ext_robot_hurt;

void can_device_init(void);
void can_receive_start(void);
void CAN_InitArgument(void);
void send_gimbal_ms(uint32_t id,uint8_t data[8]);
void send_chassis_ms(uint32_t id,uint8_t data[8]);
void encoder_data_handle(moto_param* ptr,uint8_t RxData[8]);
void gyro_data_handle(wl2data* ptr,wl4data* ptrr,gyro_param* gyro,uint8_t RxData[8]);
void send_can1_cur(uint32_t id,int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4);  //CAN 1
void send_can2_cur(uint32_t id,int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4);  //CAN 2

void send_405(uint32_t id,uint8_t data[8]);
#endif
