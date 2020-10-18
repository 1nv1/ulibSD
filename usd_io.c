/*
 *  File: usd_io.c
 *  Author: Nelson Lombardo
 *  Year: 2015
 *  e-mail: nelson.lombardo@gmail.com
 *  License at the end of file.
 */

#include "usd_io.h"

#ifdef _M_IX86  // For use over x86

/*****************************************************************************/
/* Private Methods Prototypes - Direct work with PC file                     */
/*****************************************************************************/

/**
 * \brief Get the total numbers of sectors in SD card.
 * \param dev Device descriptor.
 * \return Quantity of sectors. Zero if fail.
 */
USD_DWORD __uSD_Sectors (uSD_DEV* dev);

/*****************************************************************************/
/* Private Methods - Direct work with PC file                                */
/*****************************************************************************/

USD_DWORD __uSD_Sectors (uSD_DEV *dev) {
  if (dev->fp == NULL) {
    return(0); // Fail
  } else {
    fseek(dev->fp, 0L, SEEK_END);
    return (((USD_DWORD)(ftell(dev->fp)))/((USD_DWORD)512) - 1);
  }
}

#else   // For use with uControllers

/******************************************************************************
 Private Methods Prototypes - Direct work with SD card
******************************************************************************/

/**
  \brief Simple function to calculate power of two.
  \param e Exponent.
  \return Math function result.
*/
USD_DWORD __uSD_Power_Of_Two(BYTE e);

/**
  \brief Assert the SD card (SPI CS low).
*/
inline void __uSD_Assert (void);

/**
  \brief Deassert the SD (SPI CS high).
*/
inline void __uSD_Deassert (void);

/**
  \brief Change to max the speed transfer.
  \param throttle
*/
void __uSD_Speed_Transfer (BYTE throttle);

/**
  \brief Send SPI commands.
  \param cmd Command to send.
  \param arg Argument to send.
  \return R1 response.
*/
BYTE __uSD_Send_Cmd(BYTE cmd, USD_DWORD arg);

/**
  \brief Write a data block on SD card.
  \param dat Storage the data to transfer.
  \param token Inidicates the type of transfer (single or multiple).
*/
USD_RESULTS __uSD_Write_Block(uSD_DEV *dev, void *dat, BYTE token);

/**
  \brief Get the total numbers of sectors in SD card.
  \param dev Device descriptor.
  \return Quantity of sectors. Zero if fail.
*/
UDF_DWORD __uSD_Sectors (uSD_DEV *dev);

/******************************************************************************
 Private Methods - Direct work with SD card
******************************************************************************/

UDWORD __uSD_Power_Of_Two(BYTE e) {
  DWORD partial = 1;
  BYTE idx;
  for (idx = 0; idx != e; idx++)
    partial *= 2;
  return (partial);
}

inline void __uSD_Assert(void){
  uSD_SPI_CS_Low();
}

inline void __uSD_Deassert(void){
  uSD_SPI_CS_High();
}

void __uSD_Speed_Transfer(BYTE throttle) {
  if (throttle == HIGH)
    uSD_SPI_Freq_High();
  else
    uSD_SPI_Freq_Low();
}

BYTE __uSD_Send_Cmd(BYTE cmd, USD_DWORD arg)
{
  BYTE crc, res;
  // ACMD«n» is the command sequense of CMD55-CMD«n»
  if (cmd & 0x80) {
    cmd &= 0x7F;
    res = __uSD_Send_Cmd(CMD55, 0);
    if (res > 1) return (res);
  }

  // Select the card
  __uSD_Deassert();
  uSD_SPI_RW(0xFF);
  __uSD_Assert();
  uSD_SPI_RW(0xFF);

  // Send complete command set
  uSD_SPI_RW(cmd);                // Start and command index
  uSD_SPI_RW((BYTE)(arg >> 24));  // Arg[31-24]
  uSD_SPI_RW((BYTE)(arg >> 16));  // Arg[23-16]
  uSD_SPI_RW((BYTE)(arg >> 8 ));  // Arg[15-08]
  uSD_SPI_RW((BYTE)(arg >> 0 ));  // Arg[07-00]

  // CRC?
  crc = 0x01;                     // Dummy CRC and stop
  if (cmd == CMD0)
    crc = 0x95;                   // Valid CRC for CMD0(0)
  if (cmd == CMD8)
    crc = 0x87;                   // Valid CRC for CMD8(0x1AA)
  uSD_SPI_RW(crc);

  // Receive command response
  // Wait for a valid response in timeout of 5 milliseconds
  uSD_SPI_Timer_On(5);
  do {
    res = uSD_SPI_RW(0xFF);
  } while ((res & 0x80)&&(uSD_SPI_Timer_Status() == TRUE));
  uSD_SPI_Timer_Off();
  // Return with the response value
  return(res);
}

USD_RESULTS __SD_Write_Block(SD_DEV *dev, void *dat, BYTE token)
{
  USD_WORD idx;
  BYTE line;
  // Send token (single or multiple)
  uSD_SPI_RW(token);
  // Single block write?
  if (token != 0xFD) {
    // Send block data
    for (idx = 0; idx != USD_BLK_SIZE; idx++)
      uSD_SPI_RW(*((BYTE*)dat + idx));
    /* Dummy CRC */
    uSD_SPI_RW(0xFF);
    uSD_SPI_RW(0xFF);
    // If not accepted, returns the reject error
    if ((uSD_SPI_RW(0xFF) & 0x1F) != 0x05)
      return(USD_REJECT);
  }
#ifdef USD_IO_WRITE_WAIT_BLOCKER
  // Waits until finish of data programming (blocked)
  while(uSD_SPI_RW(0xFF) == 0);
  return(USD_OK);
#else
  // Waits until finish of data programming with a timeout
  uSD_SPI_Timer_On(SD_IO_WRITE_TIMEOUT_WAIT);
  do {
    line = uSD_SPI_RW(0xFF);
  } while ((line == 0) && (uSD_SPI_Timer_Status() == TRUE));
  uSD_SPI_Timer_Off();
#ifdef SD_IO_DBG_COUNT
  dev->debug.write++;
#endif
  if (line == 0)
    return(USD_BUSY);
  else
    return(USD_OK);
#endif
}

USD_DWORD __uSD_Sectors (uSD_DEV *dev)
{
  BYTE csd[16];
  BYTE idx;
  USD_DWORD ss = 0;
  USD_WORD C_SIZE = 0;
  BYTE C_SIZE_MULT = 0;
  BYTE READ_BL_LEN = 0;
  if (__uSD_Send_Cmd(CMD9, 0) == 0) {
    // Wait for response
    while (uSPI_RW(0xFF) == 0xFF);
    for (idx = 0; idx != 16; idx++)
      csd[idx] = uSD_SPI_RW(0xFF);
    // Dummy CRC
    uSD_SPI_RW(0xFF);
    uSD_SPI_RW(0xFF);
    uSD_SPI_Release();
    if (dev->cardtype & SDCT_SD1) {
      ss = csd[0];
      // READ_BL_LEN[83:80]: max. read data block length
      READ_BL_LEN = (csd[5] & 0x0F);
      // C_SIZE [73:62]
      C_SIZE = (csd[6] & 0x03);
      C_SIZE <<= 8;
      C_SIZE |= (csd[7]);
      C_SIZE <<= 2;
      C_SIZE |= ((csd[8] >> 6) & 0x03);
      // C_SIZE_MULT [49:47]
      C_SIZE_MULT = (csd[9] & 0x03);
      C_SIZE_MULT <<= 1;
      C_SIZE_MULT |= ((csd[10] >> 7) & 0x01);
    } else if (dev->cardtype & SDCT_SD2) {
      // C_SIZE [69:48]
      C_SIZE = (csd[7] & 0x3F);
      C_SIZE <<= 8;
      C_SIZE |= (csd[8] & 0xFF);
      C_SIZE <<= 8;
      C_SIZE |= (csd[9] & 0xFF);
      // C_SIZE_MULT [--]. don't exits
      C_SIZE_MULT = 0;
    }
    ss = (C_SIZE + 1);
    ss *= __uSD_Power_Of_Two(C_SIZE_MULT + 2);
    ss *= __uSD_Power_Of_Two(READ_BL_LEN);
    ss /= uSD_BLK_SIZE;
    return (ss);
  } else {
    return (0); // Error
  }
}
#endif // Private methods for uC

/******************************************************************************
 Public Methods - Direct work with SD card
******************************************************************************/

USD_RESULTS uSD_Init(uSD_DEV *dev)
{
#if defined(_M_IX86)    // x86
  dev->fp = fopen(dev->fn, "r+");
  if (dev->fp == NULL) {
    return (uSD_ERROR);
  } else {
    dev->last_sector = __SD_Sectors(dev);
#ifdef USD_IO_DBG_COUNT
    dev->debug.read = 0;
    dev->debug.write = 0;
#endif
    return (USD_OK);
  }
#else   // uControllers
  BYTE n, cmd, ct, ocr[4];
  BYTE idx;
  BYTE init_trys;
  ct = 0;
  for (init_trys = 0; ((init_trys != USD_INIT_TRYS) && (!ct)); init_trys++) {
    // Initialize SPI for use with the memory card
    uSD_SPI_Init();
    uSD_SPI_CS_High();
    uSD_SPI_Freq_Low();
    // 80 dummy clocks
    for (idx = 0; idx != 10; idx++)
      uSD_SPI_RW(0xFF);

    uSD_SPI_Timer_On(500);
    while (uSD_SPI_Timer_Status() == TRUE);
    uSD_SPI_Timer_Off();

    dev->mount = FALSE;
    uSD_SPI_Timer_On(500);
    while ((__uSD_Send_Cmd(CMD0, 0) != 1) && (uSD_SPI_Timer_Status() == TRUE));
    uSD_SPI_Timer_Off();
    // Idle state
    if (__uSD_Send_Cmd(CMD0, 0) == 1) {
      // SD version 2?
      if (__uSD_Send_Cmd(CMD8, 0x1AA) == 1) {
        // Get trailing return value of R7 resp
        for (n = 0; n < 4; n++)
          ocr[n] = uSD_SPI_RW(0xFF);

        // VDD range of 2.7-3.6V is OK?
        if ((ocr[2] == 0x01)&&(ocr[3] == 0xAA)) {
          // Wait for leaving idle state (ACMD41 with HCS bit)...
          uSD_SPI_Timer_On(1000);
          while ((SPI_Timer_Status() == TRUE) && (__uSD_Send_Cmd(ACMD41, 1UL << 30)));
          uSD_SPI_Timer_Off();
          // CCS in the OCR?
          if ((uSD_SPI_Timer_Status() == TRUE) && (__uSD_Send_Cmd(CMD58, 0) == 0)) {
            for (n = 0; n < 4; n++)
              ocr[n] = uSD_SPI_RW(0xFF);
            // SD version 2?
            ct = (ocr[0] & 0x40) ? USD_CT_SD2 | USD_CT_BLOCK : USD_CT_SD2;
          }
        }
      } else {
        // SD version 1 or MMC?
        if (__uSD_Send_Cmd(ACMD41, 0) <= 1) {
          // SD version 1
          ct = SDCT_SD1;
          cmd = ACMD41;
        } else {
          // MMC version 3
          ct = USD_CT_MMC;
          cmd = USD_CMD1;
        }
        // Wait for leaving idle state
        uSD_SPI_Timer_On(250);
        while ((uSD_SPI_Timer_Status() == TRUE) && (__uSD_Send_Cmd(cmd, 0)));
        uSD_SPI_Timer_Off();
        if (uSD_SPI_Timer_Status() == FALSE)
          ct = 0;
        if (__uSD_Send_Cmd(CMD59, 0))
          ct = 0;   // Deactivate CRC check (default)
        if (__uSD_Send_Cmd(CMD16, 512))
          ct = 0;   // Set R/W block length to 512 bytes
      }
    }
  }
  if (ct) {
    dev->cardtype = ct;
    dev->mount = TRUE;
    dev->last_sector = __SD_Sectors(dev) - 1;
#ifdef USD_IO_DBG_COUNT
    dev->debug.read = 0;
    dev->debug.write = 0;
#endif
    __uSD_Speed_Transfer(HIGH); // High speed transfer
  }
  uSD_SPI_Release();
  return(ct ? USD_OK : USD_NOINIT);
#endif
}

USD_RESULTS uSD_Read(uSD_DEV *dev, void *dat, USD_DWORD sector, USD_WORD ofs, USD_WORD cnt) {
#if defined(_M_IX86)    // x86
  // Check the sector query
  if ((sector > dev->last_sector)||(cnt == 0))
    return(USD_PARERR);

  if (dev->fp!=NULL) {
    if (fseek(dev->fp, ((512 * sector) + ofs), SEEK_SET)!=0) {
      return (USD_ERROR);
    } else {
      if (fread(dat, 1, (cnt - ofs),dev->fp)==(cnt - ofs)) {
#ifdef USD_IO_DBG_COUNT
        dev->debug.read++;
#endif
        return(USD_OK);
      } else {
        return(USD_ERROR);
      }
    }
  } else {
    return(USD_ERROR);
  }
#else // uControllers
  USD_RESULTS res;
  BYTE tkn;
  USD_WORD remaining;
  res = USD_ERROR;
  if ((sector > dev->last_sector)||(cnt == 0))
    return(USD_PARERR);
  // Convert sector number to byte address (sector * SD_BLK_SIZE)
  if (__uSD_Send_Cmd(CMD17, sector * SD_BLK_SIZE) == 0) {
    uSD_SPI_Timer_On(100);  // Wait for data packet (timeout of 100ms)
    do {
      tkn = uSD_SPI_RW(0xFF);
    } while ((tkn == 0xFF) && (uSD_SPI_Timer_Status() == TRUE));
    SPI_Timer_Off();
    // Token of single block?
    if (tkn == 0xFE) {
      // Size block (512 bytes) + CRC (2 bytes) - offset - bytes to count
      remaining = USD_BLK_SIZE + 2 - ofs - cnt;
      // Skip offset
      if (ofs) {
        do {
          uSD_SPI_RW(0xFF);
        } while(--ofs);
      }
      // I receive the data and I write in user's buffer
      do {
        *(BYTE*)dat = uSD_SPI_RW(0xFF);
        dat++;
      } while(--cnt);
      // Skip remaining
      do {
        uSD_SPI_RW(0xFF);
      } while (--remaining);
      res = USD_OK;
    }
  }
  uSD_SPI_Release();
#ifdef USD_IO_DBG_COUNT
  dev->debug.read++;
#endif
  return(res);
#endif
}

#ifdef USD_IO_WRITE
USD_RESULTS uSD_Write(uSD_DEV *dev, void *dat, USD_DWORD sector) {
#if defined(_M_IX86)    // x86
  // Query ok?
  if (sector > dev->last_sector)
    return(USD_PARERR);

  if (dev->fp != NULL) {
    if (fseek(dev->fp, USD_BLK_SIZE * sector, SEEK_SET) != 0) {
      return(USD_ERROR);
    } else {
      if (fwrite(dat, 1, USD_BLK_SIZE, dev->fp) == USD_BLK_SIZE) {
#ifdef USD_IO_DBG_COUNT
        dev->debug.write++;
#endif
        return(USD_OK);
      } else {
        return(USD_ERROR);
      }
    }
  } else {
    return(USD_ERROR);
  }
#else // uControllers
  // Query ok?
  if (sector > dev->last_sector)
    return(USD_PARERR);

  // Single block write (token <- 0xFE)
  // Convert sector number to bytes address (sector * SD_BLK_SIZE)
  if (__uSD_Send_Cmd(CMD24, sector * USD_BLK_SIZE)==0) {
    return(__uSD_Write_Block(dev, dat, 0xFE));
  } else {
    return(USD_ERROR);
  }
#endif
}
#endif

USD_RESULTS uSD_Status(uSD_DEV *dev) {
#if defined(_M_IX86)
  return((dev->fp == NULL) ? USD_OK : USD_NORESPONSE);
#else
  return(__uSD_Send_Cmd(CMD0, 0) ? USD_OK : USD_NORESPONSE);
#endif
}

// «usd_io.c» is part of:
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
