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

/* iowin32.h -- IO base function header for compress/uncompress .zip
   Version 1.2.0, September 16th, 2017
   part of the MiniZip project

   Copyright (C) 2012-2017 Nathan Moinvaziri
     https://github.com/nmoinvaz/minizip
   Copyright (C) 2009-2010 Mathias Svensson
     Modifications for Zip64 support
     http://result42.com
   Copyright (C) 1998-2010 Gilles Vollant
     http://www.winimage.com/zLibDll/minizip.html

   This program is distributed under the terms of the same license as zlib.
   See the accompanying LICENSE file for the full text of the license.
 */

#pragma once

#ifdef WIN32

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

void fill_win32_filefunc(zlib_filefunc_def* pzlib_filefunc_def);
void fill_win32_filefunc64(zlib_filefunc64_def* pzlib_filefunc_def);
void fill_win32_filefunc64A(zlib_filefunc64_def* pzlib_filefunc_def);
void fill_win32_filefunc64W(zlib_filefunc64_def* pzlib_filefunc_def);

#ifdef __cplusplus
}
#endif

#endif // WIN32
