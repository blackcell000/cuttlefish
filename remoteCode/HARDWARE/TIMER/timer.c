#include "stm32f10x.h"
#include "timer.h"
#include "usart.h"	
#include "adc.h"
#include "24l01.h"


u16 adcx,adcy;
u8 send_flag;

float temp;
float speed[2];
u8 speed_flag;
void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����
							 
}



//#define Front 1
//#define Back 2
//#define OneSpeed 1
//#define TwoSpeed 2

void speed_solve()
{
	u8 i=2;
	
	if(speed[0]<0.5&&speed[0]>-0.5)
	{
		if(speed[1]<-1.5) speed_flag=GoUp;//ǰ��
		else if(speed[1]>1.5) speed_flag=GoBack; //����
		else speed_flag=Stop;//
	}
	else if(speed[0]>1.5) speed_flag=GoLeft;//��ת
	else if(speed[0]<-1.5) speed_flag=GoRight;//��ת
	else speed_flag=Stop;
}

//��ʱ��3�жϷ������
//ҡ�������ɼ�Ϊ 2+-0.3

/*

��ҡ�˵�λ���ɼ�ֵ (adc10,adc11)
ǰ (1.96,0.005)  ��(1.96,3.9)
�� (3.9,1.98)  ��(0.001,1.98)

speed[]={adc10,adc11,adc12,adc13)
*/

void TIM3_IRQHandler(void)     //TIM3���ж�
{
	static char i = 0;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
		{
		i++;
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx���жϴ�����λ:TIM �ж�Դ 
		if(i == 5)
		{
			LED1 = !LED1;i = 0; //������ʾ
		}
		adcx=Get_Adc_Average(ADC_Channel_10,10);
		temp=(float)adcx*(4.0/4096);
		//printf("adc10 %f\r\n",temp);
		temp -= 2.0;
		if(temp<0.15&&temp>-0.15) temp = 0;
		speed[0] = temp;//left or right
		
		adcy=Get_Adc_Average(ADC_Channel_11,10);
		temp=(float)adcy*(4.0/4096);
		//printf("adc11 %f\r\n",temp);
		temp -= 2.0;
		if(temp<0.15&&temp>-0.15) temp = 0;
		speed[1] = temp;//front or back
		
//		adcq=Get_Adc_Average(ADC_Channel_12,10);
//		temp=(float)adcq*(4.0/4096);
//		//printf("adc12 %f\r\n",temp);
//		temp -= 2.0;
//		if(temp<0.15&&temp>-0.15) temp = 0;
//		speed[0] = temp;//left and right
//		
//		adch=Get_Adc_Average(ADC_Channel_13,10);
//		temp=(float)adch*(4.0/4096);
//		//printf("adc13 %f\r\n",temp);
//		temp -= 2.0;
//		if(temp<0.15&&temp>-0.15) temp = 0;
//		speed[1] = temp;//front and back
		
	   speed_solve();
		 
		}
}
			
	



