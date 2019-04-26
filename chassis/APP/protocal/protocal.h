#ifndef PROTOCAL_H
#define PROTOCAL_H

#include "device.h"




#define MAX_DATA_LENGTH			200 	//������ݳ���
//#define EN_USART3_RX			1		//ʹ��(1)/��ֹ(0)����3����

#define PACK_HEAD 				0xFF	//��ʼλΪ0xFF

//�������
#define COMPUTER_ID				0x05	//������յ��������

#define BRODCAST_SIGNAL			0x5F	//�㲥�ź�



//��λ��ͨѶ״̬��
typedef enum tagSTATE
{
	STATE_BEGIN,		//��ʼλ
	STATE_RAMDOM,		//���У��λ
	STATE_SENDERID,		//������
	STATE_RECEIVERID,	//�ӻ���
	STATE_LENGTH_H,		//���ݳ��ȸ�λ
	STATE_LENGTH_L,		//���ݳ��ȵ�λ
	STATE_INSTYPE,		//ָ������
	STATE_DATA,			//ָ��
	STATE_CHECK			//У��λ
}STATE;


typedef struct tagCOMMAND
{
	u8	Random;								//���У��λ(��һλ����λ����������ʱû�õ�)
	u16 Length;								//�����
	u8 	InsType;							//ָ������
    u8  Data[MAX_DATA_LENGTH];				//����
	u16 check;								//У��
}COMMAND, *PCOMMAND;
extern COMMAND Command_Buffer;			//����������ϢΪָ��

void MESSAGE_Reply(u8 INS_TYPE,u16 length,volatile u8 *data);


#define SERVO_TYPE					Digital	//�������

/*�����������*/
#define Digital						0x01	//UBT12HB���ֶ��
#define Simulate_LD2701				0x02	//LD1602ģ����


/*��λ���ظ���ָ�*/
#define RECEIVE_OK					0x00	//���ճɹ�
#define LENGTH_Error				0x01	//���ݳ��ȳ������ֵ
#define CHECK_Error					0x02	//У��λ����ȷ

/*��λ�������ָ�*/
#define GetServoConnection			0x03	//��ȡ��λ����Ϣ(����������������ͣ������)
#define GetServoPosition			0x04	//������ֶ���ĵ�ǰλ��(�����ֶ������)
#define EnableDigitalServo			0x05	//ʹ��ȫ�����ֶ��(�����ֶ������)
#define DisableDigitalServo			0x06	//��ʹ��ȫ�����ֶ��(�����ֶ������)
#define MoveServo					0x07	//�ƶ�ָ�����(���ڶ�̬����)

/*�ļ�ϵͳ�����ļ�ָ�*/
#define DownloadActionFile			0x08	//����ָ���Ķ����ļ�
#define ExecuteActionFile			0x09	//ִ��ָ���ļ�����΢�����ļ�
#define DeleteActionFile			0x0A	//ɾ��ָ�������ļ�
#define ReNameActionFile			0x0B	//�����������ļ�
#define UploadActionName			0x0C	//�ϴ�FLASH��΢�����ļ���

/*�ļ�ϵͳ�����ļ�ָ�*/
#define DownloadOrder				0x0D	//����ָ���Ķ��������ļ�
#define ExecuteOrderFile			0x0E	//ִ��ָ���ļ����Ķ�������
#define UploadOrderName				0x0F	//�ϴ����������ļ���
#define UploadOrderFile				0x10	//�ϴ����������ļ�
#define RenameOrderFile				0x11	//�����������ļ�
#define DeleteOrderFile				0x12	//ɾ��ָ�����������ļ�

/*��λ�������ظ���ָ�*/
#define ReplyServoConnection		0x13	//�ظ���λ����Ϣ(������ͣ����������ʼ״̬)
#define ReplyServoPosition			0x14	//�ظ����ֶ����ǰλ��(�����ֶ������)



//��������Ϣ����Ϊָ��
void ptcGetCommand(u8 byte_data);

//ָ�����
void pctCommandClassify(COMMAND Command_Buffer);	

//�ظ���λ������
void Reply_GetServoConnection(void);		

//����λ��������Ϣ����
void MESSAGE_Reply(u8 INS_TYPE, u16 length, volatile u8 *data);


extern COMMAND Command_Buffer;
extern u8 GetProtocal;
void ptcClearCmdbuf(PCOMMAND cmd);

FRESULT ExcuteAction(const TCHAR *path,u8 first_or_not);
FRESULT ExcuteOrder(const TCHAR *path);

#endif
