#ifndef DMA_H
#define DMA_H


#include "stm32f10x.h"


extern u8 DMA1_BUSY_FLAG;				//DMA1æµ״̬���

void DMA1_Init(void);					//DMA1��ʼ��

u32 DMA1_SendDat(u8 *buf, u8 numdat);	//����һ��DMA����

/*
typedef struct tagDMA1U3BUF				//DMA����3���仺����
{
	u8 dat[256];
	u16 num;
	struct tagDMA1U3BUF *next;
} DMA1U3BUF, *PDMA1U3BUF;
*/

#endif
