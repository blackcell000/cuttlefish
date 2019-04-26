#include "file.h"
#include "string.h"
#include "config.h"

FDS fds=Download_Free;			//����״̬��Ϊ����
FATFS fs;


/*�ļ����ݷ��ͺ���*/
FSS SendFileCommand(u8 INS_TYPE,u16 length,volatile u8 *data)
{
	int i;
	extern u8 RAND;
	for(i=0;i<10;i++)	//����ط�ʮ��
	{
		MESSAGE_Reply(INS_TYPE,length,data);	//��������
		delay_ms(20);
		if(Command_Buffer.InsType==RECEIVE_OK/*&&Command_Buffer.Random==RAND*/)
			break;
		//delay_ms(15);
	}
	switch(Command_Buffer.InsType)			//����λ���ظ�ָ�����ʶ��
	{
		case RECEIVE_OK:									//��λ�����ճɹ�
			ptcClearCmdbuf(&Command_Buffer);
			return FileSend_OK;
		case LENGTH_Error:								//���ݳ��ȳ���һ�㲻̫����
			ptcClearCmdbuf(&Command_Buffer);			
			return FileSend_LengthError;
		case CHECK_Error:									//У��λ����ûʲô�Ѱ취
			ptcClearCmdbuf(&Command_Buffer);
			return FileSend_CheckError;
		default:													//����δ֪������TM�ͺ�������
			ptcClearCmdbuf(&Command_Buffer);
			return FileSend_UnknownError;
	}
}

/*���ļ������ϴ�ϵͳ�ĳ�ʼ��*/
void FileSystemInit(void)
{
	DIR dir;			//Ŀ¼�ṹ��
	FRESULT res;	//ͨ�ý������
	fds=Download_Free;			//����״̬��Ϊ����
	SD_Init();							//SD����ʼ��			 
	f_mount(&fs,"/",1);			//ǿ�ƹ���SD��
	
	/*�ж��Ƿ����ActionĿ¼*/
	res=f_opendir(&dir,"/Action");
	if(res==FR_NO_PATH)
		res=f_mkdir("/Action");
	else
		res=f_closedir(&dir);
	
	/*�ж��Ƿ����OrderĿ¼*/
	res=f_opendir(&dir,"/Order");
	if(res==FR_NO_PATH)
		res=f_mkdir("/Order");
	else
		res=f_closedir(&dir);
}

/*�ϴ�SD��������΢�����ļ���*/
void UploadActionName_Function(void)
{
	FRESULT res;	//ͨ�ý������
	FILINFO fno;	//�ļ�״̬�ṹ��
	FSS fss;			//�ļ����ݷ���״̬
	TCHAR *fn;		//�ļ���
	DIR dir;			//Ŀ¼�ṹ��
	u8 Finish[2]={0xFF,0xFF};	//��������Ϣλ
	u8 FileNumber=0;			//�ļ���
	
	MESSAGE_Reply(RECEIVE_OK,0,NULL);	//�ظ���λ�����ճɹ�
	res=f_opendir(&dir,"/Action");	//��ActionĿ¼�����ļ�����
	while(1)
	{
		res=f_readdir(&dir,&fno);
		fn=fno.fname;		//ͬʱ֧�ֳ����ļ���
		if((*fn)==NULL)
			break;
		FileNumber++;
	}
	res=f_closedir(&dir);
	
	res=f_opendir(&dir,"/Action");	//��ActionĿ¼
	if(res==FR_OK)	//��ActionĿ¼�ɹ�
	{
		fss=SendFileCommand(ReplyActionName,1,&FileNumber);	//����λ�����Ͱ�ͷ
		if(fss!=FileSend_OK)	//���û���͵���λ������������
			return;
		while(1)
		{
			res=f_readdir(&dir,&fno);	//��ȡActionĿ¼
			if(res!=FR_OK)	//��ȡAcitonĿ¼�ļ���ʧ����ȡ����ζ�ȡ
			{
				fss=SendFileCommand(BAD_MESSAGE,0,(u8*)(&res));	//����λ�����ش�������
				if(fss!=FileSend_OK)	//���û���͵���λ������������
					return;
				break;																//ȡ����ȡ
			}
			fn=fno.fname;		//ͬʱ֧�ֳ����ļ���
			if((*fn)==NULL)	//��ȡ�������ѭ��
				break;
			fss=SendFileCommand(ReplyActionName,strlen(fn),(u8*)fn);	//����λ�������ļ���
			if(fss!=FileSend_OK)	//���û���͵���λ������������
				return;
		}
		
		fss=SendFileCommand(ReplyActionName,2,Finish);	//���Ͱ�β
		if(fss!=FileSend_OK)	//���û���͵���λ������������
			return;
	}
	else												//����δ֪��������λ�����ʹ���ֵ
	{
		fss=SendFileCommand(BAD_MESSAGE,1,(u8*)(&res));
		if(fss!=FileSend_OK)	//���û���͵���λ������������
			return;
	}
}

/*����ָ���Ķ����ļ�*/
void DownloadActionFile_Function(void)	//��������ı���ϵͳ������
{
		u8 debugnum=0;
	FRESULT res;				//ͨ�ý������
	FSS fss;						//�ļ����ݷ���״̬?
	u8 *NONE;						//ֻ���ڷ�����Ϣռλ
	UINT bw;						//ûʲô�õ�һ���ֽڣ�д�ļ�����Ҫ��
	u8 ActBuffer[100]={"/Action/"};		//�ļ��������ֽ�
	u8 i,p=0;								//������
	
	const u8 CommandNumberError[26]="Too Many Command Number !";	//������
	
	static FIL file_Action;			//�ļ�����ṹ��
	static u16 ADN;							//��¼�����ļ���������(ActionDownloadNumber)
	static u16 ADC;							//�����ļ�������(ActionDownloadCount)
	static u8 random_check;
	switch(fds)	//�ж�����״̬
	{
		case Download_Free:		//���ؿ���״̬
			f_sync(&file_Action);				//ˢ��һ��������
			ADN=Command_Buffer.Data[0]*256;	//��¼������
			ADN+=Command_Buffer.Data[1];
			for(i=2;i<Command_Buffer.Length;i++)				//�ļ���ת�浽�ַ�����
				ActBuffer[i+6]=Command_Buffer.Data[i];
			ActBuffer[i+6]='\0';												//���һλ�ǳɿ��ַ�
			ADC=0;						//���ؼ���������
			random_check=Command_Buffer.Random+1;
			res=f_open(&file_Action,(const TCHAR*)ActBuffer,FA_CREATE_ALWAYS|FA_WRITE);
																	//���ļ��������ھʹ�����
			MESSAGE_Reply(RECEIVE_OK,0,NONE);	//�ظ���λ�����ճɹ�
			if(res==FR_OK)
				fds=Action_Sending;					//״̬����Ϊ���ն���
			else
				fss=SendFileCommand(BAD_MESSAGE,strlen((const char*)PRINT_FREUSTL(res)),PRINT_FREUSTL(res));
			break;
		
		case Action_Sending:	//���ն���״̬
			
			if(Command_Buffer.Random!=random_check)	//�ȼ������У��λ�Ƿ��д�
				return;
			random_check=Command_Buffer.Random+1;
			if(Command_Buffer.Random==0xFF)
				random_check=0;
			
			if(ADC>=ADN)	//�����������ﵽ������
			{							//�жϸ������ǲ��ǽ���λ
				res=f_close(&file_Action);			//�ر��ļ�
				fds=Download_Free;						//״̬����λ
				MESSAGE_Reply(RECEIVE_OK,0,NONE);	//�ظ���λ�����ճɹ�
				if(Command_Buffer.Data[0]==0xFF&Command_Buffer.Data[1]==0xFF)	//����ǽ���λ
				{
					if(res==FR_OK)
					{
						fss=SendFileCommand(GOOD_MESSAGE,0,NONE);	//����λ�����ʹ����ļ��ɹ�
						if(fss!=FileSend_OK)	//���û���͵���λ������������
							return;
					}
					else	
					{						//����δ֪��������λ�����ʹ���ֵ
						fss=SendFileCommand(BAD_MESSAGE,strlen((const char*)PRINT_FREUSTL(res)),PRINT_FREUSTL(res));
						res=f_unlink((const TCHAR*)ActBuffer);	//ɾ���ļ� 
						fss=SendFileCommand(BAD_MESSAGE,26,(u8*)CommandNumberError);	//����λ�����ʹ����ļ�ʧ��
						if(fss!=FileSend_OK)	//���û���͵���λ������������
							return;
					}
				}
				else
				{
					res=f_unlink((const TCHAR*)ActBuffer);	//ɾ���ļ�
					fss=SendFileCommand(BAD_MESSAGE,26,(u8*)CommandNumberError);	//����λ�����ʹ����ļ�ʧ��
					if(fss!=FileSend_OK)	//���û���͵���λ������������
						return;
				}
			}
			else																			//����������û�ﵽ����
			{
				if(Command_Buffer.Data[0]==0xFF&Command_Buffer.Data[1]==0xFF)	//����ǽ���λ
				{
					res=f_close(&file_Action);			//�ر��ļ�
					res=f_unlink((const TCHAR*)ActBuffer);	//ɾ���ļ�
					MESSAGE_Reply(RECEIVE_OK,0,NONE);	//�ظ���λ�����ճɹ�
					fss=SendFileCommand(BAD_MESSAGE,strlen((const char*)PRINT_FREUSTL(res)),PRINT_FREUSTL(res));
					if(fss!=FileSend_OK)	//���û���͵���λ������������
						return;
				}
				else	//���ǽ���λ
				{
					//SendFileCommand(BAD_MESSAGE,1,&random_check);
					//SendFileCommand(BAD_MESSAGE,0,NULL);/

					res=f_lseek(&file_Action,ADC*180);	//�������ַƫ����
					res=f_write(&file_Action,Command_Buffer.Data,Command_Buffer.Length,&bw);	//д��һ����������
					//usart2_senddat(Command_Buffer.Data,Command_Buffer.Length);  //ʹ�ô���3����һ������
					/*if(p<RefreshNum)   
					{
						p++;
						f_sync(&file_Action);
					}
					else p=0;*/
					MESSAGE_Reply(RECEIVE_OK,0,NONE);	//�ظ���λ�����ճɹ�
					if(res!=FR_OK)	//��ط���Ҫ���Ͼ���������λ���������İ������⣬���������͵�ַƫ�����Բ���
						fss=SendFileCommand(BAD_MESSAGE,strlen((const char*)PRINT_FREUSTL(res)),PRINT_FREUSTL(res));
				}
			}
			ADC++;
			//if(ADC==23)
				//debugnum=0;
				
			break;
			
		default:
			break;
	}
}

