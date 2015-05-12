/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEMORY_FILEFORMAT_HPP__
#define __FWMEMORY_FILEFORMAT_HPP__

namespace fwMemory
{

typedef enum
{
    OTHER = 0,
    RAW   = 1,
    RAWZ  = 1 << 2
} FileFormatType;

} // namespace fwMemory

#endif // __FWMEMORY_FILEFORMAT_HPP__

