/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/registry/ActiveWorkers.hpp>

#include "ActiveWorkersTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::ActiveWorkersTest );

namespace fwServices
{
namespace ut
{

//------------------------------------------------------------------------------

void ActiveWorkersTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ActiveWorkersTest::tearDown()
{
    // Clean up after the test run.
}

//------------------------------------------------------------------------------

void ActiveWorkersTest::basicTest()
{
    using namespace ::fwServices::registry;

    ActiveWorkers::sptr activeWorkers = ActiveWorkers::getDefault();
    CPPUNIT_ASSERT( ! activeWorkers->getWorker( ActiveWorkers::s_DEFAULT_WORKER ) );

    activeWorkers->initRegistry();
    CPPUNIT_ASSERT( activeWorkers->getWorker( ActiveWorkers::s_DEFAULT_WORKER ) );

    CPPUNIT_ASSERT( ! activeWorkers->getWorker( "Wrong_id" ) );

    ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();
    activeWorkers->addWorker("another_worker",worker);
    CPPUNIT_ASSERT( activeWorkers->getWorker( "another_worker" ) );

    activeWorkers->clearRegistry();
    CPPUNIT_ASSERT( ! activeWorkers->getWorker( "another_worker" ) );
    CPPUNIT_ASSERT( ! activeWorkers->getWorker( ActiveWorkers::s_DEFAULT_WORKER ) );
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices
