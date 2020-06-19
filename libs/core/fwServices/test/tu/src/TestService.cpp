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

#include "TestService.hpp"

#include <fwServices/macros.hpp>

#include <fwCom/Slots.hxx>

#include <fwData/Object.hpp>

fwServicesRegisterMacro( ::fwServices::ut::TestService, ::fwServices::ut::TestServiceImplementation);
fwServicesRegisterObjectMacro(::fwServices::ut::TestServiceImplementation, ::fwData::Object);
fwServicesRegisterMacro( ::fwServices::ut::TestService, ::fwServices::ut::TestServiceImplementation2 );
fwServicesRegisterMacro( ::fwServices::ut::TestService, ::fwServices::ut::TestSrvAutoconnect );
fwServicesRegisterMacro( ::fwServices::IService, ::fwServices::ut::TestServiceWithData );

namespace fwServices
{
namespace ut
{

const ::fwCom::Signals::SignalKeyType TestServiceImplementation::s_MSG_SENT_SIG = "msgSent";
const ::fwCom::Slots::SlotKeyType TestServiceImplementation::s_UPDATE2_SLOT     = "update2";

const ::fwCom::Signals::SignalKeyType TestSrvAutoconnect::s_SIG_1 = "signal1";
const ::fwCom::Slots::SlotKeyType TestSrvAutoconnect::s_SLOT_1    = "slot1";

unsigned int TestService::s_START_COUNTER  = 0;
unsigned int TestService::s_UPDATE_COUNTER = 0;

const std::string TestService::s_OPTION_KEY   = "option";
const std::string TestService::s_UNCONFIGURED = "UNCONFIGURED";
const std::string TestService::s_NOT_DEFINED  = "NOT_DEFINED";

const ::fwServices::IService::KeyType TestServiceWithData::s_INPUT       = "input";
const ::fwServices::IService::KeyType TestServiceWithData::s_INOUT_GROUP = "inoutGroup";
const ::fwServices::IService::KeyType TestServiceWithData::s_OUTPUT      = "output";

//------------------------------------------------------------------------------

void TestService::starting()
{
    if(m_raiseException)
    {
        throw ::fwCore::Exception("start error");
    }
    m_startOrder = s_START_COUNTER++;
}

//------------------------------------------------------------------------------

void TestService::stopping()
{
    if(m_raiseException)
    {
        throw ::fwCore::Exception("stop error");
    }
}

//------------------------------------------------------------------------------

void TestService::updating()
{
    if(m_raiseException)
    {
        throw ::fwCore::Exception("update error");
    }
    m_updateOrder = s_UPDATE_COUNTER++;
}

//------------------------------------------------------------------------------

TestServiceImplementation::TestServiceImplementation() noexcept
{
    newSignal<MsgSentSignalType>(s_MSG_SENT_SIG);
    newSlot(s_UPDATE2_SLOT, &TestServiceImplementation::update2, this);
}

//------------------------------------------------------------------------------

TestServiceImplementation::~TestServiceImplementation() noexcept
{
}

//------------------------------------------------------------------------------

void TestServiceWithData::updating()
{
    ::fwData::Object::csptr input = this->getInput< ::fwData::Object >(s_INPUT);

    ::fwData::Object::sptr output = ::fwData::Object::copy(input);

    this->setOutput(s_OUTPUT, output);
}

//------------------------------------------------------------------------------

void TestServiceWithData::stopping()
{
    this->setOutput(s_OUTPUT, nullptr);
}

//------------------------------------------------------------------------------

} // namespace ut
} // namespace fwServices
