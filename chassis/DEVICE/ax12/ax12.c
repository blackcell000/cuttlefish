#include "ax12.h"

#define GET_LOW_BYTE(A) ((u8)(A))
#define GET_HIGH_BYTE(A) ((u8)((A) >> 8))
#define BYTE_TO_HW(A, B) ((((u16)(A)) << 8) | (u8)(B))

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
//�û�����*********************************************************
void ax_usartx_senddat(u8 *buf, u8 len)
{
	usart3_senddat(buf, len);
}
void lx_usartx_senddat(u8 *buf, u8 len)
{
	usart3_senddat(buf, len);
}

#define	AX_MODE_TX		usart_modetx();	//�л�Ϊ����ģʽ
#define	AX_MODE_RX		usart_moderx();	//�л�Ϊ����ģʽ
#define AX_RXBUF_CLR	{ u3len = 0; }	//��մ��ڽ��ջ�����
#define AX_RXBUF		u3buf			//���ڽ��ջ������׵�ַ
//****************************************************************

u8 LobotCheckSum(u8 buf[])
{
  u8 i;
  u16 temp = 0;
  for (i=2;i<buf[3]+2;i++){
    temp+=buf[i];
  }
  temp=~temp;
  i=(u8)temp;
  return i;
}

u8 axSendPack(u8 ID, u8 order, u8 param[], u16 length)//�����12����Ļ����԰�ORDER�ı������͸Ĺ���
{
	u16 count = 0;
	//u32 check = 0;

	u8 ax_txbuf[256] = {0};
	
	if(length + 6 > 256)
		return 0;
	
	ax_txbuf[0] = LOBOT_SERVO_FRAME_HEADER;				//����ͷ0x55
	ax_txbuf[1] = LOBOT_SERVO_FRAME_HEADER;
	ax_txbuf[2] = ID;				//���ID
	ax_txbuf[3] = length + 3;		//���ݳ��ȣ�12����������в�����У��ͣ���16����Ҫ+1
	ax_txbuf[4] = order;			//ָ������
	
	//check = ax_txbuf[2] + ax_txbuf[3] + ax_txbuf[4];//�ȼ���ǰ�߼������ݵĺ�
	
	if(length != 0)
	{
		for (count = 0; count <= length - 1; count++) //�����ĺ�
		{
			ax_txbuf[count + 5] = param[count];
		}
	}	
	ax_txbuf[length + 5] = LobotCheckSum(ax_txbuf);//12��У���
	
	AX_MODE_TX;
	ax_usartx_senddat(ax_txbuf, length + 6);
	AX_MODE_RX;
	
	return length + 6;
}

//���͸�LX-16һ��Ҫת����λ��
u8 axSendPosition(u8 ID, u16 target_pos, u16 target_speed)
{
	u8 param[8] = {0};
	param[0] = GET_LOW_BYTE(target_pos); 	
	param[1] = GET_HIGH_BYTE(target_pos); 			
	param[2] = GET_LOW_BYTE(target_speed); 	
	param[3] = GET_HIGH_BYTE(target_speed); 		
	axSendPack(ID, LOBOT_SERVO_MOVE_TIME_WRITE, param, 4);
	delay_ms(2);
	return 0x00;
}


//LX-16�����ʹ�����ʹ�ܣ�mode:	0Ϊ��ʹ��, 1Ϊʹ��
u8 axTorque(u8 ID, u8 mode) 
{
	u8 param[1] = {0};
	param[0] = mode & 1;
	axSendPack(ID, LOBOT_SERVO_LOAD_OR_UNLOAD_WRITE, param, 1);
	return 1;//*****************************
}


u8 axReset(u8 ID)
{
	u8 param[8] = {0};


	axSendPack(ID, AXORDER_RESET, param, 0);
	delay_ms(1);
	return 0x00;	
}

//�޸�LX-16ID
void axModifyID(u8 oldID, u8 newID)
{
	u8 param[2];
  param[0] = newID;
  axSendPack(oldID,LOBOT_SERVO_ID_WRITE,param,1);
}


/*
//�������Ͷ��λ��Ԥ������
//num_ax:		�������, ���͵�ID��1~numax
//position[]:	λ������
//speed:		�ٶ�
u8 axAllPosition(u8 num_ax, u16 position[], u16 speed) 
{
	u8 count_ax = 0;
	u8 i = 0;
	u8 param[256] = {0};
	
	param[i++] = P_GOAL_POSITION_L;
	param[i++] = 4;	
	
	for(count_ax = 1; count_ax <= num_ax; count_ax++)
	{
		param[i++] = count_ax;	
		param[i++] = position[count_ax - 1] & 0x00ff;
		param[i++] = (position[count_ax - 1] >> 8) & 0x00ff;
		param[i++] = speed & 0x00ff;
		param[i++] = (speed >> 8) & 0x00ff;
	}
	
	axSendPack(ID_ALL, AXORDER_SYNC_WRITE, param, i);

	return 0x00;
}*/
//�������Ͷ��λ��Ԥ������
//num_ax:		�������, ���͵�ID��1~numax
//position[]:	λ������
//speed:		�ٶ�
u8 axAllPosition(u8 num_ax, u16 position[], u16 speed) 
{
	u8 i;
	for(i=0;i<num_ax;i++){
			axSendPosition(i+1,position[i],speed);
	}
	return 0x00;
}