/*ִ��ָ���Ķ����ļ�*/
void ExecuteActionFile_Function(void)	//���������usart.c�е�changeposition�����е�ë��
{
	FRESULT res;					//ͨ�ý������
	FSS fss;						//�ļ����ݷ���״̬
	u8 *NONE;						//ֻ���ڷ�����Ϣռλ
	//Command_Buffer.Data[Command_Buffer.Length]='\0';	//���һλ�����ַ�
	//res=ExcuteAction((const TCHAR*)(Command_Buffer.Data));	//ִ�ж��������ļ�
	/*�ⲿ���Ҿ��ÿ��Բ��öԶ�������жϣ���Ϊ��ExcuteAction������жԶ�����ж�*/
	switch(SERVO_TYPE)
	{
		case Digital:
			Command_Buffer.Data[Command_Buffer.Length]='\0';	//���һλ�����ַ�
			res=ExcuteAction((const TCHAR*)(Command_Buffer.Data),0);	//ִ�ж����ļ�
			break;
		case Simulate_LD2701:
			Command_Buffer.Data[Command_Buffer.Length]='\0';	//���һλ�����ַ�
			res=ExcuteAction((const TCHAR*)(Command_Buffer.Data),0);	//ִ�ж����ļ�
			break;
		default:
			break;
	}
	/*
	Command_Buffer.Data[Command_Buffer.Length]='\0';	//���һλ�����ַ�
	res=ExcuteAction((const TCHAR*)(Command_Buffer.Data));	//ִ�ж����ļ�
	*/
	if(res==FR_OK)
	{
		fss=SendFileCommand(GOOD_MESSAGE,0,NONE);
		if(fss!=FileSend_OK)	//���û���͵���λ������������
			return;
	}
	else	
	{						//����δ֪��������λ�����ʹ���ֵ
		fss=SendFileCommand(BAD_MESSAGE,1,(u8*)(&res));
		if(fss!=FileSend_OK)	//���û���͵���λ������������
			return;
	}
}

