#ifndef USART2_H
#define USART2_H	
 
#include "hardware.h"

extern u8 u2buf[256];					//����2���ջ���
extern u32 u2len;						//����2���ܻ��������ݸ���



void usart2_init(u32 bound);			//����2��ʼ�� bound: ������

void usart2_senddat(u8 *buf,u8 len);	//ʹ�ô���2����һ������
										//buf:�������׵�ַ
										//len:���͵��ֽ���


#endif	   



