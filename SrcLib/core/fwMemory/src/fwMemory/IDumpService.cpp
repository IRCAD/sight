/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwData/Object.hpp>
#include <fwTools/System.hpp>
#include <fwServices/Base.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include "fwMemory/IDumpService.hpp"
#include "fwMemory/IMemoryService.hpp"
#include "fwMemory/MemoryMonitor.hpp"

namespace fwMemory
{

//------------------------------------------------------------------------------

::boost::filesystem::path IDumpService::m_dumpFolder( ::fwTools::System::getDefault()->getDumpFolder() );

//------------------------------------------------------------------------------

IDumpService::IDumpService() :
    m_status ( IDumpService::NOTSET ),
    m_dumpLockCount ( 0 )
{}

//------------------------------------------------------------------------------

IDumpService::~IDumpService()
{}

//------------------------------------------------------------------------------

void IDumpService::info(std::ostream &_sstream )
{
    _sstream << "Dump Service";
}

//------------------------------------------------------------------------------

IDumpService::DumpStatus IDumpService::getStatus()
{
    return m_status;
}


void IDumpService::setDumpIsLocked( bool _bDumpIsLocked )
{
    _bDumpIsLocked?requestLock():releaseLock();
}


//------------------------------------------------------------------------------

void IDumpService::requestLock()
{
    SLM_ASSERT("m_dumpLockCount must be >= 0", m_dumpLockCount>=0 )
    m_dumpLockCount++;

    // Update MemoryMonitor
    ::fwData::Object::sptr obj = this->getObject< ::fwData::Object >();
    SLM_ASSERT("obj not instanced", obj);
    MemoryMonitor::MemoryInfo memInfo = MemoryMonitor::getDefault()->getStatus( obj );
    memInfo.dumpLockCount = m_dumpLockCount;
    MemoryMonitor::getDefault()->statusChanged( obj, memInfo );
}

void IDumpService::releaseLock()
{
    SLM_ASSERT("IDumpService::releaseLock() with m_dumpLockCount == 0 !! ", m_dumpLockCount>0 );
    m_dumpLockCount--;

    // Update MemoryMonitor
    ::fwData::Object::sptr obj = this->getObject< ::fwData::Object >();
    SLM_ASSERT("obj not instanced", obj);
    MemoryMonitor::MemoryInfo memInfo = MemoryMonitor::getDefault()->getStatus( obj );
    memInfo.dumpLockCount = m_dumpLockCount;
    MemoryMonitor::getDefault()->statusChanged( obj, memInfo );
}

//------------------------------------------------------------------------------

bool IDumpService::getDumpIsLocked() const
{
    return m_dumpLockCount>0;
}


unsigned long  IDumpService::getDumpLockCount() const
{
    return m_dumpLockCount;
}

//------------------------------------------------------------------------------

const ::boost::filesystem::path  & IDumpService::getDumpFolder()
{
    return m_dumpFolder;
}

//------------------------------------------------------------------------------

void IDumpService::setDumpFolder(::boost::filesystem::path dumpFolder )
{
    m_dumpFolder = dumpFolder;
}

//------------------------------------------------------------------------------

} // namespace fwMemory
