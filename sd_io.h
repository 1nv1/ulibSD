/*
 *  sd_io.h
 *  Author: Nelson Lombardo (C) 2015
 *  e-mail: nelson.lombardo@gmail.com
 *  License: MIT
 */
#ifndef _SD_IO_H_
#define _SD_IO_H_

/*****************************************************************************/
/* Configurations                                                            */
/*****************************************************************************/
//#define _M_IX86           // For use with x86 architecture
#define SD_IO_WRITE
//#define SD_IO_WRITE_WAIT_BLOCKER
#define SD_IO_WRITE_TIMEOUT_WAIT 250

//#define SD_IO_DBG_COUNT
/*****************************************************************************/

#if defined(_M_IX86)

#include <stdio.h>
#include "integer.h"

/* Results of SD functions */
typedef enum {
    SD_OK = 0,      /* 0: Function succeeded    */
    SD_NOINIT       /* 1: SD not initialized    */
    SD_ERROR,       /* 2: Disk error            */
    SD_PARERR,      /* 3: Invalid parameter     */
    SD_BUSY,        /* 4: Programming busy      */
    SD_REJECT       /* 5: Reject data           */
} SDRESULTS;

#ifdef SD_IO_DBG_COUNT
typedef struct _DBG_COUNT {
    WORD read;
    WORD write;
} DBG_COUNT;
#endif

/* SD device object */
typedef struct _SD_DEV {
    BOOL mount;
    BYTE cardtype;
    char fn[20]; /* dd if=/dev/zero of=sim_sd.raw bs=1k count=0 seek=8192 */
    FILE *fp;
    DWORD last_sector;
#ifdef SD_IO_DBG_COUNT
    DBG_COUNT debug;
#endif
} SD_DEV;

#else // For use with uControllers

#include "spi_io.h" /* Provide the low-level functions */

/* Definitions of SD commands */
#define CMD0    (0x40+0)        /* GO_IDLE_STATE            */
#define CMD1    (0x40+1)        /* SEND_OP_COND (MMC)       */
#define ACMD41  (0xC0+41)       /* SEND_OP_COND (SDC)       */
#define CMD8    (0x40+8)        /* SEND_IF_COND             */
#define CMD9    (0x40+9)        /* SEND_CSD                 */
#define CMD16   (0x40+16)       /* SET_BLOCKLEN             */
#define CMD17   (0x40+17)       /* READ_SINGLE_BLOCK        */
#define CMD24   (0x40+24)       /* WRITE_SINGLE_BLOCK       */
#define CMD42   (0x40+42)       /* LOCK_UNLOCK              */
#define CMD55   (0x40+55)       /* APP_CMD                  */
#define CMD58   (0x40+58)       /* READ_OCR                 */
#define CMD59   (0x40+59)       /* CRC_ON_OFF               */

#define SD_INIT_TRYS    0x03

/* CardType) */
#define SDCT_MMC        0x01                    /* MMC version 3    */
#define SDCT_SD1        0x02                    /* SD version 1     */
#define SDCT_SD2        0x04                    /* SD version 2     */
#define SDCT_SDC        (SDCT_SD1|SDCT_SD2)     /* SD               */
#define SDCT_BLOCK      0x08                    /* Block addressing */

#define SD_BLK_SIZE     512

/* Results of SD functions */
typedef enum {
    SD_OK = 0,      /* 0: Function succeeded    */
    SD_NOINIT,      /* 1: SD not initialized    */
    SD_ERROR,       /* 2: Disk error            */
    SD_PARERR,      /* 3: Invalid parameter     */
    SD_BUSY,        /* 4: Programming busy      */
    SD_REJECT,      /* 5: Reject data           */
    SD_NORESPONSE   /* 6: No response           */
} SDRESULTS;

#ifdef SD_IO_DBG_COUNT
typedef struct _DBG_COUNT {
    WORD read;
    WORD write;
} DBG_COUNT;
#endif


/* SD device object */
typedef struct _SD_DEV {
    BOOL mount;
    BYTE cardtype;
    DWORD last_sector;
#ifdef SD_IO_DBG_COUNT
    DBG_COUNT debug;
#endif
} SD_DEV;

#endif

/*******************************************************************************
 * Public Methods - Direct work with SD card                                   *
 ******************************************************************************/

/**
    \brief Initialization the SD card.
    \return If all goes well returns SD_OK.
 */
SDRESULTS SD_Init (SD_DEV *dev);

/**
    \brief Read a single block.
    \param dest Pointer to the destination object to put data
    \param sector Start sector number (internally is converted to byte address).
    \param ofs Byte offset in the sector (0..511).
    \param cnt Byte count (1..512).
    \return If all goes well returns SD_OK.
 */
SDRESULTS SD_Read (SD_DEV *dev, void *dat, DWORD sector, WORD ofs, WORD cnt);

/**
    \brief Write a single block.
    \param dat Data to write.
    \param sector Sector number to write (internally is converted to byte address).
    \return If all goes well returns SD_OK.
 */
SDRESULTS SD_Write (SD_DEV *dev, void *dat, DWORD sector);

/**
    \brief Allows know status of SD card.
    \return If all goes well returns SD_OK.
*/
SDRESULTS SD_Status (SD_DEV *dev);

#endif  /* _SD_IO_H_ */
