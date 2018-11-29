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
