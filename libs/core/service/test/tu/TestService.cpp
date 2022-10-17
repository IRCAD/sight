/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <core/com/Slots.hxx>

#include <data/Object.hpp>

#include <service/macros.hpp>

SIGHT_REGISTER_SERVICE(sight::service::ut::ISTest, sight::service::ut::STestNoData);
SIGHT_REGISTER_SERVICE(sight::service::ut::ISTest, sight::service::ut::STestNoData2);
SIGHT_REGISTER_SERVICE(sight::service::ut::ISTest, sight::service::ut::STest1Input);
SIGHT_REGISTER_SERVICE(sight::service::ut::ISTest, sight::service::ut::STest1OptInput);
SIGHT_REGISTER_SERVICE(sight::service::ut::ISTest, sight::service::ut::STest2Inputs);
SIGHT_REGISTER_SERVICE(sight::service::ut::ISTest, sight::service::ut::STest2InputsV2);
SIGHT_REGISTER_SERVICE(sight::service::ut::ISTest, sight::service::ut::STest4Inputs);
SIGHT_REGISTER_SERVICE(sight::service::ut::ISTest, sight::service::ut::STest1Inout);
SIGHT_REGISTER_SERVICE(sight::service::ut::ISTest, sight::service::ut::STest3Inouts);
SIGHT_REGISTER_SERVICE(sight::service::ut::ISTest, sight::service::ut::STest3InoutsV2);
SIGHT_REGISTER_SERVICE(sight::service::ut::ISTest, sight::service::ut::STest2Inouts1Input);
SIGHT_REGISTER_SERVICE(sight::service::ut::ISTest, sight::service::ut::STest2InputGroups);
SIGHT_REGISTER_SERVICE(sight::service::ut::ISTest, sight::service::ut::STest1Input1Output2Inouts);
SIGHT_REGISTER_SERVICE(sight::service::ut::ISTest, sight::service::ut::STest1Input1InputGroup);
SIGHT_REGISTER_SERVICE(sight::service::ut::ISTest, sight::service::ut::STest1Input1OptInput1OptInOut);
SIGHT_REGISTER_SERVICE(sight::service::ut::ISTest, sight::service::ut::STestOut);
SIGHT_REGISTER_SERVICE(sight::service::IService, sight::service::ut::TestServiceWithData);

namespace sight::service::ut
{

const core::com::Signals::SignalKeyType ISTest::s_MSG_SENT_SIG = "msgSent";
const core::com::Slots::SlotKeyType ISTest::s_UPDATE2_SLOT     = "update2";

const core::com::Signals::SignalKeyType ISTest::s_SIG_1 = "signal1";
const core::com::Slots::SlotKeyType ISTest::s_SLOT_1    = "slot1";

unsigned int TestService::s_START_COUNTER  = 0;
unsigned int TestService::s_UPDATE_COUNTER = 0;

const std::string TestService::s_OPTION_KEY   = "option";
const std::string TestService::s_UNCONFIGURED = "UNCONFIGURED";
const std::string TestService::s_NOT_DEFINED  = "NOT_DEFINED";

const service::IService::KeyType TestServiceWithData::s_INPUT       = "input";
const service::IService::KeyType TestServiceWithData::s_INOUT_GROUP = "inoutGroup";
const service::IService::KeyType TestServiceWithData::s_OUTPUT      = "output";

//------------------------------------------------------------------------------

void TestService::starting()
{
    if(m_raiseException)
    {
        throw core::Exception("start error");
    }

    m_startOrder = s_START_COUNTER++;
}

//------------------------------------------------------------------------------

void TestService::stopping()
{
    if(m_raiseException)
    {
        throw core::Exception("stop error");
    }
}

//------------------------------------------------------------------------------

void TestService::updating()
{
    if(m_raiseException)
    {
        throw core::Exception("update error");
    }

    m_updateOrder = s_UPDATE_COUNTER++;
}

//------------------------------------------------------------------------------

void TestServiceWithData::updating()
{
    auto lock                 = m_input.lock();
    data::Object::csptr input = lock.get_shared();

    data::Object::sptr output = data::Object::copy(input);
    m_output = output;
}

//------------------------------------------------------------------------------

void TestServiceWithData::stopping()
{
    m_output = nullptr;
}

//------------------------------------------------------------------------------

} // namespace sight::service::ut
