/*
 *  File: usd_types.h
 *  Author: Nelson Lombardo
 *  Year: 2015-2020
 *  e-mail: nelson.lombardo@gmail.com
 *  License at the end of file.
 */

/******************************************************************************/
/* Types definitions                                                          */
/******************************************************************************/
#ifndef _USD_TYPES_H_
#define _USD_TYPES_H_

#include <stdint.h>

/* 16-bit, 32-bit or larger integer */
typedef int16_t   USD_INT;
typedef uint16_t  USD_UINT;

/* 8-bit integer */
typedef int8_t    USD_CHAR;
typedef uint8_t   USD_UCHAR;
typedef uint8_t   USD_BYTE;
typedef uint8_t   USD_BOOL;

/* 16-bit integer */
typedef int16_t   USD_SHORT;
typedef uint16_t  USD_USHORT;
typedef uint16_t  USD_WORD;
typedef uint16_t  USD_WCHAR;

/* 32-bit integer */
typedef int32_t   USD_LONG;
typedef uint32_t  USD_ULONG;
typedef uint32_t  USD_DWORD;

/* Boolean type */
typedef enum { FALSE = 0, TRUE } USD_BOOLEAN;
typedef enum { LOW = 0, HIGH }   USD_THROTTLE;

#endif

// «usd_types.h» is part of:
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
