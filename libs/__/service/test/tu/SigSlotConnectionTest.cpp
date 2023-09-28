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

#include "SigSlotConnectionTest.hpp"

#include "SlotsSignalsStuff.hpp"

#include <core/com/helper/sig_slot_connection.hpp>

#include <service/macros.hpp>
#include <service/registry.hpp>

#include <utest/exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::ut::SigSlotConnectionTest);

namespace sight::service::ut
{

//------------------------------------------------------------------------------

void SigSlotConnectionTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void SigSlotConnectionTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void SigSlotConnectionTest::basicTest()
{
    Buffer::sptr buffer = std::make_shared<Buffer>();

    SShowTest::sptr showTestSrv = service::factory::make<SShowTest>();
    service::registerService(showTestSrv);
    showTestSrv->setInOut(buffer, basic_srv::s_BUFFER_INOUT, true);
    showTestSrv->set_worker(core::thread::get_default_worker());

    data::Object::ModifiedSignalType::sptr sig =
        buffer->signal<data::Object::ModifiedSignalType>(data::Object::MODIFIED_SIG);
    sig->async_emit();
    CPPUNIT_ASSERT_EQUAL(0, showTestSrv->m_receiveCount);

    showTestSrv->start().wait();
    sig->async_emit();
    showTestSrv->stop().wait();
    CPPUNIT_ASSERT_EQUAL(1, showTestSrv->m_receiveCount);

    sig->async_emit();
    CPPUNIT_ASSERT_EQUAL(1, showTestSrv->m_receiveCount);

    service::unregisterService(showTestSrv);
}

//------------------------------------------------------------------------------

} // namespace sight::service::ut
