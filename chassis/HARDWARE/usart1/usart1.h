#ifndef USART_H
#define USART_H

#include "hardware.h"

void usart1_init(u32 bound);			//����1��ʼ�� bound: ������

void usart1_senddat(u8 *buf, u8 len);	//ʹ�ô���1����һ������
										//buf:�������׵�ַ
										//len:���͵��ֽ���
																				
extern u8 u1buf[256];					//����2���ջ���
extern u32 u1len;						//����2���ܻ��������ݸ���
extern u8 continue_or_not;

#endif	   








