/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
    SLM_ASSERT("The inout key '" + s_TARGET_INOUT + "' is not correctly set.", object);

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
