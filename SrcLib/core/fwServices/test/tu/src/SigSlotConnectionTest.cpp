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

#include "SigSlotConnectionTest.hpp"

#include "SlotsSignalsStuff.hpp"

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <fwCom/helper/SigSlotConnection.hpp>

#include <fwTest/Exception.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::SigSlotConnectionTest );

namespace fwServices
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
    SLM_TRACE_FUNC();
    registry::ActiveWorkers::sptr activeWorkers = registry::ActiveWorkers::getDefault();
    activeWorkers->initRegistry();

    Buffer::sptr buffer = Buffer::New();

    SShowTest::sptr showTestSrv = ::fwServices::factory::New<SShowTest>();
    ::fwServices::OSR::registerService(showTestSrv);
    showTestSrv->registerInOut(buffer, "buffer", true);
    showTestSrv->setWorker(activeWorkers->getWorker(registry::ActiveWorkers::s_DEFAULT_WORKER));

    ::fwData::Object::ModifiedSignalType::sptr sig =
        buffer->signal< ::fwData::Object::ModifiedSignalType >( ::fwData::Object::s_MODIFIED_SIG );
    sig->asyncEmit();
    CPPUNIT_ASSERT_EQUAL(0, showTestSrv->m_receiveCount);

    showTestSrv->start().wait();
    sig->asyncEmit();
    showTestSrv->stop().wait();
    CPPUNIT_ASSERT_EQUAL(1, showTestSrv->m_receiveCount);

    sig->asyncEmit();
    CPPUNIT_ASSERT_EQUAL(1, showTestSrv->m_receiveCount);

    ::fwServices::OSR::unregisterService(showTestSrv);
    activeWorkers->clearRegistry();
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices
