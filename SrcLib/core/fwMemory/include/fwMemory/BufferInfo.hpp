/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWMEMORY_BUFFERINFO_HPP_
#define FWMEMORY_BUFFERINFO_HPP_

#include <boost/filesystem/path.hpp>

#include <fwCore/LogicStamp.hpp>

#include <fwTools/IBufferManager.hpp>

#include "fwMemory/config.hpp"

namespace fwMemory
{

struct FWMEMORY_CLASS_API BufferInfo
{

    typedef std::map< void **,  BufferInfo > MapType;

    typedef ::fwTools::IBufferManager::SizeType SizeType;

    FWMEMORY_API BufferInfo()
    {
        this->size = 0;
        this->lockCount = NULL;
        this->isDumped = false;
        this->lastAccess.modified();
    }

    SizeType size;
    bool     isDumped;
    ::boost::filesystem::path dumpedFile;
    long * lockCount;
    ::fwCore::LogicStamp lastAccess;
    ::fwTools::BufferAllocationPolicy::sptr bufferPolicy;


};




} // namespace fwMemory

#endif // FWMEMORY_BUFFERINFO_HPP_

