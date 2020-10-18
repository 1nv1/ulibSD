/*
 *  File: usd_io.h
 *  Author: Nelson Lombardo
 *  Year: 2015
 *  e-mail: nelson.lombardo@gmail.com
 *  License at the end of file.
 */

#ifndef _USD_IO_H_
#define _USD_IO_H_

/*****************************************************************************/
/* Configurations                                                            */
/*****************************************************************************/
//#define _M_IX86           // For use with x86 architecture
#define USD_IO_WRITE
//#define uSD_IO_WRITE_WAIT_BLOCKER
#define USD_IO_WRITE_TIMEOUT_WAIT 250

//#define SD_IO_DBG_COUNT
/*****************************************************************************/

#if defined(_M_IX86)

#include <stdio.h>
#include "usd_types.h"

/* Results of uSD functions */
typedef enum {
  USD_OK = 0,      /* 0: Function succeeded    */
  USD_NOINIT,      /* 1: SD not initialized    */
  USD_ERROR,       /* 2: Disk error            */
  USD_PARERR,      /* 3: Invalid parameter     */
  USD_BUSY,        /* 4: Programming busy      */
  USD_REJECT       /* 5: Reject data           */
} SDRESULTS;

#ifdef uSD_IO_DBG_COUNT
typedef struct _DBG_COUNT {
  USD_WORD read;
  USD_WORD write;
} DBG_COUNT;
#endif

/* SD device object */
typedef struct _uSD_DEV {
  BOOL mount;
  BYTE cardtype;
  char fn[20]; /* dd if=/dev/zero of=sim_sd.raw bs=1k count=0 seek=8192 */
  FILE *fp;
  USD_DWORD last_sector;
#ifdef SD_IO_DBG_COUNT
  DBG_COUNT debug;
#endif
} uSD_DEV;

#else // For use with uControllers

#include "usd_spi.h" /* Provide the low-level functions */

/* Definitions of SD commands */
#define USD_CMD0    (0x40+0)        /* GO_IDLE_STATE            */
#define USD_CMD1    (0x40+1)        /* SEND_OP_COND (MMC)       */
#define USD_ACMD41  (0xC0+41)       /* SEND_OP_COND (SDC)       */
#define USD_CMD8    (0x40+8)        /* SEND_IF_COND             */
#define USD_CMD9    (0x40+9)        /* SEND_CSD                 */
#define USD_CMD16   (0x40+16)       /* SET_BLOCKLEN             */
#define USD_CMD17   (0x40+17)       /* READ_SINGLE_BLOCK        */
#define USD_CMD24   (0x40+24)       /* WRITE_SINGLE_BLOCK       */
#define USD_CMD42   (0x40+42)       /* LOCK_UNLOCK              */
#define USD_CMD55   (0x40+55)       /* APP_CMD                  */
#define USD_CMD58   (0x40+58)       /* READ_OCR                 */
#define USD_CMD59   (0x40+59)       /* CRC_ON_OFF               */

#define USD_INIT_TRYS    0x03

/* CardType) */
#define USD_CT_MMC       0x01                     /* MMC version 3    */
#define USD_CT_SD1       0x02                     /* SD version 1     */
#define USD_CT_SD2       0x04                     /* SD version 2     */
#define USD_CT_SDC       (USD_CT_SD1|USD_CT_SD2)  /* SD               */
#define USD_CT_BLOCK     0x08                     /* Block addressing */

#define USD_BLK_SIZE     512

/* Results of uSD functions */
typedef enum {
    USD_OK = 0,      /* 0: Function succeeded    */
    USD_NOINIT,      /* 1: SD not initialized    */
    USD_ERROR,       /* 2: Disk error            */
    USD_PARERR,      /* 3: Invalid parameter     */
    USD_BUSY,        /* 4: Programming busy      */
    USD_REJECT,      /* 5: Reject data           */
    USD_NORESPONSE   /* 6: No response           */
} USD_RESULTS;

#ifdef USD_IO_DBG_COUNT
typedef struct _DBG_COUNT {
  USD_WORD read;
  USD_WORD write;
} DBG_COUNT;
#endif

/* uSD device object */
typedef struct _uSD_DEV {
  BOOL mount;
  BYTE cardtype;
  USD_DWORD last_sector;
#ifdef USD_IO_DBG_COUNT
  DBG_COUNT debug;
#endif
} uSD_DEV;

#endif

/*******************************************************************************
 * Public Methods - Direct work with SD card                                   *
 ******************************************************************************/

/**
  \brief Initialization the SD card.
  \return If all goes well returns USD_OK.
*/
USD_RESULTS uSD_Init (uSD_DEV *dev);

/**
  \brief Read a single block.
  \param dest Pointer to the destination object to put data
  \param sector Start sector number (internally is converted to byte address).
  \param ofs Byte offset in the sector (0..511).
  \param cnt Byte count (1..512).
  \return If all goes well returns SD_OK.
*/
USD_RESULTS uSD_Read (uSD_DEV *dev, void *dat, USD_DWORD sector, USD_WORD ofs, USD_WORD cnt);

/**
    \brief Write a single block.
    \param dat Data to write.
    \param sector Sector number to write (internally is converted to byte address).
    \return If all goes well returns USD_OK.
 */
USD_RESULTS uSD_Write (uSD_DEV *dev, void *dat, USD_DWORD sector);

/**
    \brief Allows know status of SD card.
    \return If all goes well returns USD_OK.
*/
USD_RESULTS uSD_Status (uSD_DEV *dev);

#endif

// «usd_io.h» is part of:
/*----------------------------------------------------------------------------/
/  ulibSD - Library for SD cards semantics            (C)Nelson Lombardo, 2015
/-----------------------------------------------------------------------------/
/ ulibSD library is a free software that opened under license policy of
/ following conditions.
/
/ Copyright (C) 2015, ChaN, all right reserved.
/
/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/----------------------------------------------------------------------------*/

// Derived from Mister Chan works on FatFs code (http://elm-chan.org/fsw/ff/00index_e.html):
/*----------------------------------------------------------------------------/
/  FatFs - FAT file system module  R0.11                 (C)ChaN, 2015
/-----------------------------------------------------------------------------/
/ FatFs module is a free software that opened under license policy of
/ following conditions.
/
/ Copyright (C) 2015, ChaN, all right reserved.
/
/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/----------------------------------------------------------------------------*/
