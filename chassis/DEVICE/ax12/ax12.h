
#ifndef AX12_H
#define AX12_H

#include "hardware.h"



#define	ID_ALL		0Xfe			//�㲥ID
//LX-16�Ŀ��Ʊ�
#define LOBOT_SERVO_FRAME_HEADER         0x55
#define LOBOT_SERVO_MOVE_TIME_WRITE      1
#define LOBOT_SERVO_MOVE_TIME_READ       2
#define LOBOT_SERVO_MOVE_TIME_WAIT_WRITE 7
#define LOBOT_SERVO_MOVE_TIME_WAIT_READ  8
#define LOBOT_SERVO_MOVE_START           11
#define LOBOT_SERVO_MOVE_STOP            12
#define LOBOT_SERVO_ID_WRITE             13
#define LOBOT_SERVO_ID_READ              14
#define LOBOT_SERVO_ANGLE_OFFSET_ADJUST  17
#define LOBOT_SERVO_ANGLE_OFFSET_WRITE   18
#define LOBOT_SERVO_ANGLE_OFFSET_READ    19
#define LOBOT_SERVO_ANGLE_LIMIT_WRITE    20
#define LOBOT_SERVO_ANGLE_LIMIT_READ     21
#define LOBOT_SERVO_VIN_LIMIT_WRITE      22
#define LOBOT_SERVO_VIN_LIMIT_READ       23
#define LOBOT_SERVO_TEMP_MAX_LIMIT_WRITE 24
#define LOBOT_SERVO_TEMP_MAX_LIMIT_READ  25
#define LOBOT_SERVO_TEMP_READ            26
#define LOBOT_SERVO_VIN_READ             27
#define LOBOT_SERVO_POS_READ             28
#define LOBOT_SERVO_OR_MOTOR_MODE_WRITE  29
#define LOBOT_SERVO_OR_MOTOR_MODE_READ   30
#define LOBOT_SERVO_LOAD_OR_UNLOAD_WRITE 31
#define LOBOT_SERVO_LOAD_OR_UNLOAD_READ  32
#define LOBOT_SERVO_LED_CTRL_WRITE       33
#define LOBOT_SERVO_LED_CTRL_READ        34
#define LOBOT_SERVO_LED_ERROR_WRITE      35
#define LOBOT_SERVO_LED_ERROR_READ       36


//AX-12���Ʊ�
#define P_MODEL_NUMBER_L      		0
#define P_MODOEL_NUMBER_H     		1
#define P_VERSION             		2
#define P_ID                  		3
#define P_BAUD_RATE           		4
#define P_RETURN_DELAY_TIME   		5
#define P_CW_ANGLE_LIMIT_L    		6
#define P_CW_ANGLE_LIMIT_H    		7
#define P_CCW_ANGLE_LIMIT_L   		8
#define P_CCW_ANGLE_LIMIT_H   		9
#define P_SYSTEM_DATA2        		10
#define P_LIMIT_TEMPERATURE   		11
#define P_DOWN_LIMIT_VOLTAGE  		12
#define P_UP_LIMIT_VOLTAGE    		13
#define P_MAX_TORQUE_L        		14
#define P_MAX_TORQUE_H        		15
#define P_RETURN_LEVEL        		16
#define P_ALARM_LED           		17
#define P_ALARM_SHUTDOWN      		18
#define P_OPERATING_MODE      		19
#define P_DOWN_CALIBRATION_L  		20
#define P_DOWN_CALIBRATION_H  		21
#define P_UP_CALIBRATION_L    		22
#define P_UP_CALIBRATION_H    		23

#define P_TORQUE_ENABLE         	24
#define P_LED                   	25
#define P_CW_COMPLIANCE_MARGIN  	26
#define P_CCW_COMPLIANCE_MARGIN 	27
#define P_CW_COMPLIANCE_SLOPE   	28
#define P_CCW_COMPLIANCE_SLOPE  	29
#define P_GOAL_POSITION_L      	 	30
#define P_GOAL_POSITION_H      	 	31
#define P_GOAL_SPEED_L          	32
#define P_GOAL_SPEED_H          	33
#define P_TORQUE_LIMIT_L        	34
#define P_TORQUE_LIMIT_H        	35
#define P_PRESENT_POSITION_L    	36
#define P_PRESENT_POSITION_H    	37
#define P_PRESENT_SPEED_L       	38
#define P_PRESENT_SPEED_H       	39
#define P_PRESENT_LOAD_L        	40
#define P_PRESENT_LOAD_H        	41
#define P_PRESENT_VOLTAGE       	42
#define P_PRESENT_TEMPERATURE   	43
#define P_REGISTERED_INSTRUCTION	44
#define P_PAUSE_TIME            	45
#define P_MOVING 					46
#define P_LOCK                  	47
#define P_PUNCH_L               	48
#define P_PUNCH_H               	49