/*���ض��������ļ�*/
void DownloadSequence_Function(void)
{
	FRESULT res;				//ͨ�ý������
	FSS fss;						//�ļ����ݷ���״̬
	u8 *NONE;						//ֻ���ڷ�����Ϣռλ
	static UINT bw;			//д�ļ�����Ҫ��
	u8 OrdBuffer[100]={"/Order/"};		//�ļ��������ֽ�
	u8 i;								//������
	static FIL file_Order;	//�ļ�����ṹ��
	static u16 ODN;					//��¼�����ļ���������(OrderDownloadNumber)
	static u16 ODC;					//�����ļ�������(OrderDownloadCount)
	static u16 OAO;					//д���ļ���ַƫ����(OrderAddressOffset)
	switch(fds)	//�ж�����״̬
	{
		case Download_Free:		//���ؿ���״̬
			ODN=Command_Buffer.Data[0]*256;	//��¼������
			ODN+=Command_Buffer.Data[1];
			for(i=2;i<Command_Buffer.Length;i++)				//�ļ���ת�浽�ַ�����
				OrdBuffer[i+5]=Command_Buffer.Data[i];
			ODC=0;						//���ؼ���������
			OrdBuffer[i+5]='\0';				//���һλ���ַ�
			OAO=0;						//��ַƫ������Ϊ0
			res=f_open(&file_Order,(const TCHAR*)OrdBuffer,FA_CREATE_ALWAYS|FA_WRITE);
																	//���ļ��������ھʹ���������+2��ԭ����ǰ��λ��������
			MESSAGE_Reply(RECEIVE_OK,0,NULL);	//�ظ���λ�����ճɹ�
			fds=Order_Sending;					//״̬����Ϊ���ն���
			break;
		
		case Order_Sending:	//���ն���״̬
			if(ODC>=ODN)	//�����������ﵽ������
			{																					//�жϸ������ǲ��ǽ���λ
				res=f_close(&file_Order);			//�ر��ļ�
				fds=Download_Free;						//״̬����λ
				MESSAGE_Reply(RECEIVE_OK,0,NULL);	//�ظ���λ�����ճɹ�
				if(Command_Buffer.Data[0]==0xFF&Command_Buffer.Data[1]==0xFF)	//����ǽ���λ
				{
					if(res==FR_OK)
					{
						fss=SendFileCommand(GOOD_MESSAGE,0,NONE);	//����λ�����ʹ����ļ��ɹ�
						if(fss!=FileSend_OK)	//���û���͵���λ������������
							return;
					}
					else	
					{						//����δ֪��������λ�����ʹ���ֵ
						fss=SendFileCommand(BAD_MESSAGE,strlen((const char*)PRINT_FREUSTL(res)),PRINT_FREUSTL(res));
						if(fss!=FileSend_OK)	//���û���͵���λ������������
							return;
					}
				}
				else
				{
					fss=SendFileCommand(BAD_MESSAGE,0,NONE);	//����λ�����ʹ����ļ�ʧ��
					if(fss!=FileSend_OK)	//���û���͵���λ������������
						return;
				}
			}
			else																			//����������û�ﵽ����
			{
				Command_Buffer.Data[Command_Buffer.Length]=' ';					//�Կո�ָ�
				res=f_lseek(&file_Order,OAO);	//�������ַƫ����
				if(res==FR_OK)	//��ط���Ҫ���Ͼ���������λ���������İ������⣬���������͵�ַƫ�����Բ���
					res=f_write(&file_Order,Command_Buffer.Data,Command_Buffer.Length+1,&bw);	//д��һ����������
				MESSAGE_Reply(RECEIVE_OK,0,NULL);	//�ظ���λ�����ճɹ�
			}
			ODC++;
			OAO+=(Command_Buffer.Length+1);	//��һ����Ϊ�пո�
			break;
			
		default:
			break;
	}
}

/*ִ�ж��������ļ�*/
void ExecuteSequenceFile_Function(void)
{
	FRESULT res;			//ͨ�ý������
	FSS fss;					//�ļ����ݷ���״̬
	u8 *NONE;					//ֻ���ڷ�����Ϣռλ
	MESSAGE_Reply(RECEIVE_OK,0,NONE);	//�ظ���λ�����ճɹ�
	Command_Buffer.Data[Command_Buffer.Length]='\0';	//���һλ�ÿ�
	res=ExcuteOrder((TCHAR*)(Command_Buffer.Data));		//ִ�ж�������
	if(res==FR_OK)
	{
		fss=SendFileCommand(GOOD_MESSAGE,0,NONE);	//����λ�����ʹ����ļ��ɹ�
		if(fss!=FileSend_OK)	//���û���͵���λ������������
			return;
	} 
	else	
	{						//����δ֪��������λ�����ʹ���ֵ
		fss=SendFileCommand(BAD_MESSAGE,1,(u8*)(&res));
		if(fss!=FileSend_OK)	//���û���͵���λ������������
			return;
	}
}

/*�ƶ�ָ�����*/
void MoveServo_Function(void)
{
	volatile u8 jishuqi;
	FSS fss;					//�ļ����ݷ���״̬
	u16 Servo_Position[100];
	u8 i;
	//LD2701_MoveServoPosition(Command_Buffer.Data);
	MESSAGE_Reply(RECEIVE_OK,0,NULL);	//�ظ���λ�����ճɹ�
	switch(SERVO_TYPE)
	{
		case Digital:
			for(i=1;i<=SERVO_NUMBER;i++)
			{
				//�˴�Ӧ���м��������Ƿ���ȷ��
				Servo_Position[i - 1]=Command_Buffer.Data[(i-1)*3+1]*256+Command_Buffer.Data[(i-1)*3+2];	//�������Ƕ�
				//Servo_Position=Servo_Position*240/1023;	//���Ǽ������Ƕ� ������0-1023����ʾ0-300��
				//UBT12HB_SendPosition(i,Servo_Position,0,0);							
			}
			
			axAllPosition(SERVO_NUMBER, Servo_Position, 0x0320);
#ifdef KONGLONG
			steel_pst(22, Servo_Position[21]);
			steel_pst(23, Servo_Position[22]);
			steel_pst(24, Servo_Position[23]);
#endif			
			
			break;
			
		case Simulate_LD2701:
			Command_Buffer.Data[Command_Buffer.Length]='\0';	//���һλ�����ַ�
			//LD2701_MoveServoPosition(Command_Buffer.Data);
			break;
		
		default:
			SendFileCommand(BAD_MESSAGE,0,NULL);
			return;
	}
	fss=SendFileCommand(GOOD_MESSAGE,0,NULL);	//����λ�������ƶ�����ɹ�
	if(fss!=FileSend_OK)	//���û���͵���λ������������
		return;
}

