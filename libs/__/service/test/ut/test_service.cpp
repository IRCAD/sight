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

SIGHT_REGISTER_SERVICE(sight::service::ut::test_srv, sight::service::ut::test_no_data);
SIGHT_REGISTER_SERVICE(sight::service::ut::test_srv, sight::service::ut::test_no_data2);
SIGHT_REGISTER_SERVICE(sight::service::ut::test_srv, sight::service::ut::test1_input);
SIGHT_REGISTER_SERVICE(sight::service::ut::test_srv, sight::service::ut::test1_inout);
SIGHT_REGISTER_SERVICE(sight::service::ut::test_srv, sight::service::ut::test2_inouts1_input);
SIGHT_REGISTER_SERVICE(sight::service::ut::test_srv, sight::service::ut::test1_input1_opt_input1_opt_in_out);
SIGHT_REGISTER_SERVICE(sight::service::ut::test_srv, sight::service::ut::test3_inouts_v2);
SIGHT_REGISTER_SERVICE(sight::service::base, sight::service::ut::test_service_with_data);

namespace sight::service::ut
{

unsigned int test_service::s_start_counter  = 0;
unsigned int test_service::s_update_counter = 0;

const std::string test_service::OPTION_KEY   = "option";
const std::string test_service::UNCONFIGURED = "UNCONFIGURED";
const std::string test_service::NOT_DEFINED  = "NOT_DEFINED";

const service::base::key_t test_service_with_data::INPUT       = "input";
const service::base::key_t test_service_with_data::INOUT_GROUP = "inoutGroup";
const service::base::key_t test_service_with_data::OUTPUT      = "output";

//------------------------------------------------------------------------------

void test_service::starting()
{
    if(m_raise_exception)
    {
        throw core::exception("start error");
    }

    m_start_order = s_start_counter++;
}

//------------------------------------------------------------------------------

void test_service::stopping()
{
    if(m_raise_exception)
    {
        throw core::exception("stop error");
    }
}

//------------------------------------------------------------------------------

void test_service::updating()
{
    if(m_raise_exception)
    {
        throw core::exception("update error");
    }

    m_update_order = s_update_counter++;
}

//------------------------------------------------------------------------------

void test_service_with_data::updating()
{
    auto lock                 = m_input.lock();
    data::object::csptr input = lock.get_shared();

    auto output = data::object::copy(input);
    m_output = output;
}

//------------------------------------------------------------------------------

void test_service_with_data::stopping()
{
    m_output = nullptr;
}

//------------------------------------------------------------------------------

} // namespace sight::service::ut