//ָ������
typedef enum tagAXORDER
{
	AXORDER_PING           = 0x01,
	AXORDER_READ           = 0x02,
	AXORDER_WRITE          = 0x03,
	AXORDER_REG_WRITE      = 0x04,
	AXORDER_ACTION         = 0x05,
	AXORDER_RESET          = 0x06,
	AXORDER_DIGITAL_RESET  = 0x07,
	AXORDER_SYSTEM_READ    = 0x0C,
	AXORDER_SYSTEM_WRITE   = 0x0D,
	AXORDER_SYNC_WRITE     = 0x83,
	AXORDER_SYNC_REG_WRITE = 0x84	
}AXORDER;


/*AXָ��ִ�н��*/
typedef enum
{	
	AXERO_VOLTAGE,			//��ѹ��Χ����
	AXERO_ANGLE,			//�Ƕ����ƴ���
	AXERO_HEAT,				//���ȴ���
	AXERO_RANGE,			//ָ�Χ����
	AXERO_CHECK,			//У�������
	AXERO_LOAD,				//���ش���
	AXERO_INST,				//ָ�����
	AXERO_OTHER,			//������������
	AXERO_OK,				//Ӧ��ɹ�		
//	AXERO_ID,				//����ID����ȷ
//	Timeout,				//Ӧ��ʱ
}AXERROR;


//AX12�������������
typedef struct tagAXFB
{
	u8 id;					//�����1
	u8 length;             	//���ݳ���
	u8 axerror;            	//��������
	u8 param[20];          	//��������
	u8 check;				//У��λ
}AXFB, *PAXFB;



//AX12���������״̬��
typedef enum tagAXSTATE
{
	AXSTATE_HEAD1,
	AXSTATE_HEAD2,
	AXSTATE_ID,
	AXSTATE_LENGTH,
	AXSTATE_ERROR,
	AXSTATE_DATA,
	AXSTATE_CHECK
}AXSTATE;


typedef struct tagFLEXCONFIG
{
	u8 cwbound;		//˳ʱ�����Ա߾�
	u8 ccwbound;	//��ʱ�����Ա߾�
	u8 cwslope;		//˳ʱ������б��
	u8 ccwslope;	//��ʱ������б��
	u8 hit_high;	//ײ��(��)
	u8 hit_low;		//ײ��(��)	
}FLEXCONFIG, *PFLEXCONFIG;

//��������

u8 axReset(u8 ID);

//���͸�ĳһ�����һ��Ҫ�ƶ�����λ��
//ID: 			���ID
//target_pos: 	Ŀ��λ��	0-1023
//target_speed: ָ���ٶ�	0-1023
uint8_t axSendPosition(u8 ID, u16 target_pos, u16 target_speed);


//��ʹ�� / ʹ�ܶ��
//ID: 	���ID
//mode:	0Ϊ��ʹ��, 1Ϊʹ��
u8 axTorque(u8 ID, u8 mode);

//�޸Ķ��ID
//ID: 		���ID
//trgID:	�޸ĺ�ID	
void axModifyID(u8 ID, u8 trgID);

//�������Ͷ��λ��Ԥ������
//num_ax:		�������, ���͵�ID��1~numax
//position[]:	λ������
//speed:		�ٶ�
u8 axAllPosition(u8 num_ax, u16 position[], u16 speed);

u16 axGetPosition(u8 ID);

//�޸�����б�ʵ�
u8 axFlexible(u8 ID, PFLEXCONFIG pflexconfig);
//LX16���
u8 LobotCheckSum(u8 buf[]);
void LobotSerialServoMove(u8 id, u16 position, u16 time);
void LobotSerialServoSetID(u8 oldID, u8 newID);

#define AX_ON	1
#define AX_OFF	0

#endif



