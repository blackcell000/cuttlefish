#include "timer.h"



int count_sec = -1;


void TIM2_IRQHandler(void)   							//TIM4�ж�
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM4�����жϷ������
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  	//���TIMx�����жϱ�־ 
		
		//LED0 = ~LED0;
		count_sec++;
	}
}


void TIM2_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); 			//ʱ��ʹ��
												
	
	//��ʱ��TIM4��ʼ��
	TIM_TimeBaseStructure.TIM_Period = 9999; 						//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =7199; 						//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 		//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); 				//����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); 						//ʹ��ָ����TIM3�ж�,��������ж�

	//�ж����ȼ�NVIC����

	NVIC_Init(&TIM2_NVIC_CONFIG);  									//��ʼ��NVIC�Ĵ���


	TIM_Cmd(TIM2, ENABLE);  										//ʹ��TIMx					 
}


u32 clock_ms()
{
	return count_sec * 1000 + TIM2->CNT / 10.0;
}










