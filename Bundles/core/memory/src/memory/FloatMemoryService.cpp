/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Float.hpp>
#include <fwServices/Base.hpp>

#include "memory/FloatMemoryService.hpp"


namespace memory
{

// FIXME : no ckecking float because lost of perf
//REGISTER_SERVICE( ::fwMemory::IMemoryService, ::memory::FloatMemoryService , ::fwData::Float );

//-----------------------------------------------------------------------------

FloatMemoryService::FloatMemoryService()
{}

//-----------------------------------------------------------------------------

FloatMemoryService::~FloatMemoryService()
{}

//-----------------------------------------------------------------------------

unsigned long FloatMemoryService::getVirtualSize()
{
    ::fwData::Float::sptr v = this->getObject< ::fwData::Float>() ;

    if( v )
    {
        SLM_ASSERT("v not instanced", v);
        return sizeof(float);
    }
    return 0 ;
}

//-----------------------------------------------------------------------------

}
