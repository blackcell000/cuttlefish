#ifndef USTDOUT_H
#define USTDOUT_H

#include "hardware.h"


extern USART_TypeDef *stdout_USARTx;		//��׼���������ָ��

void usartx_senddat(USART_TypeDef *USARTx, u8 *buf, u8 len);
							

#endif

