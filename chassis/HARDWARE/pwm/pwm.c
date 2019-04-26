#include "pwm.h"


u8 cur_steel_tim4 = 1;
u8 cur_steel_tim3 = 1;

#define steel1_mdf 0
#define steel2_mdf 0 	//�������� ���¼�С
#define steel3_mdf 0	//���ϼ�С ��������

u16 steel1_pst = 1500 + steel1_mdf;	//ID = 22
u16 steel2_pst = 1500 + steel2_mdf;	//ID = 23
u16 steel3_pst = 1500 + steel3_mdf;	//ID = 24
u16 steel4_pst = 1500;


//750us - 2250us
void steel_pst(u8 ID, u16 pst)
{
	if(ID == 22)
	{
		steel1_pst = pst / 1024.0 * 1500 + 750 + steel1_mdf;
		steel1_pst = steel1_pst;
	}
	else if(ID == 23)
	{
		steel2_pst = pst / 1024.10 * 1500 + 750 + steel2_mdf;
		steel2_pst = steel2_pst;

	}
	else if(ID == 24)
	{
		steel3_pst = pst / 1024.0 * 1500 + 750 + steel3_mdf;	
		steel3_pst = steel3_pst;		
	}
	
	
}
void TIM4_IRQHandler(void)   							//TIM4�ж�
{
	
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  	//���TIMx�����жϱ�־ 
		
		cur_steel_tim4++;
		if(cur_steel_tim4 > 4)
			cur_steel_tim4 = 1;
		
		TIM4->CCR1 = steel1_pst;	
		TIM4->CCR2 = steel2_pst;	
		TIM4->CCR3 = steel3_pst;	
		
		if(cur_steel_tim4 == 1)
		{
			TIM4->CCER |= 0x0001;
		}
		else if(cur_steel_tim4 == 2)
		{
			TIM4->CCER |= 0x0010;
		}		
		else if(cur_steel_tim4 == 3)
		{
			TIM4->CCER |= 0x0100;
		}		
		else if(cur_steel_tim4 == 4)
		{
			TIM4->CCER |= 0x0000;
		}		
		
	}
}



//TIM4 PWM���ֳ�ʼ�� 

void TIM4_Init(void)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);			//ʹ�ܶ�ʱ��4ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  
																	//ʹ��GPIO�����AFIO���ù���ģ��ʱ��
	
 
	//���ø�����Ϊ�����������,���TIM4 CH1��PWM���岨��	GPIOB.6
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
																	//TIM4_CH1, TIM4_CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  				//�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);							//��ʼ��GPIO
 
	//��ʼ��TIM4
	TIM_TimeBaseStructure.TIM_Period = 19999; 						//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 71; 						//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 					//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;		//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 				//����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); 						//ʹ��ָ����TIM4�ж�,��������ж�


	NVIC_Init(&TIM4_NVIC_CONFIG);  									//��ʼ��NVIC�Ĵ���


	//��ʼ��TIM4 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 		//�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  						//����Tָ���Ĳ�����ʼ������TIM4 OC1
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);  						//����Tָ���Ĳ�����ʼ������TIM4 OC2
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);  						//����Tָ���Ĳ�����ʼ������TIM4 OC3

	TIM_Cmd(TIM4, ENABLE); 							 				//ʹ��TIM4	

}



