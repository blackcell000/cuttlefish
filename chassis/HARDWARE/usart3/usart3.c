#include "usart3.h"



u8 u3buf[512] = { 0 };					//����2���ջ���
u32 u3len = 0;							//����2���ܻ��������ݸ���



void USART3_IRQHandler()				//����2�жϺ���
{
	if (USART3->SR & (1 << 5))
	{
		u3buf[u3len] = USART3->DR;		//�յ�������ѹ�뻺����
		//USART1->DR = u3buf[u3len];		//ͨ����һ�����ڷ��ͳ�ȥ
		u3len = (u3len + 1) & 0x1FF;	//��512������
	}
}



void usart3_init(u32 bound)				//����2��ʼ�� bound: ������
{	
	USART_InitTypeDef usart3ini =		//����2��ʼ���ṹ��
	{
		115200,							//������
		USART_WordLength_8b,			//8λ���ݸ�ʽ
		1,								//1��ֹͣλ
		USART_Parity_No,				//����żУ��λ
		USART_Mode_Rx | USART_Mode_Tx,	//���� / ����ģʽ
		USART_HardwareFlowControl_None	//��Ӳ������������		
	};
	usart3ini.USART_BaudRate = bound;
	
	RCC->APB1RSTR |= 1 << 18;			//����3��λ
	RCC->APB1RSTR &= 0xFFFFFFFF ^ (1 << 18);	
	
	RCC->APB2ENR |= 1 << 3;				//ʹ��PBʱ��  
	RCC->APB1ENR |= 1 << 18;			//ʹ�ܴ���3ʱ��


	GPIOB->CRH &= 0xFFFF00FF;			//PB.10 ����������� ����ٶ�50MHz
	GPIOB->CRH |= 0X00008B00;			//PB.11 ���� / ���� ����
	
	RCC->APB1RSTR |= 1 << 18;			//����3��λ
	RCC->APB1RSTR &= 0xFFFFFFFF ^ (1 << 18);
										//����3��λֹͣ 
	NVIC_Init(&USART3_NVIC_CONFIG); 	//���ô���2�������ȼ�	  

	USART_Init(USART3, &usart3ini); 	//��ʼ������2
	USART3->CR1 |= 1 << 5;				//���ջ����� (�Ĵ���RDR) �ǿ��ж�ʹ��
	USART3->CR1 |= 1 << 13;				//����3����״̬ʹ��
	
	//USART2->CR3 |= 1 << 7;			//����2DMA����ʹ��
}



void usart3_senddat(u8 *buf, u8 len)	//ʹ�ô���3����һ������
{										//buf:�������׵�ַ
	u8 i;								//len:���͵��ֽ���
	
	for (i = 0; i <= len - 1; i++)
	{
		while (!(USART3->SR & (1 << 6)));
										//�ȴ��������
		USART3->DR = buf[i];
	}
	while (!(USART3->SR & (1 << 6)));
}



