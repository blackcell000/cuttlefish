
#include "protocal.h"
#include "file.h"
#include "string.h"
#include "config.h"


STATE receive_state;			//״̬��ָ��
COMMAND Command_Buffer;			//����������ϢΪָ��
u16 Present_DataLength; 		//��ǰ�����ֽڳ��ȣ����ڼ���
u8 GetProtocal = 0;				//���յ�ָ���־λ

/*�û����崮�ڷ��ͺ���*******************************************/
void ptc_usartx_senddat(u8 *buf, u8 len)
{
	usart1_senddat(buf, len);
}
/****************************************************************/

int rand = 0;


void ptcClearCmdbuf(PCOMMAND cmd)
{
	memset((void*)cmd, 0, sizeof(COMMAND));
	cmd->InsType = 0xff;
}


//����λ��������Ϣ����
void MESSAGE_Reply(u8 INS_TYPE, u16 length, volatile u8 *data)
{
	u32 check = 0;					//У�������
	u8 buf[256] = { 0 };		//�������ݻ���

	u16 i = 0;					//���ݷ��ͼ�����
	extern u8 RAND;

	if(INS_TYPE == BAD_MESSAGE)
		LED0 = ~LED0;
	
	buf[0] = PACK_HEAD;			//����ͷ							
//	buf[1] = (INS_TYPE == RECEIVE_OK) ? Command_Buffer.Random : (rand++);
	buf[1] = Command_Buffer.Random;
	//����ǻظ�OKָ��ͷ��������, �����������һ����������ͳ�ȥ		
	buf[2] = COMPUTER_ID; 		//������(�����˱��)
	buf[3] = ROBOT_ID; 			//�ӻ���(��λ�����)	
	buf[4] = length >> 8; 		//���ݳ��ȸ�λ		
	buf[5] = length % 256; 		//���ݳ��ȵ�λ	
	buf[6] = INS_TYPE; 			//��������

	for (i = 0; i <= length - 1; i++)
		buf[7 + i] = data[i];

	for (i = 1; i <= length + 6; i++)
		check += buf[i];
	check = check % 255;

	buf[7 + length] = check;

	ptc_usartx_senddat(buf, length + 8);
}

