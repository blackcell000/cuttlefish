 
#include "usart_tren.h"


void usart_modetx(void)
{
	RXEN = 0;
	TXEN = 1;	
	USART3->CR2 &= ~(1 << 5);	//���ս�ֹ
}

void usart_moderx(void)
{
	USART3->CR2 |= 1 << 5;	//����ʹ��

	TXEN = 0;
	RXEN = 1;	
}


void usart_tren_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 
															//ʹ��PB�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
															//LED0-->PB.15 �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					//�����趨������ʼ��GPIOB.0/1		

	usart_moderx();
}





