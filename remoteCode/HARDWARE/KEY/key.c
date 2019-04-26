#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"



//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);//ʹ��PA PC ʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	PBout(12)=1;
	PBout(13)=1;
	PBout(14)=1;
	PBout(15)=1;
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8;//
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��
		PCout(6)=1;
		PCout(7)=1;
    PCout(8)=1;
}
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;

//ע��˺�������Ӧ���ȼ�,KEY_Front>KEY_Back>KEY_Left>KEY_Right>KEY4>KEY5>KEY6==0
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if((key_up)&&(KEYLeft==0||KEYRight==0||KEY4==0||KEY5==0||KEY6==0||KEYFront==0||KEYBack==0))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(KEYFront==0)   return KEYFront_PRES;
		else if(KEYBack==0)return KEYBack_PRES;
		else if(KEYLeft==0) return KEYLeft_PRES;
		else if(KEYRight==0) return KEYRight_PRES;
		else if(KEY4==0) return KEY4_PRES;
		else if(KEY5==0) return KEY5_PRES;
		else if(KEY6==0) return KEY6_PRES;
		
	}
	else if(KEYLeft==1&&KEYRight==1&&KEY4==1&&KEY5==1&&KEY6==1&&KEYFront==1&&KEYBack==0) key_up=1; 	    
 	return 0;// �ް�������
}

/**************************************************************************
�������ܣ�����ɨ��
��ڲ�����˫���ȴ�ʱ��
����  ֵ������״̬ 0���޶��� 1������ 2��˫�� 
**************************************************************************/
u8 click_KEYFront (u8 time)
{
		static	u8 flag_key,count_key,double_key;	
		static	u16 count_single,Forever_count;
	  if(KEYFront==0)  Forever_count++;   //������־λδ��1
     else        Forever_count=0;
		if(0==KEYFront&&0==flag_key)		flag_key=1;	
	  if(0==count_key)
		{
				if(flag_key==1) 
				{
					double_key++;
					count_key=1;	
				}
				if(double_key==2) 
				{
					double_key=0;
					count_single=0;
					return doubleClick;//˫��ִ�е�ָ��
				}
		}
		if(1==KEYFront)			flag_key=0,count_key=0;
		
		if(1==double_key)
		{
			count_single++;
			if(count_single>time&&Forever_count<time)
			{
			double_key=0;
			count_single=0;	
			return singleClick;//����ִ�е�ָ��
			}
			if(Forever_count>time)
			{
			double_key=0;
			count_single=0;	
			//return longPress;
			}
		}	
		return 0;
}

u8 click_KEYBack (u8 time)
{
		static	u8 flag_key,count_key,double_key;	
		static	u16 count_single,Forever_count;
	  if(KEYBack==0)  Forever_count++;   //������־λδ��1
     else        Forever_count=0;
		if(0==KEYBack&&0==flag_key)		flag_key=1;	
	  if(0==count_key)
		{
				if(flag_key==1) 
				{
					double_key++;
					count_key=1;	
				}
				if(double_key==2) 
				{
					double_key=0;
					count_single=0;
					return doubleClick;//˫��ִ�е�ָ��
				}
		}
		if(1==KEYBack)			flag_key=0,count_key=0;
		
		if(1==double_key)
		{
			count_single++;
			if(count_single>time&&Forever_count<time)
			{
			double_key=0;
			count_single=0;	
			return singleClick;//����ִ�е�ָ��
			}
			if(Forever_count>time)
			{
			double_key=0;
			count_single=0;	
			}
		}	
		return 0;
}