//�޸����Ա߾�
u8 axFlexible(u8 ID, PFLEXCONFIG pflexconfig)
{
	u8 param[16] = {0};
	
	param[0] = P_CW_COMPLIANCE_MARGIN;
	param[1] = pflexconfig->cwbound;
	param[2] = pflexconfig->ccwbound;
	param[3] = pflexconfig->cwslope;
	param[4] = pflexconfig->ccwslope;
	axSendPack(ID, AXORDER_WRITE, param, 5);
	delay_ms(5);	
	
	param[0] = P_PUNCH_L;
	param[1] = pflexconfig->hit_low;
	param[2] = pflexconfig->hit_high;
	axSendPack(ID, AXORDER_WRITE, param, 3);
	delay_ms(5);

	return 0x00;	
}
/*LX-16���ص����ݰ���ʽ��ԭ������ͬ��ֻ��lengthλ��Ϊԭ���ļ�2��ͬʱ����֡�������λ�Ǵ���Ƕ�ֵ*/
//��һ�����ݰ��������λ��
u16 axAnalizePosition(u8 ID, u8 *pack)
{
	u8 i = 0;
	u32 check = 0;
	
	while(1)
	{
		if(pack[0] == 0x55 && pack[1] == 0x55 && pack[2] == ID && pack[3] == 5 && pack[4] == LOBOT_SERVO_POS_READ)
		{
			break;
		}
		else
		{
			pack++;
			i++;
			if(i >= 30){
		//		printf("time error\r\n");
				return 0x01ff;
			}
		}
	}

/*	for(i = 2; i <= 6; i++)
	{
		check += pack[i];
	}
	check = (~check) & 0xff;*///�ϵ�AX-12�����У�鷽ʽ
	check=LobotCheckSum(pack);
	
	if((u8)check != pack[7])	//���У��λ��ע���������λ��
	{
	//	printf("check error\r\n");
		return 0x01ff;
	}
	
	return pack[5] | (pack[6] << 8);	
}
/*
int LobotSerialServoReadPosition(uint8_t id)
{
  int ret;
  uint8_t buf[6];

  buf[0] = buf[1] = LOBOT_SERVO_FRAME_HEADER;
  buf[2] = id;
  buf[3] = 3;
  buf[4] = LOBOT_SERVO_POS_READ;
  buf[5] = LobotCheckSum(buf);
	
	LobotSerialWrite(buf, 6);
	
	ret = LobotSerialMsgHandle();
  return ret;
}*/
/*
int LobotSerialServoReadPosition(uint8_t id)
{
  int ret;
  uint8_t buf[6];

  buf[0] = buf[1] = LOBOT_SERVO_FRAME_HEADER;
  buf[2] = id;
  buf[3] = 3;
  buf[4] = LOBOT_SERVO_POS_READ;
  buf[5] = LobotCheckSum(buf);
	
	LobotSerialWrite(buf, 6);
	
	ret = LobotSerialMsgHandle();
  return ret;
}*/


//��ȡLX-16�Ķ��λ��
u16 axGetPosition(u8 ID)
//�ĳ���LX16
{
	u8 param[8] = {0};
	u8 i = 0;
	u16 position = 0;
	//param[0] = P_PRESENT_POSITION_L; 	
	//param[1] = 2; 	
	while(1)
	{
		AX_RXBUF_CLR;//����һ�»�����
		axSendPack(ID, LOBOT_SERVO_POS_READ, param, 0);//����ȥһ֡�������ص�����
		delay_ms(2);
		position = axAnalizePosition(ID, AX_RXBUF);
		if(position != 0xffff)
			break;
		else
		{
			i++;
			if(i >= 10)
				return 0xffff;
		}
	}
	return position;
}



//����������
u16 axAnalizePing(u8 ID, u8 *pack)
{
	u8 i = 0;
	u32 check = 0;
	
	while(1)
	{
		if(pack[0] == 0xff && pack[1] == 0xff && pack[2] == ID && pack[3] == 2)
		{
			break;
		}
		else
		{
			pack++;
			i++;
			if(i >= 30)
				return 0xff;
		}
	}

	for(i = 2; i <= 4; i++)
	{
		check += pack[i];
	}
	check = (~check) & 0xff;
	
	if((u8)check != pack[7])	//���У��λ
	{
		return 0x01ff;
	}
	
	return pack[5] | (pack[6] << 8);	
}

u8 axPing(u8 ID)
{
	
//	u8 param[16] = {0};
		
	AX_RXBUF_CLR;
	axSendPack(ID, AXORDER_PING, (u8*)0, 0);
	delay_ms(2);	
  return 1;	
}

//LX-16��ID����
/*void LobotSerialServoSetID(u8 oldID, u8 newID)
{
	u8 param[2];
  param[0] = newID;
  axSendPack(oldID,LOBOT_SERVO_ID_WRITE,param,1);
}*/


