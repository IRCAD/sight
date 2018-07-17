/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
