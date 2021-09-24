/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <core/com/helper/SigSlotConnection.hpp>
#include <core/thread/ActiveWorkers.hpp>

#include <service/macros.hpp>
#include <service/registry/ObjectService.hpp>

#include <utest/Exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::service::ut::SigSlotConnectionTest);

namespace sight::service
{

namespace ut
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
    auto activeWorkers = core::thread::ActiveWorkers::getDefault();
    activeWorkers->initRegistry();

    Buffer::sptr buffer = Buffer::New();

    SShowTest::sptr showTestSrv = service::factory::New<SShowTest>();
    service::OSR::registerService(showTestSrv);
    showTestSrv->setInOut(buffer, IBasicTest::s_BUFFER_INOUT, true);
    showTestSrv->setWorker(activeWorkers->getWorker(core::thread::ActiveWorkers::s_DEFAULT_WORKER));

    data::Object::ModifiedSignalType::sptr sig =
        buffer->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
    CPPUNIT_ASSERT_EQUAL(0, showTestSrv->m_receiveCount);

    showTestSrv->start().wait();
    sig->asyncEmit();
    showTestSrv->stop().wait();
    CPPUNIT_ASSERT_EQUAL(1, showTestSrv->m_receiveCount);

    sig->asyncEmit();
    CPPUNIT_ASSERT_EQUAL(1, showTestSrv->m_receiveCount);

    service::OSR::unregisterService(showTestSrv);
    activeWorkers->clearRegistry();
}

//------------------------------------------------------------------------------

} //namespace ut

} //namespace sight::service
