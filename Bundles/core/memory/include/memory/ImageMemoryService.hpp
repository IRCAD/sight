/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _MEMORY_IMAGEMEMORYSERVICE_HPP_
#define _MEMORY_IMAGEMEMORYSERVICE_HPP_

#include <fwMemory/IMemoryService.hpp>
#include "memory/config.hpp"

namespace memory
{

class MEMORY_CLASS_API ImageMemoryService : public ::fwMemory::IMemoryService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (ImageMemoryService)( ::fwMemory::IMemoryService) ) ;
    MEMORY_API ImageMemoryService();

    MEMORY_API virtual ~ImageMemoryService();

protected:

    /// update MemoryMonitor of change on data
    MEMORY_API virtual void updating( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg )  throw(::fwTools::Failed);

    MEMORY_API unsigned long getVirtualSize();

};

} // namespace memory

#endif // _MEMORY_IMAGEMEMORYSERVICE_HPP_
