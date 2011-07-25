/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _MEMORY_DUMP_IMAGEDUMPSERVICE_HPP_
#define _MEMORY_DUMP_IMAGEDUMPSERVICE_HPP_

#include <boost/filesystem/path.hpp>

#include <fwTools/Failed.hpp>

#include <fwData/IBufferDelegate.hpp>

#include <fwMemory/IDumpService.hpp>
#include <fwMemory/IMemoryService.hpp>

#include "memory/config.hpp"

namespace memory
{
namespace dump
{
class MEMORY_CLASS_API ImageDumpService : public ::fwMemory::IDumpService ,
                                            virtual public ::fwData::IBufferDelegate
{

public:

    fwCoreServiceClassDefinitionsMacro( (ImageDumpService)( ::fwMemory::IDumpService) );

    MEMORY_API ImageDumpService();

    MEMORY_API virtual ~ImageDumpService();

    /// perform dump only if status is restored : return true iff the dump action is performed
    MEMORY_API virtual void dump();

    /// perform restore only if status is dumped : return true iff the restore action is performed
    MEMORY_API virtual void restore();

    MEMORY_API virtual void * getBuffer();

    MEMORY_API virtual void setBuffer(void * _buffer);

    /// return the real state of RAM used
    MEMORY_API ::boost::uint64_t  getCurrentSize();

protected :

    ::fwMemory::IMemoryService::sptr getMemoryService();
    void updateMemoryService();

    MEMORY_API virtual void starting() throw( ::fwTools::Failed );
    MEMORY_API virtual void stopping() throw( ::fwTools::Failed );
    MEMORY_API virtual void info(std::ostream &_sstream );

    ///  call update MemoryInfo via several way
    void updateTimeStamp();
    void updateVirtualSize();
    void updateCurrentSize();
    ::boost::uint64_t getVirtualSize();

    // reserve memory when restore image
    bool requestMemory();

    ::boost::uint64_t  m_currentSize;

    ::boost::filesystem::path m_fsFilename;

    ::boost::weak_ptr< ::fwData::Image > m_image ;

    ::fwMemory::IMemoryService::wptr m_memoryService;
};
} //namespace dump
} // namespace memory

#endif //_MEMORY_DUMP_IMAGEDUMPSERVICE_HPP_
