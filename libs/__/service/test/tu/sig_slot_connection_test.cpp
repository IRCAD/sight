/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "sig_slot_connection_test.hpp"

#include "slots_signals_stuff.hpp"

#include <core/com/helper/sig_slot_connection.hpp>

#include <service/macros.hpp>
#include <service/registry.hpp>

#include <utest/exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::ut::sig_slot_connection_test);

namespace sight::service::ut
{

//------------------------------------------------------------------------------

void sig_slot_connection_test::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void sig_slot_connection_test::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void sig_slot_connection_test::basicTest()
{
    auto buffer_data = std::make_shared<buffer>();

    SShowTest::sptr show_test_srv = service::factory::make<SShowTest>();
    service::register_service(show_test_srv);
    show_test_srv->set_inout(buffer_data, basic_srv::s_BUFFER_INOUT, true);
    show_test_srv->set_worker(core::thread::get_default_worker());

    data::object::modified_signal_t::sptr sig =
        buffer_data->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
    sig->async_emit();
    CPPUNIT_ASSERT_EQUAL(0, show_test_srv->m_receiveCount);

    show_test_srv->start().wait();
    sig->async_emit();
    show_test_srv->stop().wait();
    CPPUNIT_ASSERT_EQUAL(1, show_test_srv->m_receiveCount);

    sig->async_emit();
    CPPUNIT_ASSERT_EQUAL(1, show_test_srv->m_receiveCount);

    service::unregister_service(show_test_srv);
}

//------------------------------------------------------------------------------

} // namespace sight::service::ut
