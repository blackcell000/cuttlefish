#ifndef FILE_H
#define FILE_H

#include "hardware.h"
#include "device.h"
#include "app.h"

/*�ļ�ϵͳָ�*/
#define ReplyActionName			0x15	//�ϴ�΢�����ļ���
#define ReplyOrderName			0x16	//�ϴ����������ļ���
#define ReplyOrder				0x17	//�ϴ����������ļ�

/*�ļ�ϵͳ�ظ���ָ�*/
#define GOOD_MESSAGE			0x18	//ִ������ɹ�
#define BAD_MESSAGE				0x19	//ִ������ʧ��

/*���ݰ�����ˢ��Ƶ��*/
#define RefreshNum            	5  		//ÿ5��ˢ��һ�»�����

/*�ļ������ϴ���������*/
typedef enum
{
	FileSend_OK,						//���ݰ����ͳɹ�
	FileSend_LengthError,				//���ݰ��������
	FileSend_CheckError,				//���ݰ�У��λ����
	FileSend_UnknownError				//δ֪����
}FSS;									//FileSendState

/*�ļ�����״̬��*/
typedef enum
{
	Download_Free,						//���ؿ���
	Action_Sending,						//��������΢�����ļ�
	Order_Sending						//�������ض��������ļ�
}FDS;									//FileDownloadState


FSS SendFileCommand(u8 INS_TYPE,u16 length,volatile u8 *data);	
														//�ļ����ݷ��ͺ���
void FileSystemInit(void);								//�ļ������ϴ�ϵͳ��ʼ��
void UploadActionName_Function(void);					//�ϴ�����΢����
void DownloadActionFile_Function(void);					//����΢�����ļ�
void ExecuteActionFile_Function(void);					//ִ��ָ�����Ƶ�΢����
void DownloadSequence_Function(void);					//���ض��������ļ�
void ExecuteSequenceFile_Function(void);				//ִ�ж��������ļ�
void ForceExecuteActionFile_Function(u8 *buf, u8 len); 	//ǿ��ִ��ĳ�������ļ�														
void MoveServo_Function(void);							//�ƶ�ָ�����
void DeleteActionFile_Function(void);					//ɾ��ָ�������ļ�
void UploadOrderName_Function(void);					//�ϴ����ж��������ļ���
void UploadOrderFile_Function(void);					//�ϴ�ָ�����������ļ�
void DeleteOrderFile_Function(void);					//ɾ��ָ�����������ļ�
void ReNameActionFile_Function(void);					//������ָ�������ļ�
void EnableAllServo(void);
void ForceExecuteSequenceFile_Function(u8 *buf, u8 len);//ǿ��ִ�������ļ�
u8 exf_getfree(u8 *drv,u32 *total,u32 *free);
/*�ӿ��ͺ���*/
FRESULT ExcuteOrder(const TCHAR *path);					//ִ��ָ���Ķ��������ļ�
FRESULT ExcuteAction(const TCHAR *path,u8 first_or_not);
u8* PRINT_FREUSTL(FRESULT res);






#endif

