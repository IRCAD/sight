/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/helper/SigSlotConnection.hpp>

#include <fwTest/Exception.hpp>

#include "SigSlotConnectionTest.hpp"
#include "SlotsSignalsStuff.hpp"

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

    Buffer::NewSptr buffer;

    SShowTest::sptr showTestSrv = ::fwServices::factory::New<SShowTest>();
    ::fwServices::OSR::registerService(buffer, showTestSrv);

    ::fwData::Object::ObjectModifiedSignalType::sptr sig =
            buffer->signal< ::fwData::Object::ObjectModifiedSignalType >( ::fwData::Object::s_OBJECT_MODIFIED_SIG );

    ::fwServices::ObjectMsg::NewSptr msg;

    ::fwServices::helper::SigSlotConnection::NewSptr helper;

    helper->connect( buffer, showTestSrv, showTestSrv->getObjSrvConnections() );
    showTestSrv->start().wait();
    sig->asyncEmit( msg);
    showTestSrv->stop().wait();
    CPPUNIT_ASSERT_EQUAL(1, showTestSrv->m_receiveCount);

    helper->disconnect();
    showTestSrv->start().wait();
    sig->asyncEmit( msg);
    showTestSrv->stop().wait();
    CPPUNIT_ASSERT_EQUAL(1, showTestSrv->m_receiveCount);

    ::fwServices::OSR::unregisterService(showTestSrv);
    activeWorkers->clearRegistry();
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices
