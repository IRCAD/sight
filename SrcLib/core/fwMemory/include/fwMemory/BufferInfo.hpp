/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWMEMORY_BUFFERINFO_HPP_
#define FWMEMORY_BUFFERINFO_HPP_

#include <boost/filesystem/path.hpp>

#include <fwCore/LogicStamp.hpp>

#include "fwMemory/stream/in/IFactory.hpp"
#include "fwMemory/IBufferManager.hpp"
#include "fwMemory/config.hpp"

namespace fwMemory
{

struct FWMEMORY_CLASS_API BufferInfo
{

    typedef std::map< ::fwMemory::IBufferManager::BufferPtrType, BufferInfo > MapType;

    typedef ::fwMemory::IBufferManager::SizeType SizeType;

    FWMEMORY_API BufferInfo()
    {
        this->size = 0;
        this->isDumped = false;
        this->lastAccess.modified();
    }

    SizeType size;
    bool     isDumped;
    ::boost::filesystem::path dumpedFile;
    ::fwMemory::IBufferManager::LockCountFunctionType lockCount;
    ::fwCore::LogicStamp lastAccess;
    ::fwMemory::BufferAllocationPolicy::sptr bufferPolicy;

    SPTR( ::fwMemory::stream::in::IFactory ) istreamFactory;


};




} // namespace fwMemory

#endif // FWMEMORY_BUFFERINFO_HPP_

