/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWMEMORY_IMEMORYSERVICE_HPP_
#define _FWMEMORY_IMEMORYSERVICE_HPP_

#include <boost/cstdint.hpp>

#include <fwServices/IService.hpp>

#include "fwMemory/config.hpp"

namespace fwData
{
    class Object;
}

namespace fwMemory
{

class FWMEMORY_CLASS_API IMemoryService : public fwServices::IService
{

public:
    fwCoreServiceClassDefinitionsMacro ( (IMemoryService)(::fwServices::IService) ) ;

    FWMEMORY_API IMemoryService();

    FWMEMORY_API virtual ~IMemoryService();

    /// return string memory "memory"
    FWMEMORY_API virtual std::string getPersistanceId();

    // return virtual ( aka max possiblz ) memory size in Bytes
    FWMEMORY_API virtual unsigned long getVirtualSize() = 0;

protected :

    FWMEMORY_API virtual void configuring() throw(::fwTools::Failed) {} ;

    /// install observation +  register in MemoryPool singleton
    FWMEMORY_API virtual void starting() throw(::fwTools::Failed);

    /// implicit remove observation + unregister in MemoryPool singleton
    FWMEMORY_API virtual void stopping() throw(::fwTools::Failed);

    /// notify MemoryMonitor of change on data
    FWMEMORY_API virtual void updating() throw(::fwTools::Failed);

    /// update MemoryMonitor of change on data
    FWMEMORY_API virtual void updating( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg )  throw(::fwTools::Failed);

    FWMEMORY_API virtual void info(std::ostream &_sstream );

    static ::boost::uint64_t localTimer;

    // pointer on related object
    ::fwData::Object::wptr m_relatedObject;
    ::fwServices::IService::wptr m_comChannel;

};

} // namespace fwMemory

#endif // _FWMEMORY_IMEMORYSERVICE_HPP_
