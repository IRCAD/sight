/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMemory/BufferInfo.hpp"

namespace fwMemory
{

BufferInfo::BufferInfo() :
    size(0),
    loaded(true),
    userStreamFactory(false),
    fileFormat(::fwMemory::OTHER)
{
    this->lastAccess.modified();
}


//-----------------------------------------------------------------------------

void BufferInfo::clear()
{
    size = 0;
    loaded = true;
    userStreamFactory = false;
    fsFile.clear();
    fileFormat = ::fwMemory::OTHER;
    lockCounter.reset();
    lastAccess = ::fwCore::LogicStamp();
    bufferPolicy.reset();
    istreamFactory.reset();
}

} // namespace fwMemory

