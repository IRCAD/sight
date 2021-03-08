/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

/* ioapi_mem.h -- IO base function header for compress/uncompress .zip
   files using zlib + zip or unzip API

   This version of ioapi is designed to access memory rather than files.
   We do use a region of memory to put data in to and take it out of.

   Copyright (C) 2012-2017 Nathan Moinvaziri (https://github.com/nmoinvaz/minizip)
             (C) 2003 Justin Fletcher
             (C) 1998-2003 Gilles Vollant

   This program is distributed under the terms of the same license as zlib.
   See the accompanying LICENSE file for the full text of the license.
 */

#pragma once

#ifndef _IOAPI_MEM_H
#define _IOAPI_MEM_H

#include "ioapi.h"
#include "zlib.h"

#include <stdio.h>
#include <string.h>

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

voidpf ZCALLBACK fopen_mem_func(voidpf opaque, const char* filename, int mode);
voidpf ZCALLBACK fopendisk_mem_func(voidpf opaque, voidpf stream, uint32_t number_disk, int mode);
uint32_t ZCALLBACK fread_mem_func(voidpf opaque, voidpf stream, void* buf, uint32_t size);
uint32_t ZCALLBACK fwrite_mem_func(voidpf opaque, voidpf stream, const void* buf, uint32_t size);
long ZCALLBACK ftell_mem_func(voidpf opaque, voidpf stream);
long ZCALLBACK fseek_mem_func(voidpf opaque, voidpf stream, uint32_t offset, int origin);
int ZCALLBACK fclose_mem_func(voidpf opaque, voidpf stream);
int ZCALLBACK ferror_mem_func(voidpf opaque, voidpf stream);

typedef struct ourmemory_s {
    char* base;          /* Base of the region of memory we're using */
    uint32_t size;       /* Size of the region of memory we're using */
    uint32_t limit;      /* Furthest we've written */
    uint32_t cur_offset; /* Current offset in the area */
    int grow;            /* Growable memory buffer */
} ourmemory_t;

void fill_memory_filefunc(zlib_filefunc_def* pzlib_filefunc_def, ourmemory_t* ourmem);

#ifdef __cplusplus
}
#endif

#endif
