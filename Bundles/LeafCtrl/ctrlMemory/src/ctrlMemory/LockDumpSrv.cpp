/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/macros.hpp>

#include "ctrlMemory/LockDumpSrv.hpp"

namespace ctrlMemory
{

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
    m_objLock = ::fwData::ObjectLock( this->getObject() );
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

void LockDumpSrv::info( std::ostream &_sstream )
{
}

//-----------------------------------------------------------------------------

} // ctrlMemory