//��������Ϣ����Ϊָ��
void ptcGetCommand(u8 byte_data)
{
	u8 *NONE;					

	switch (receive_state)
	{
		case STATE_BEGIN:							//**������ʼλ
			if (byte_data == PACK_HEAD)
			{
				Command_Buffer.check = PACK_HEAD;
				Present_DataLength = 0;
				receive_state = STATE_RAMDOM;
			}
			break;

		case STATE_RAMDOM:
			Command_Buffer.Random = byte_data;
			Command_Buffer.check = (Command_Buffer.check + byte_data) % 255;
			receive_state = STATE_SENDERID;
			break;

		case STATE_SENDERID:						//**������
			if (byte_data == COMPUTER_ID)			//������յ�������
			{
				Command_Buffer.check = (Command_Buffer.check + byte_data) % 255;
				receive_state = STATE_RECEIVERID;
			}
			else
				receive_state = STATE_BEGIN;		//����ʧ��״̬����λ
			break;

		case STATE_RECEIVERID:						//**�ӻ���
			if (byte_data == ROBOT_ID
			|| byte_data == BRODCAST_SIGNAL)	//�㲥�ź�
			{
				Command_Buffer.check = (Command_Buffer.check + byte_data) % 255;
				receive_state = STATE_LENGTH_H;
			}
			else
				receive_state = STATE_BEGIN;		//����ʧ��״̬����λ
			break;

		case STATE_LENGTH_H:						//**���ݳ��ȸ�λ
			Command_Buffer.Length = byte_data;
			Command_Buffer.Length <<= 8;
			Command_Buffer.check = (Command_Buffer.check + byte_data) % 255;
			receive_state = STATE_LENGTH_L;
			break;

		case STATE_LENGTH_L:						//**���ݳ��ȵ�λ
			Command_Buffer.Length += byte_data;
			if (Command_Buffer.Length < MAX_DATA_LENGTH)
			{
				Command_Buffer.check = (Command_Buffer.check + byte_data) % 255;
				receive_state = STATE_INSTYPE;
			}
			else
			{
				receive_state = STATE_BEGIN;		//����ʧ��״̬����λ
				MESSAGE_Reply(LENGTH_Error, 0, NONE);
													//�ظ���λ����������
			}
			break;

		case STATE_INSTYPE:							//**������������
			Command_Buffer.InsType = byte_data;
			Command_Buffer.check = (Command_Buffer.check + byte_data) % 255;
			receive_state = STATE_DATA;
			if (Command_Buffer.Length == 0)			//������ݳ��ȵ���0����������������ֱ��У��
				receive_state = STATE_CHECK;
			break;

		case STATE_DATA:							//**��������
			if (Present_DataLength < Command_Buffer.Length)
			{
				Command_Buffer.Data[Present_DataLength++] = byte_data;
				Command_Buffer.check = (Command_Buffer.check + byte_data) % 255;
			}
			if (Present_DataLength == Command_Buffer.Length)
			{										//�������
				Command_Buffer.check = Command_Buffer.check % 255;
				receive_state = STATE_CHECK;
			}
			break;

		case STATE_CHECK:							//**�������У��λ
			if (byte_data == Command_Buffer.check % 255)
			{
				if(Command_Buffer.InsType != RECEIVE_OK
					&& Command_Buffer.InsType != LENGTH_Error
					&& Command_Buffer.InsType != CHECK_Error
					&& Command_Buffer.InsType != 0xff)
					GetProtocal=1;		
					receive_state = STATE_BEGIN;	//���ճɹ�״̬����λ				
			}
			else
			{
				receive_state = STATE_BEGIN;		//����ʧ��״̬����λ
				MESSAGE_Reply(CHECK_Error, 0, 0);	//�ظ���λ����������								
				Command_Buffer.check = 0;
			}
			break;

		default:
			receive_state = STATE_BEGIN;			//����ʧ��״̬����λ
			break;
	}
	
					//�жϽ��յ�ָ��
}

extern u16 InitialPosition[SERVO_NUMBER];
extern const u8 SERVO_NAME[SERVO_NUMBER][100];	//������.h�ļ��ж���Ļ����˶����	
u8 Ax_Position[100];
u8 ax_receive;	
u16 ax_long_receive[100];

//����λ������������ָ����з���
void pctCommandClassify(COMMAND command)
{

	u8 INS = command.InsType;		//��ȡ��������
	u8 *NONE = 0;
	u8 i;
	switch (INS)
	{

		case GetServoConnection:	//��ȡ��λ����Ϣ
			Reply_GetServoConnection();	//�ظ���������Ϣ
			break;

		case GetServoPosition:		//������ֶ���ĵ�ǰλ��
				MESSAGE_Reply(RECEIVE_OK,0,NONE);	//�ظ���λ�����ճɹ�
				Ax_Position[0]=Digital;
				Ax_Position[1]=SERVO_NUMBER;
				for(i = 1; i <= SERVO_NUMBER; i++)
				{
						ax_long_receive[i] = axGetPosition(i);
						Ax_Position[3*i-1]=i;
						Ax_Position[3*i]=ax_long_receive[i]>>8;
						Ax_Position[3*i+1]=ax_long_receive[i];
						if(ax_long_receive[i]==0xFFFF)
							MESSAGE_Reply(BAD_MESSAGE,0,NONE);	//�ظ���λ��������ʹ���
				}
				SendFileCommand(ReplyServoPosition,SERVO_NUMBER*3+2,(u8*)Ax_Position);

		
			break;

		case EnableDigitalServo:	//ʹ��ȫ�����ֶ��
				if(SERVO_TYPE==Digital)	//ֻ�����ֶ���Ч
					 EnableAllServo();
				else
					MESSAGE_Reply(BAD_MESSAGE,0,NONE);	//�ظ���λ��������ʹ���
			break;


		case DisableDigitalServo:	//��ʹ��ȫ�����ֶ��
				if(SERVO_TYPE==Digital)	//ֻ�����ֶ���Ч
				{
					int i;
					for(i=1;i<=SERVO_NUMBER;i++)
					{
						ax_receive=axTorque(i,AX_OFF) ;
						if(ax_receive==0xFF)
							MESSAGE_Reply(BAD_MESSAGE,0,NONE);	//�ظ���λ��������ʹ���
					}
				}
				else
					MESSAGE_Reply(BAD_MESSAGE,0,NONE);	//�ظ���λ��������ʹ���
			break;

		case UploadActionName:		//�ϴ�FLASH��΢�����ļ���
				UploadActionName_Function();
			break;

		case DownloadActionFile:	//����ָ���Ķ����ļ�
				DownloadActionFile_Function();
			break;

		case ExecuteActionFile:		//ִ��ָ���ļ�����΢�����ļ�
				ExecuteActionFile_Function();
			break;

		case DownloadOrder:			//����ָ���ļ����Ķ�������
				DownloadSequence_Function();
			break;

		case ExecuteOrderFile:		//ִ��ָ���ļ����Ķ�������
				ExecuteSequenceFile_Function();
			break;

		case MoveServo:				//�ƶ�ָ�����(���ڶ�̬����)
				MoveServo_Function();
			break;

		case DeleteActionFile:		//ɾ��ָ�������ļ�
				DeleteActionFile_Function();
			break;

		case UploadOrderName:		//�ϴ����ж��������ļ�
				UploadOrderName_Function();
			break;

		case UploadOrderFile:		//�ϴ����������ļ�����	
				UploadOrderFile_Function();
			break;

		case DeleteOrderFile:		//ɾ��ָ�����������ļ�
				DeleteOrderFile_Function();
			break;

		default:
				MESSAGE_Reply(BAD_MESSAGE,0,NONE);	//�ظ���λ��ָ�����ʹ���
			break;
	}
}

