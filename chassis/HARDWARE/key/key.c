#include "key.h"
#include "protocal.h"
#include "string.h"
#include "stdio.h"
#include "config.h"

void EXTI1_IRQHandler(void)
{
#ifdef MOYU
	char filename[] = "moyu1.0.odr";
#endif

	
	LED0 = 0;
	delay_ms(50);//����
	LED1 = 1;
	
	Command_Buffer.InsType = ExecuteOrderFile;	
	sprintf((char*)Command_Buffer.Data, filename);
	Command_Buffer.Length = strlen(filename);
	GetProtocal = 1;
	

	EXTI_ClearITPendingBit(EXTI_Line1); //���LINE0�ϵ��жϱ�־λ  
	
}
 


void EXTIX_Init(void)
{

 	EXTI_InitTypeDef EXTI_InitStructure;
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��

    //GPIOC.1 �ж����Լ��жϳ�ʼ������   �½��ش���
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line1;			//KEY2
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 					//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���


  	NVIC_Init(&EXIT1_NVIC_CONFIG); 
}

//�ⲿ�ж�1������� 

void key_init(void)
{
	RCC->APB2ENR |= 1 << 4;     //ʹ��PORTCʱ��
	GPIOC->CRL &= 0XFFFFFF0F;	//PC1���ó�����  
	GPIOC->CRL |= 0X00000080; 	//���� / ��������
	  
	GPIOC->BSRR |= 1 << 1;	   	//PC1 ����
	EXTIX_Init();	
} 


