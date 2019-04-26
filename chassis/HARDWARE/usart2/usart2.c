#include "usart2.h"
#include "jy61.h"
#include "config.h"



u8 u2buf[256] = { 0 };					//����2���ջ���
u32 u2len = 0;							//����2���ܻ��������ݸ���



void USART2_IRQHandler()				//����2�жϺ���
{
	if (USART2->SR & (1 << 5))
	{
		u2buf[u2len] = USART2->DR;		//�յ�������ѹ�뻺����
		//USART1->DR = u2buf[u2len];	//ͨ����һ�����ڷ��ͳ�ȥ
#ifdef MOFANG
		jyStateIterator(u2buf[u2len]);
#endif
		u2len = (u2len + 1) & 0xFF;		//��128������										
	}
}


void usart2_init(u32 bound)				//����2��ʼ�� bound: ������
{	
	USART_InitTypeDef usart2ini =		//����2��ʼ���ṹ��
	{
		115200,							//������
		USART_WordLength_8b,			//8λ���ݸ�ʽ
		1,								//1��ֹͣλ
		USART_Parity_No,				//����żУ��λ
		USART_Mode_Rx | USART_Mode_Tx,	//���� / ����ģʽ
		USART_HardwareFlowControl_None	//��Ӳ������������		
	};
	usart2ini.USART_BaudRate = bound;
	
	RCC->APB1RSTR |= 1 << 17;			//����2��λ
	RCC->APB1RSTR &= 0xFFFFFFFF ^ (1 << 17);	
	
	RCC->APB2ENR |= 1 << 2;				//ʹ��PAʱ��  
	RCC->APB1ENR |= 1 << 17;			//ʹ�ܴ���2ʱ��


	GPIOA->CRL &= 0xFFFF00FF;			//PA.2 ����������� ����ٶ�50MHz
	GPIOA->CRL |= 0X00008B00;			//PA.3 ���� / ���� ����
	
	RCC->APB1RSTR |= 1 << 17;			//����2��λ
	RCC->APB1RSTR &= 0xFFFFFFFF ^ (1 << 17);
										//����2��λֹͣ 
	NVIC_Init(&USART2_NVIC_CONFIG); 	//���ô���2�������ȼ�	  

	USART_Init(USART2, &usart2ini); 	//��ʼ������2
	USART2->CR1 |= 1 << 5;				//���ջ����� (�Ĵ���RDR) �ǿ��ж�ʹ��
	USART2->CR1 |= 1 << 13;				//����2����״̬ʹ��
	
	//USART2->CR3 |= 1 << 7;			//����2DMA����ʹ��
}



void usart2_senddat(u8 *buf, u8 len)	//ʹ�ô���2����һ������
{										//buf:�������׵�ַ
	u8 i;								//len:���͵��ֽ���

	while (!(USART2->SR & (1 << 6)));

	for (i = 0; i <= len - 1; i++)		//�ȴ��������
	{		
										
		USART2->DR = buf[i];
		while (!(USART2->SR & (1 << 7)));
										//�ȴ��������ݼĴ�����
	}
	while (!(USART2->SR & (1 << 6)));	//�ȴ��������
}