/*ɾ��ָ�������ļ�*/
void DeleteActionFile_Function(void)
{
	FRESULT res;			//ͨ�ý������
	FSS fss;					//�ļ����ݷ���״̬
	u8 *NONE;					//ֻ���ڷ�����Ϣռλ
	char ActBuffer[100]={"/Action/"};			//�����ļ��������ֽ�
	u8 i;							//������
	
	MESSAGE_Reply(RECEIVE_OK,0,NULL);	//�ظ���λ�����ճɹ�
	
	for(i=0;i<Command_Buffer.Length;i++)			//�����ļ�����ActionĿ¼���Ҫת��
		ActBuffer[i+8]=Command_Buffer.Data[i];
	ActBuffer[i+8] = '\0';
	delay_ms(100);
	res=f_unlink(ActBuffer);
	delay_ms(100);	
	if(res==FR_OK)
	{
		fss=SendFileCommand(GOOD_MESSAGE,0,NONE);	//����λ�����ʹ����ļ��ɹ�
		if(fss!=FileSend_OK)	//���û���͵���λ������������
			return;
	}
	else	
	{						//����δ֪��������λ�����ʹ���ֵ
		fss=SendFileCommand(BAD_MESSAGE,strlen((const char*)PRINT_FREUSTL(res)),PRINT_FREUSTL(res));
		if(fss!=FileSend_OK)	//���û���͵���λ������������
			return;
	}
}

/*�ϴ����ж��������ļ���*/
void UploadOrderName_Function(void)
{
	FRESULT res;	//ͨ�ý������
	DIR dir;		//Ŀ¼�ṹ��
	FILINFO fno;	//�ļ�״̬�ṹ��
	FSS fss;		//�ļ����ݷ���״̬
	TCHAR *fn;		//�ļ���
	u8 Finish[2]={0xFF,0xFF};	//��������Ϣλ
	u8 FileNumber=0;			//�ļ���
	
	MESSAGE_Reply(RECEIVE_OK,0,NULL);	//�ظ���λ�����ճɹ�
	res=f_opendir(&dir,"/Order");	//��OrderĿ¼�����ļ�����
	while(1)
	{
		res=f_readdir(&dir,&fno);
		fn=fno.fname;		//ͬʱ֧�ֳ����ļ���
		if((*fn)==NULL)
			break;
		FileNumber++;
	}
	res=f_closedir(&dir);
	
	res=f_opendir(&dir,"/Order");	//��OrderĿ¼
	if(res==FR_OK)	//��OrderĿ¼�ɹ�
	{
		fss=SendFileCommand(ReplyOrderName,1,&FileNumber);	//����λ�����Ͱ�ͷ
		if(fss!=FileSend_OK)	//���û���͵���λ������������
			return;
		while(1)
		{
			res=f_readdir(&dir,&fno);	//��ȡOrderĿ¼
			if(res!=FR_OK)	//��ȡOrderĿ¼�ļ���ʧ����ȡ����ζ�ȡ
			{
				fss=SendFileCommand(BAD_MESSAGE,0,(u8*)(&res));	//����λ�����ش�������
				if(fss!=FileSend_OK)	//���û���͵���λ������������
					return;
				break;																//ȡ����ȡ
			}
			fn=fno.fname;		//ͬʱ֧�ֳ����ļ���
			if((*fn)==NULL)	//��ȡ�������ѭ��
				break;
			fss=SendFileCommand(ReplyOrderName,strlen(fn),(u8*)fn);	//����λ�������ļ���
			if(fss!=FileSend_OK)	//���û���͵���λ������������
				return;
		}
		
		fss=SendFileCommand(ReplyOrderName,2,Finish);	//���Ͱ�β
		if(fss!=FileSend_OK)	//���û���͵���λ������������
			return;
	}
	else												//����δ֪��������λ�����ʹ���ֵ
	{
		fss=SendFileCommand(BAD_MESSAGE,1,(u8*)(&res));
		if(fss!=FileSend_OK)	//���û���͵���λ������������
			return;
	}
}

