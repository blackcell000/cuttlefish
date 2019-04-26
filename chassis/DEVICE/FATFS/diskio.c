/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "sdio_sdcard.h"

/* Definitions of physical drive number for each drive */
#define SD_CARD		0	/* Example: Map Ramdisk to physical drive 0 */

#define SDCARD_BLOCK_SIZE 512

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{ 
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	SD_Error  Status;
	Status=SD_Init();
	if (Status!=SD_OK )	
		return STA_NOINIT;
	else
		return RES_OK;
} 


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address in LBA */
	UINT count		/* Number of sectors to read */
)
{
	UINT i;
	for(i=0; i<count; i++)	//SD_ReadMultiBlocks�����ã�����ԭ��
	{
		SD_ReadBlock(buff+i*SDCARD_BLOCK_SIZE, (sector+i)*SDCARD_BLOCK_SIZE, SDCARD_BLOCK_SIZE);
			
		/* Check if the Transfer is finished */
		SD_WaitReadOperation();  //ѭ����ѯdma�����Ƿ����
		
		/* Wait until end of DMA transfer */
		while(SD_GetStatus() != SD_TRANSFER_OK);
	}
	return RES_OK; 
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address in LBA */
	UINT count			/* Number of sectors to write */
)
{
	if (count > 1)
	{
		SD_WriteMultiBlocks((uint8_t *)buff, sector*SDCARD_BLOCK_SIZE, SDCARD_BLOCK_SIZE, count);
			
		/* Check if the Transfer is finished */
		SD_WaitWriteOperation();	   //�ȴ�dma�������
		while(SD_GetStatus() != SD_TRANSFER_OK); //�ȴ�sdio��sd���������
	}
	else
	{
		SD_WriteBlock((uint8_t *)buff,sector*SDCARD_BLOCK_SIZE, SDCARD_BLOCK_SIZE);
			
		/* Check if the Transfer is finished */
		SD_WaitWriteOperation();	   //�ȴ�dma�������
		while(SD_GetStatus() != SD_TRANSFER_OK); //�ȴ�sdio��sd���������
	}	
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;
	switch( cmd )
	{
		case CTRL_SYNC:      // ������ֱ�ӷ���0 
		{
			res = RES_OK;
			break;
		}
		case GET_SECTOR_SIZE:   // ��ȡ����������һ�㶼��512���ֽ�
		{
			*(WORD*)buff = 512;
			res = RES_OK;
			break;
		}
		case GET_SECTOR_COUNT:  // ��ȡ��������
		{
			res = RES_OK;
			break;
		}
		case GET_BLOCK_SIZE:    // һ����������ֽ�����
		{                       
			res = RES_OK;
			break;
		}
		case CTRL_TRIM:
		{
			res = RES_OK;
			break;
		}
		default :
		{
			res = RES_PARERR;   // ��������
			break;
		}
	}
	return res;
}

DWORD get_fattime (void)
{				 
	return 0;
}	

