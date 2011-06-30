/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _MEMORY_FLOATMEMORYSERVICE_H_
#define _MEMORY_FLOATMEMORYSERVICE_H_

#include <fwMemory/IMemoryService.hpp>

#include "memory/config.hpp"

namespace memory
{

class MEMORY_CLASS_API FloatMemoryService : public fwMemory::IMemoryService
{
public:

    fwCoreServiceClassDefinitionsMacro ( (FloatMemoryService)( fwMemory::IMemoryService) ) ;

    MEMORY_API FloatMemoryService();

    MEMORY_API virtual ~FloatMemoryService();

    MEMORY_API unsigned long getVirtualSize();
};

}

#endif /*_MEMORY_FLOATMEMORYSERVICE_H_*/