/*�ϴ�����ָ�������ļ�����*/
void UploadOrderFile_Function(void)
{
	FRESULT res;	//ͨ�ý������
	static FIL fp;				//�ļ�����ṹ��
	u8 i;					//������
	UINT br;			//�Ѷ�ȡ�ֽ���
	FSS fss;			//�ļ����ݷ���״̬		
	static u8 FileName[100];	//������λ���ļ���
	u8 file_number=0;	//��¼�������ļ���Ŀ
	TCHAR OrdBuffer[100]={"/Order/"};		//���������ļ��������ֽ�
	TCHAR	ActBuffer[100];			//�����ļ��������ֽ�
	u8 Finish[2]={0xFF,0xFF};	//��������Ϣλ
	
	MESSAGE_Reply(RECEIVE_OK,0,NULL);	//�ظ���λ�����ճɹ�
	
	for(i=0;i<Command_Buffer.Length;i++)
		OrdBuffer[i+7]=Command_Buffer.Data[i];
	OrdBuffer[i+7]='\0';
	
	res=f_open(&fp,(const TCHAR*)OrdBuffer,FA_READ|FA_OPEN_EXISTING);			//��ֻ����ʽ���ļ���
	
	if(res==FR_OK)
	{
		i=1;
		for(;;i++)
		{
			res=f_read(&fp,ActBuffer+i-1,1,&br);
			if(res!=FR_OK)	//��ȡ�ļ����ɹ��ͽ���ѭ��
				break;
			if(br==0)	//�Ѷ�ȡ�ֽ���С��1����������˽�β
				break;
			if(*(ActBuffer+i-1)==' ')	//�Կո�ָ�
			{
				file_number++;
				i=0;		//���������
			}
		}
	}
	
	FileName[0]=file_number;	//��һλ���ļ���Ŀ
	for(i=0;OrdBuffer[i+7]!='\0';i++)
		FileName[i+1]=OrdBuffer[i+7];
	FileName[i+1]='\0';
	
	
	res=f_lseek(&fp,0);			//ָ���ƶ����ļ�ͷ

	if(res==FR_OK)
	{
		fss=SendFileCommand(ReplyOrder,strlen((const char*)FileName),FileName);	//����λ�����Ͱ�ͷ
		if(fss!=FileSend_OK)	//���û���͵���λ������������
			return;
		i=1;
		for(;;i++)
		{
			res=f_read(&fp,ActBuffer+i-1,1,&br);
			if(res!=FR_OK)	//��ȡ�ļ����ɹ��ͽ���ѭ��
				break;
			if(br==0)	//�Ѷ�ȡ�ֽ���С��1����������˽�β
				break;
			if(*(ActBuffer+i-1)==' ')	//�Կո�ָ�
			{
				*(ActBuffer+i-1)='\0';	//��Ϊ���ַ�
				fss=SendFileCommand(ReplyOrder,strlen(ActBuffer),(u8*)ActBuffer);	//����λ�������ļ���
				if(fss!=FileSend_OK)	//���û���͵���λ������������
					return;
				i=0;		//���������
			}
		}
		res=f_close(&fp);
		fss=SendFileCommand(ReplyOrder,2,Finish);	//���Ͱ�β
		if(fss!=FileSend_OK)	//���û���͵���λ������������
			return;
	}
	else												//����δ֪��������λ�����ʹ���ֵ
	{
		fss=SendFileCommand(BAD_MESSAGE,strlen((const char*)PRINT_FREUSTL(res)),PRINT_FREUSTL(res));
		//fss=SendFileCommand(BAD_MESSAGE,10,OrdBuffer);
		if(fss!=FileSend_OK)	//���û���͵���λ������������
			return;
	}
}

/*ɾ�����������ļ�*/
void DeleteOrderFile_Function(void)
{
	FRESULT res;			//ͨ�ý������
	FSS fss;					//�ļ����ݷ���״̬
	u8 *NONE;					//ֻ���ڷ�����Ϣռλ
	TCHAR	ActBuffer[100]={"/Order/"};			//�����ļ��������ֽ�
	u8 i;							//������
	
	MESSAGE_Reply(RECEIVE_OK,0,NULL);	//�ظ���λ�����ճɹ�
	
	for(i=0;i<Command_Buffer.Length;i++)			//�����ļ�����ActionĿ¼���Ҫת��
		ActBuffer[i+7]=Command_Buffer.Data[i];
	ActBuffer[i+7]='\0';
	printf("%d",res);
	res=f_unlink(ActBuffer);
	if(res==FR_OK)
	{
		fss=SendFileCommand(GOOD_MESSAGE,0,NONE);	//����λ�����ʹ����ļ��ɹ�
		if(fss!=FileSend_OK)	//���û���͵���λ������������
			return;
	}
	else	
	{						//����δ֪��������λ�����ʹ���ֵ
		fss=SendFileCommand(BAD_MESSAGE,1,(u8*)(&res));
		if(fss!=FileSend_OK)	//���û���͵���λ������������
			return;
	}
}


void EnableAllServo()
{
	u8 i;
	u16 position;
	//void FlexMargin(void);
	for(i=0;i<Command_Buffer.Length;i+=2)
	{
		if(Command_Buffer.Data[i+1]==1)
		{
			axTorque(Command_Buffer.Data[i],AX_ON) ;
			delay_ms(20);
		}
		else 
		{
			axTorque(Command_Buffer.Data[i],AX_OFF) ;
			/*position=axReadPosition(Command_Buffer.Data[i]);
			axSendPosition(Command_Buffer.Data[i],position,0x00);*/
			delay_ms(20);
		}
	}
    MESSAGE_Reply(RECEIVE_OK,0,NULL);
}

