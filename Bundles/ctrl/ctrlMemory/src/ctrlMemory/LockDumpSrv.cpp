/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlMemory/LockDumpSrv.hpp"

#include <fwServices/macros.hpp>

namespace ctrlMemory
{

static const ::fwServices::IService::KeyType s_TARGET_INOUT = "target";

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwServices::IController, ::ctrlMemory::LockDumpSrv, ::fwData::Object );

//-----------------------------------------------------------------------------

LockDumpSrv::LockDumpSrv() noexcept
{
}

//-----------------------------------------------------------------------------

LockDumpSrv::~LockDumpSrv() noexcept
{
}

//-----------------------------------------------------------------------------

void LockDumpSrv::starting()
{
    ::fwData::Object::sptr object = this->getInOut< ::fwData::Object >(s_TARGET_INOUT);
    if (!object)
    {
        FW_DEPRECATED_MSG("'LockDumpSrv' must have an <inout> 'target'");
        object = this->getObject();
    }
    m_objLock = ::fwData::ObjectLock( object );
}

//-----------------------------------------------------------------------------

void LockDumpSrv::stopping()
{
    m_objLock = ::fwData::ObjectLock();
}

//-----------------------------------------------------------------------------

void LockDumpSrv::configuring()
{
}

//-----------------------------------------------------------------------------

void LockDumpSrv::reconfiguring()
{
}

//-----------------------------------------------------------------------------

void LockDumpSrv::updating()
{
}

//-----------------------------------------------------------------------------

void LockDumpSrv::info( std::ostream& _sstream )
{
}

//-----------------------------------------------------------------------------

} // ctrlMemory