void Reply_GetServoConnection(void)
{
	u8 i;
	FSS fss;										//�ļ����ݷ���״̬
	u8 Data[100];								//�������͵�����
	u8 Finish[2]={0xFF,0xFF};		//��������Ϣλ
	u8 ax_num;
	
	const u8 ServoOffLine[]="Some Servo Lost Connection.";	//������ߴ�����Ϣ
	const u8 ServoTypeError[]="No Such Servo Type.";				//������ʹ�����Ϣ
	
	Data[0]=SERVO_TYPE;					//д��������
	Data[1]=SERVO_NUMBER;				//д������Ŀ
	MESSAGE_Reply(RECEIVE_OK,0,NULL);	//�ظ���λ�����ճɹ�
	
	/*switch(SERVO_TYPE)	//��Բ�ͬ���͵Ķ��Ҫ�м���
	{
		case Digital:	//����UBT12HB�ͺŶ��
			for(ax_num=1;ax_num<=SERVO_NUMBER;ax_num++)
				if(AX_Pin(ax_num)!=0x01)
				{
					SendFileCommand(BAD_MESSAGE,27,(u8*)ServoOffLine);
					return;
				}
			break;
				
		case Simulate_LD2701:	//����LD1602����޷�����
			break;
		
		default:	//����������ض���ͺŴ���
			SendFileCommand(BAD_MESSAGE,19,(u8*)ServoTypeError);
			break;
	}*/
	fss=SendFileCommand(ReplyServoConnection,2,Data);
	if(fss!=FileSend_OK)	//���û���͵���λ������������
		return;
	
	for(i=0;i<SERVO_NUMBER;i++)	//д������̬
	{
		Data[0]=i+1;										//�����
		Data[1]=(*(InitialPosition+i))/256;	//�Ƕȸ�λ
		Data[2]=(*(InitialPosition+i))%256;	//�Ƕȵ�λ
		Data[3]='\0';										//���ַ�������λ����Ϊ�ַ�������ֹ
		strcat((char*)Data,(char*)SERVO_NAME[i]);	//���������ַ���
		fss=SendFileCommand(ReplyServoConnection,strlen((char*)Data),Data);
		if(fss!=FileSend_OK)	//���û���͵���λ������������
			return;
	}
	fss=SendFileCommand(ReplyServoConnection,2,Finish);
	if(fss!=FileSend_OK)	//���û���͵���λ������������
		return;
}