/*�����������ļ�*/
void ReNameActionFile_Function(void)
{
	FRESULT res;			//ͨ�ý������
	FSS fss;					//�ļ����ݷ���״̬
	u8 *NONE;					//ֻ���ڷ�����Ϣռλ
	TCHAR	OldActBuffer[100]={"/Action/"};			//�����ļ��������ֽ�
	TCHAR	NewActBuffer[100]={"/Action/"};			//�����ļ��������ֽ�
	u8 i;							//������
	for(i=0;i<Command_Buffer.Data[0];i++)			//�����ļ�����ActionĿ¼���Ҫת��
		OldActBuffer[i+7]=Command_Buffer.Data[i];
	OldActBuffer[i+7]='\0';
	for(i=Command_Buffer.Data[0]+1;i<Command_Buffer.Length;i++)			//�����ļ�����ActionĿ¼���Ҫת��
		NewActBuffer[i+7]=Command_Buffer.Data[i];
	NewActBuffer[i+7]='\0';
	printf("%d",res);
	res=f_rename(OldActBuffer,NewActBuffer);
	if(res==FR_OK)
	{
		fss=SendFileCommand(GOOD_MESSAGE,0,NONE);	//����λ�����ʹ����ļ��ɹ�
		if(fss!=FileSend_OK)	//���û���͵���λ������������
			return;
	}
	else	
	{						//����δ֪��������λ�����ʹ���ֵ
		fss=SendFileCommand(BAD_MESSAGE,1,(u8*)(&res));
		if(fss!=FileSend_OK)	//���û���͵���λ������������
			return;
	}
}

u32 cur_time_ms = 0;
u32 next_action_ms = 0;
u8 order_flag = 0;		//�Ƿ���ִ�ж�������
/*ִ��ָ���Ķ����ļ�(�ӿ��ͺ���)*/
FRESULT ExcuteAction(const TCHAR *path,u8 first_or_not)
{
	u8 debugnum=0;
	FRESULT res;			//ͨ�ý������
	FIL fp;					//�ļ�����ṹ��
	u8 i;					//������
	u8 send[1];
	u8 *NONE;						//ֻ���ڷ�����Ϣռλ
	volatile u8 jishuqi=0;
	UINT br;				//�Ѷ�ȡ�ֽ���
	u16 Servo_Position[100];
	u8 ServoPosition[SERVO_NUMBER*3];					//���λ�û����ֽ�
	TCHAR	ActBuffer[100]={"0:/Action/"};				//�����ļ��������ֽ�
	
	if(order_flag == 0)
	{
		cur_time_ms = clock_ms();
		next_action_ms = cur_time_ms + 20;
	}
	
	for(i=0;path[i]!='\0';i++)							//�����ļ�����ActionĿ¼���Ҫת��
		ActBuffer[i+10]=path[i];
	res=f_open(&fp,ActBuffer,FA_READ|FA_OPEN_EXISTING);	//��ֻ����ʽ���ļ���
	
	//u3_printf("%s",ActBuffer);
	if(res==FR_OK)
	{
		MESSAGE_Reply(RECEIVE_OK,0,NONE);
		while(1)
		{
			res=f_read(&fp,ServoPosition,SERVO_NUMBER*3,&br);

			if(res!=FR_OK)								//��ȡ�ļ����ɹ��ͽ���ѭ��
				break;
			if(br<(SERVO_NUMBER*3))						//˵�������˽�β
				break;
			else
			
			/*�ⲿ����ǰbreak�е㿴�����������ƶ�����������û��ִ�а�*/
			switch(SERVO_TYPE)
			{
				case Simulate_LD2701:
					//LD2701_MoveServoPosition(ServoPosition);	//�ƶ����
				case Digital:
					
					for(i=1;i<=SERVO_NUMBER;i++)
					{
						//�˴�Ӧ���м��������Ƿ���ȷ��						
						Servo_Position[i - 1]=ServoPosition[(i-1)*3+1]*256+ServoPosition[(i-1)*3+2];	//�������Ƕ�ǰ����Ǹ�λ������ǵ�λ��						
					}					
			}	
			//printf("%.2lf��n", pstdata.angle.z);

			while(clock_ms() < next_action_ms)
				;	//�ȴ�
			if(first_or_not==1){
			axAllPosition(SERVO_NUMBER, Servo_Position, 0x03E8);
				delay_ms(500);
				first_or_not = 0;
			}
			else axAllPosition(SERVO_NUMBER, Servo_Position, 0x0010);
#ifdef KONGLONG
			steel_pst(22, Servo_Position[21]);
			steel_pst(23, Servo_Position[22]);
			steel_pst(24, Servo_Position[23]);
#endif			
			delay_ms(10);								//�Ͽ������ÿ֡���20ms����ʹ��ң������ʱ��ҲҪ��
			next_action_ms = next_action_ms + 20;
		}
	}
	res=f_close(&fp);
	return res;
}

