#include "stm32f10x.h"


u8 DMA1_BUSY_FLAG = 0;
//DMA1æµ״̬���
//0: DMA1����
//1: DMA1æµ

void DMA1_Channel2_IRQHandler(void)					//DMA1ͨ��2ȫ���жϷ�����
{													//�������Ϊ����������ú���, ͬ������												
	DMA1->IFCR |= 1 << 4;							//���DMA1ȫ���жϱ�־
	DMA1_Channel2->CCR &= 0xFFFE;					//�ر�DMA1����	
	DMA1_BUSY_FLAG = 0;								//DMA1��Ϊ����״̬
}

//DMA1��ʼ��
//ʹ��DMA1������, �����ƴ���3�����ݴ���
//��ʼ��ʱ, ����������
//buf: �洢����ַ
void DMA1_Init(void)
{
	NVIC_InitTypeDef DMA1_Channel2_NVIC_CONFIG = 			
	{												//DMA1 ͨ��2 ȫ���жϳ�ʼ���ṹ��
		DMA1_Channel2_IRQn,
		2,
		2,
		ENABLE	
	};
	
	volatile u32 i, j;
/*	������ʱ�ı�����Ҫ��������Ż�					*/

/*	DMA1ͨ��ʹ��									*/
	RCC->AHBENR |= 1;								

	for(i = 1; i <= 300; i++)			
		for(i = j; j <= 300; j++);
/*	�ȴ�DMA1ʱ���ȶ�, ������ʱ��Ϊ�˼����ⲿ������	
	��Լ10ms										*/
	
/* 	����ӳ���, ����3��TX��Ӧ����DMA1��ͨ��2		*/
	DMA1_Channel2->CCR = 0x309A;
	
/*	0x3090 = 0011 0000 1001 1010 b					**
**	[0]     0   ͨ����ֹ							**
**  [1]		1   ��������ж�:		����			**
**	[2]   	0 	�봫���ж�:			��ֹ 			**												
**	[3]   	1 	��������ж�:		����			**												
**	[4]     1   ���ݴ��䷽��: 		�Ӵ洢����		**
**	[5]     0   ѭ��ģʽ: 			��				**
**	[6]     0   �����ַ����ģʽ: 	��				**
**	[7]     1   �洢����ַ����ģʽ: ��				**
**	[9:8]   00  �������ݿ��: 		8λ				**
**	[11:10] 00  �������ݿ��: 		8λ				**
**	[13:12] 11  ͨ�����ȼ�:			���			**
**	[14]    0   �洢�����洢��ģʽ:	��				**
**	[31:15]    	����, ʼ��Ϊ0						*/


/*	�����ַ�Ĵ����洢����2���ͻ�����TDR�ĵ�ַ		*/	
	DMA1_Channel2->CPAR	= (u32)&USART3->DR;	
	
	NVIC_Init(&DMA1_Channel2_NVIC_CONFIG);			//DMA1 ͨ��2 �ж����ȼ���ʼ��	
}

//����һ��DMA����
//buf: �洢����ַ
//numdat: ��Ҫ�������������
//����0˵�������ɹ�, ����ʧ���򷵻ص�ǰ�������������
u32 DMA1_SendDat(u8 *buf, u8 numdat)
{
	if(DMA1_BUSY_FLAG == 0)							
	{												//�������2����ΪDMAģʽ��DMA1����											
		DMA1_BUSY_FLAG = 1;							//DMA1��Ϊæµ״̬
		
		DMA1_Channel2->CCR &= 0xFFFE;				//�ȹر�DMA1����
		
		DMA1_Channel2->CNDTR = numdat;				//���ô�����������
		DMA1_Channel2->CMAR = (u32)buf;				//���ô洢����ַ
		
		DMA1_Channel2->CCR |= 1;					//��������	
		return 0;
	}
	return DMA1_Channel2->CNDTR;
	
/*		������ɺ�, ����DMA1��������жϷ�������	**
**		��DMAæµ��־λ���� 						*/
}

