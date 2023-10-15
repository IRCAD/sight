/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "test_service.hpp"

#include <core/com/slots.hxx>

#include <data/object.hpp>

#include <service/macros.hpp>

SIGHT_REGISTER_SERVICE(sight::app::ut::test_srv, sight::app::ut::STestNoData);
SIGHT_REGISTER_SERVICE(sight::app::ut::test_srv, sight::app::ut::STestNoData2);
SIGHT_REGISTER_SERVICE(sight::app::ut::test_srv, sight::app::ut::STest1Input);
SIGHT_REGISTER_SERVICE(sight::app::ut::test_srv, sight::app::ut::STest1OptInput);
SIGHT_REGISTER_SERVICE(sight::app::ut::test_srv, sight::app::ut::STest2Inputs);
SIGHT_REGISTER_SERVICE(sight::app::ut::test_srv, sight::app::ut::STest2InputsV2);
SIGHT_REGISTER_SERVICE(sight::app::ut::test_srv, sight::app::ut::STest4Inputs);
SIGHT_REGISTER_SERVICE(sight::app::ut::test_srv, sight::app::ut::STest1Inout);
SIGHT_REGISTER_SERVICE(sight::app::ut::test_srv, sight::app::ut::STest3Inouts);
SIGHT_REGISTER_SERVICE(sight::app::ut::test_srv, sight::app::ut::STest3InoutsV2);
SIGHT_REGISTER_SERVICE(sight::app::ut::test_srv, sight::app::ut::STest2Inouts1Input);
SIGHT_REGISTER_SERVICE(sight::app::ut::test_srv, sight::app::ut::STest2InputGroups);
SIGHT_REGISTER_SERVICE(sight::app::ut::test_srv, sight::app::ut::STest1Input1Output2Inouts);
SIGHT_REGISTER_SERVICE(sight::app::ut::test_srv, sight::app::ut::STest1Input1InputGroup);
SIGHT_REGISTER_SERVICE(sight::app::ut::test_srv, sight::app::ut::STest1Input1OptInput1OptInOut);
SIGHT_REGISTER_SERVICE(sight::app::ut::test_srv, sight::app::ut::STestOut);
SIGHT_REGISTER_SERVICE(sight::service::base, sight::app::ut::test_service_with_data);

namespace sight::app::ut
{

unsigned int test_service::s_START_COUNTER  = 0;
unsigned int test_service::s_UPDATE_COUNTER = 0;

const std::string test_service::s_OPTION_KEY   = "option";
const std::string test_service::s_UNCONFIGURED = "UNCONFIGURED";
const std::string test_service::s_NOT_DEFINED  = "NOT_DEFINED";

const service::base::key_t test_service_with_data::s_INPUT       = "input";
const service::base::key_t test_service_with_data::s_INOUT_GROUP = "inoutGroup";
const service::base::key_t test_service_with_data::s_OUTPUT      = "output";

//------------------------------------------------------------------------------

void test_service::starting()
{
    if(m_raiseException)
    {
        throw core::exception("start error");
    }

    m_startOrder = s_START_COUNTER++;
}

//------------------------------------------------------------------------------

void test_service::stopping()
{
    if(m_raiseException)
    {
        throw core::exception("stop error");
    }
}

//------------------------------------------------------------------------------

void test_service::updating()
{
    if(m_raiseException)
    {
        throw core::exception("update error");
    }

    m_updateOrder = s_UPDATE_COUNTER++;
}

//------------------------------------------------------------------------------

void test_service_with_data::updating()
{
    auto lock                 = m_input.lock();
    data::object::csptr input = lock.get_shared();

    data::object::sptr output = data::object::copy(input);
    m_output = output;
}

//------------------------------------------------------------------------------

void test_service_with_data::stopping()
{
    m_output = nullptr;
}

//------------------------------------------------------------------------------

} // namespace sight::app::ut
