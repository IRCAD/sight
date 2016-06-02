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

LockDumpSrv::LockDumpSrv() throw()
{
}

//-----------------------------------------------------------------------------

LockDumpSrv::~LockDumpSrv() throw()
{
}

//-----------------------------------------------------------------------------

void LockDumpSrv::starting()  throw ( ::fwTools::Failed )
{
    m_objLock = ::fwData::ObjectLock( this->getObject() );
}

//-----------------------------------------------------------------------------

void LockDumpSrv::stopping()  throw ( ::fwTools::Failed )
{
    m_objLock = ::fwData::ObjectLock();
}

//-----------------------------------------------------------------------------

void LockDumpSrv::configuring()  throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void LockDumpSrv::reconfiguring()  throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void LockDumpSrv::updating() throw ( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void LockDumpSrv::info( std::ostream &_sstream )
{
}

//-----------------------------------------------------------------------------

} // ctrlMemory
