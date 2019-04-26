#include "hardware.h"
#include "usart1.h"
#include "protocal.h"
#include "action.h"
#include "config.h"

u8 continue_or_not = 0;
u8 u1buf[256] = { 0 };					//����1���ջ���
u32 u1len = 0;							//����1���ܻ��������ݸ���
u8 action = -1;
u8 getAction = 0;

u8 u1buf_action[256] = { 0 };					//����1���ջ���
u32 u1len_action = 0;							//����1���ܻ��������ݸ���

void USART1_IRQHandler()				//����1�жϺ���
{
	static u8 flag = 0;
	if (USART1->SR & (1 << 5))
	{
		u1buf[u1len] = USART1->DR;		//�յ�������ѹ�뻺����	
//		u1buf_action[u1len_action] = u1buf[u1len];
		if(u1buf[u1len] == 0x55)flag = 1;
		if(flag == 1){
				u1len = (u1len + 1) & 0xFF;		//��256������		
				if(u1len == 6){
					u1len = 0;
					action = u1buf[3];
					getAction = 1;
					flag = 0;
					if(action == 1||action == 2|| action == 3||action == 4||action == 5){
						stop = 1;
						continue_or_not = 1;
					}
					else if(action == 6){
						getAction = 0;
						stop =  1;
					}
					else if(action == 7){
					}
					else if(action == 8)Speed_up();
					else if(action == 9){
						Speed_down();
						LED0 = ~LED0;
					}
					else if(action == 10){
					}
					else if(action == 11){
					}
					else if(action == 12){
					}
//					else if(action == 13){
//						if(state == ROBOT_ARM )state = ROBOT_BODY;
//						else state = ROBOT_ARM;
//					}
					else if(action == 14){
						axAllPosition(SERVO_NUMBER_WITH_ARM, InitialPosition_with_arm, 0x03E8);
						delay_ms(1000);
					}
				}
		}
//		ptcGetCommand(u1buf_action[u1len_action]);
//		u1len_action = (u1len_action + 1) & 0xFF;
	}
}
//55 05 06 01 01 00

void usart1_init(u32 bound)				//����1��ʼ�� bound: ������
{	
	USART_InitTypeDef usart1ini = 		//����1��ʼ���ṹ��
	{
		115200,							//������
		USART_WordLength_8b,			//8λ���ݸ�ʽ
		1,								//1��ֹͣλ
		USART_Parity_No,				//����żУ��λ
		USART_Mode_Rx | USART_Mode_Tx,	//���� / ����ģʽ
		USART_HardwareFlowControl_None	//��Ӳ������������
	};
	usart1ini.USART_BaudRate = bound;
	
	RCC->APB2RSTR |= 1 << 14;			//����1��λ
	RCC->APB2RSTR &= 0xFFFFFFFF ^ (1 << 14);

	RCC->APB2ENR |= 1 << 2;   			//ʹ��PAʱ��  
	RCC->APB2ENR |= 1 << 14;  			//ʹ�ܴ���1ʱ�� 

	GPIOA->CRH &= 0XFFFFF00F;			//PA.9 	�����������	������Ƶ��50MHz
	GPIOA->CRH |= 0X000008F0;			//PA.10 ���� / ��������

	NVIC_Init(&USART1_NVIC_CONFIG);		//���ô���1�������ȼ�	  

	USART_Init(USART1, &usart1ini);		//��ʼ������1	
	USART1->CR1 |= 1 << 5;				//���ջ����� (�Ĵ���RDR) �ǿ��ж�ʹ��
	USART1->CR1 |= 1 << 13;				//����1����״̬ʹ��
}

void usart1_senddat(u8 *buf, u8 len)	//ʹ�ô���1����һ������
{										//buf:�������׵�ַ
	u8 i;								//len:���͵��ֽ���
	for (i = 0; i <= len - 1; i++)
	{
		while (!(USART1->SR & (1 << 6)));
										//�ȴ��������
		USART1->DR = buf[i];
	}
	while (!(USART1->SR & (1 << 6)));
}




