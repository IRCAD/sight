/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEMORY_BUFFERINFO_HPP__
#define __FWMEMORY_BUFFERINFO_HPP__

#include "fwMemory/BufferAllocationPolicy.hpp"
#include "fwMemory/FileFormat.hpp"
#include "fwMemory/FileHolder.hpp"
#include "fwMemory/config.hpp"
#include "fwMemory/stream/in/IFactory.hpp"

#include <fwCore/LogicStamp.hpp>
#include <fwCore/macros.hpp>

#include <boost/filesystem/path.hpp>

namespace fwMemory
{

struct FWMEMORY_CLASS_API BufferInfo
{

    typedef size_t SizeType;
    typedef WPTR ( void ) CounterType;

    FWMEMORY_API BufferInfo();

    FWMEMORY_API void clear();
    //------------------------------------------------------------------------------

    long lockCount() const
    {
        return lockCounter.use_count();
    }

    SizeType size;
    /// true if 'buffer' is loaded
    bool loaded;
    bool userStreamFactory;

    FileHolder fsFile;
    FileFormatType fileFormat;

    CounterType lockCounter;

    ::fwCore::LogicStamp lastAccess;
    ::fwMemory::BufferAllocationPolicy::sptr bufferPolicy;

    SPTR( ::fwMemory::stream::in::IFactory ) istreamFactory;

};

} // namespace fwMemory

#endif // __FWMEMORY_BUFFERINFO_HPP__

