/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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
    CPPUNIT_ASSERT( !activeWorkers->getWorker( ActiveWorkers::s_DEFAULT_WORKER ) );

    activeWorkers->initRegistry();
    CPPUNIT_ASSERT( activeWorkers->getWorker( ActiveWorkers::s_DEFAULT_WORKER ) );

    CPPUNIT_ASSERT( !activeWorkers->getWorker( "Wrong_id" ) );

    ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();
    activeWorkers->addWorker("another_worker",worker);
    CPPUNIT_ASSERT( activeWorkers->getWorker( "another_worker" ) );

    activeWorkers->clearRegistry();
    CPPUNIT_ASSERT( !activeWorkers->getWorker( "another_worker" ) );
    CPPUNIT_ASSERT( !activeWorkers->getWorker( ActiveWorkers::s_DEFAULT_WORKER ) );
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices
