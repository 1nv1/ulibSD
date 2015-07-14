/*
 *  integer.h
 *  Author: Nelson Lombardo (C) 2015
 *  e-mail: nelson.lombardo@gmail.com
 *  License: MIT
 */
 
/*****************************************************************************/
/* Integer type definitions                                                  */
/*****************************************************************************/
#ifndef _INTEGER_H_
#define _INTEGER_H_

#include <stdint.h>

/* 16-bit, 32-bit or larger integer */
typedef int16_t         INT;
typedef uint16_t        UINT;

/* 8-bit integer */
typedef int8_t          CHAR;
typedef uint8_t         UCHAR;
typedef uint8_t         BYTE;
typedef uint8_t         BOOL;

/* 16-bit integer */
typedef int16_t         SHORT;
typedef uint16_t        USHORT;
typedef uint16_t        WORD;
typedef uint16_t        WCHAR;

/* 32-bit integer */
typedef int32_t         LONG;
typedef uint32_t        ULONG;
typedef uint32_t        DWORD;

/* Boolean type */
typedef enum { FALSE = 0, TRUE } BOOLEAN;
typedef enum { LOW = 0, HIGH } THROTTLE;

#endif
