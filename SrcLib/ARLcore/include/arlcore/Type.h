/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ARLCORE_TYPE_H
#define _ARLCORE_TYPE_H

//const double PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679;

#if defined( _MSC_VER )

// The Visual C++ debugger doesn't know string
// larger than 255 char. STL often creates large
// symbol. The following line cancels warning
// created by Visual.
#pragma warning (disable : 4786)

// need a dll interface 
// (Un classe de base ou une structure doit être déclarée avec 
// le mot clé __declspec(dllexport) pour qu'une fonction d'une 
//classe dérivée soit exportée)
#pragma warning (disable:4251)

// The following line prevents from this file to
// be included several times by the compiler with
// Visual C++. This makes compiling quicker, but
// must not be used when your file content may
// change depending on some macro definitions. In
// the general case, leave these lines as they
// are.
#pragma once

#else

#include <typeinfo>

#endif

#include <cassert>

#ifdef __MACOSX__
#include <sys/types.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _SINT64
typedef int64_t sint64;
#define _SINT64
#endif
#ifndef _UINT64
typedef uint64_t uint64;
#define _UINT64
#endif
#ifndef _SINT32
typedef int32_t sint32;
#define _SINT32
#endif
#ifndef _SINT16
typedef int16_t sint16;
#define _SINT16
#endif
#ifndef _SINT8
typedef int8_t sint8;
#define _SINT8
#endif
#ifndef _UINT32
typedef uint32_t uint32;
#define _UINT32
#endif
#ifndef _UINT16
typedef uint16_t uint16;
#define _UINT16
#endif
#ifndef _UINT8
typedef uint8_t uint8;
#define _UINT8
#endif

#ifdef __cplusplus
}
#endif

#else
#ifdef __STDC__
typedef signed char int8;       // NB: non-ANSI compilers may not grok
#else
typedef char int8;
#endif
typedef unsigned char                           uint8;
typedef unsigned short                          uint16;
typedef short int16;
#if defined(__alpha) || (defined(_MIPS_SZLONG) && _MIPS_SZLONG == 64)
typedef int int32;
typedef unsigned int uint32;    // sizeof (uint32) must == 4
#else
typedef long int32;
typedef unsigned long uint32;   // sizeof (uint32) must == 4
#endif
typedef unsigned long long                      uint64;
#endif

typedef struct
{
    uint32 Low; //UINT32
    uint32 High; //UINT32
}uint32HL;

// Use only when you decide to delegate size to compiler
//typedef int       int;
typedef unsigned int    uint; 
typedef unsigned long   ulong;
typedef unsigned short  ushort;
typedef signed char     schar;
#ifndef uchar
    #ifndef _CV_H_
        typedef unsigned char   uchar;
    #endif
#endif

// For 64 bits
// For MSVC : __int64
//typedef long long int64;

#endif // _ARLCORE_TYPE_H