/*ִ��ָ���Ķ��������ļ�(�ӿ��ͺ���)*/
FRESULT ExcuteOrder(const TCHAR *path)
{
	u8 first_or_not = 1;
	u8 excute_or_not = 0;
	FRESULT res;			//ͨ�ý������
	FIL fp;						//�ļ�����ṹ��
	u8 i;							//������
	UINT br;					//�Ѷ�ȡ�ֽ���
	TCHAR OrdBuffer[100]={"0:/Order/"};		//���������ļ��������ֽ�
	TCHAR	ActBuffer[100];			//�����ļ��������ֽ�

	
	//��¼��ǰϵͳʱ��	
	cur_time_ms = clock_ms();
	next_action_ms = cur_time_ms + 20;
	order_flag = 1;
	
	
	for(i=0;path[i]!='\0';i++)
		OrdBuffer[i+9]=path[i];
	res=f_open(&fp,OrdBuffer,FA_READ|FA_OPEN_EXISTING);			//��ֻ����ʽ���ļ���
	if(res==FR_OK)
	{
		i=1;
		for(;;i++)	//�������ַ�˵�������˽�β
		{
			res=f_read(&fp,ActBuffer+i-1,1,&br);
			if(res!=FR_OK)	//��ȡ�ļ����ɹ��ͽ���ѭ��
				break;
			if(br==0)	//�Ѷ�ȡ�ֽ���С��1����������˽�β
				break;
			if(*(ActBuffer+i-1)==' ')	//�Կո�ָ�
			{
				*(ActBuffer+i-1)='\0';	//��Ϊ���ַ�
				res=ExcuteAction(ActBuffer,first_or_not);	//ִ��΢����
				excute_or_not = 1;
				i=0;		//���������
			}
			if(excute_or_not == 1)first_or_not = 0;
		}
	}
	res=f_close(&fp);
	
	order_flag = 0;
	return res;
}

/*�����ñ������*/
/*�������20��warning*/
u8* PRINT_FREUSTL(FRESULT res)
{
	switch(res)
	{
		case FR_OK: 					return (u8*)"FR_OK"; 							
		case FR_DISK_ERR: 				return (u8*)"FR_DISK_ERR";						/* (1) A hard error occured in the low level disk I/O layer */
		case FR_INT_ERR: 				return (u8*)"FR_INT_ERR"; 						/* (2) Assertion failed */
		case FR_NOT_READY: 				return (u8*)"FR_NOT_READY"; 					/* (3) The physical drive cannot work */
		case FR_NO_FILE: 				return (u8*)"FR_NO_FILE"; 						/* (4) Could not find the file */
		case FR_NO_PATH: 				return (u8*)"FR_NO_PATH"; 						/* (5) Could not find the path */
		case FR_INVALID_NAME: 			return (u8*)"FR_INVALID_NAME"; 					/* (6) The path name format is invalid */
		case FR_DENIED: 				return (u8*)"FR_DENIED"; 						/* (7) Acces denied due to prohibited access or directory full */
		case FR_EXIST: 					return (u8*)"FR_EXIST"; 						/* (8) Acces denied due to prohibited access */
		case FR_INVALID_OBJECT: 		return (u8*)"FR_INVALID_OBJECT"; 				/* (9) The file/directory object is invalid */
		case FR_WRITE_PROTECTED:		return (u8*)"FR_WRITE_PROTECTED"; 				/* (10) The physical drive is write protected */
		case FR_INVALID_DRIVE: 			return (u8*)"FR_INVALID_DRIVE"; 				/* (11) The logical drive number is invalid */
		case FR_NOT_ENABLED: 			return (u8*)"FR_NOT_ENABLED"; 					/* (12) The volume has no work area */
		case FR_NO_FILESYSTEM: 			return (u8*)"FR_NO_FILESYSTEM"; 				/* (13) There is no valid FAT volume */
		case FR_MKFS_ABORTED: 			return (u8*)"FR_MKFS_ABORTED";	 				/* (14) The f_mkfs() aborted due to any parameter error */
		case FR_TIMEOUT: 				return (u8*)"FR_TIMEOUT"; 						/* (15) Could not get a grant to access the volume within defined period */
		case FR_LOCKED: 				return (u8*)"FR_LOCKED"; 						/* (16) The operation is rejected according to the file shareing policy */
		case FR_NOT_ENOUGH_CORE: 		return (u8*)"FR_NOT_ENOUGH_CORE"; 				/* (17) LFN working buffer could not be allocated */
		case FR_TOO_MANY_OPEN_FILES: 	return (u8*)"FR_TOO_MANY_OPEN_FILES"; 			/* (18) Number of open files > _FS_SHARE */
		case FR_INVALID_PARAMETER: 		return (u8*)"FR_INVALID_PARAMETER"; 	
		default: 						return 0;
	}
}




