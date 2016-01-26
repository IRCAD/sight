/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/*
   Additional tools for Minizip
   Code: Xavier Roche '2004
   License: Same as ZLIB (www.gzip.org)
 */

#ifndef __MINIZIP_MZTOOLS_H__
#define __MINIZIP_MZTOOLS_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ZLIB_H
#include <zlib.h>
#endif

#include "minizip/unzip.h"

/* Repair a ZIP file (missing central directory)
   file: file to recover
   fileOut: output file after recovery
   fileOutTmp: temporary file name used for recovery
 */
extern MINIZIP_API int unzRepair(const char* file,
                                 const char* fileOut,
                                 const char* fileOutTmp,
                                 uLong* nRecovered,
                                 uLong* bytesRecovered);

#endif //__MINIZIP_MZTOOLS_H__
