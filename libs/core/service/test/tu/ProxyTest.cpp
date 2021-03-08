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

#include "ProxyTest.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/thread/ActiveWorkers.hpp>

#include <service/registry/Proxy.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::service::ut::ProxyTest );

namespace sight::service
{
namespace ut
{

//------------------------------------------------------------------------------

void ProxyTest::setUp()
{
    // Set up context before running a test.
}

//------------------------------------------------------------------------------

void ProxyTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

struct ProxyTestClass
{
    ProxyTestClass() :
        m_methodSum(0),
        m_methodSquare(0),
        m_methodDoNothing(0)
    {
    }

    //------------------------------------------------------------------------------

    int sum(int a, int b)
    {
        SIGHT_INFO("SUM " << a << " + " << b);
        ++m_methodSum;
        return a+b;
    }

    //------------------------------------------------------------------------------

    int square (int a)
    {
        SIGHT_INFO("SQUARE " << a );
        ++m_methodSquare;
        return a*a;
    }

    //------------------------------------------------------------------------------

    void doNothing()
    {
        SIGHT_INFO("DO NOTHING");
        ++m_methodDoNothing;
    }

    int m_methodSum;
    int m_methodSquare;
    int m_methodDoNothing;
};

//------------------------------------------------------------------------------

void ProxyTest::basicTest()
{
    const std::string CHANNEL = "channel";

    service::registry::Proxy::sptr proxy = service::registry::Proxy::getDefault();

    core::com::Signal< void(int, int) >::sptr sig        = core::com::Signal< void(int, int) >::New();
    core::com::Signal< void(int, int, char) >::sptr sig2 = core::com::Signal< void(int, int, char) >::New();

    ProxyTestClass testObject;
    core::com::Slot< int (int, int) >::sptr slot = core::com::newSlot( &ProxyTestClass::sum, &testObject );
    core::com::Slot< int (int) >::sptr slot2     = core::com::newSlot( &ProxyTestClass::square, &testObject );
    core::com::Slot< void() >::sptr slot3        = core::com::newSlot( &ProxyTestClass::doNothing, &testObject );
    core::thread::Worker::sptr worker            = core::thread::Worker::New();
    slot->setWorker(worker);
    slot2->setWorker(worker);
    slot3->setWorker(worker);

    proxy->connect(CHANNEL, sig);
    proxy->connect(CHANNEL, sig2);
    proxy->connect(CHANNEL, slot);
    proxy->connect(CHANNEL, slot2);
    proxy->connect(CHANNEL, slot3);

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), sig->getNumberOfConnections());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), sig2->getNumberOfConnections());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), slot->getNumberOfConnections());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), slot2->getNumberOfConnections());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), slot3->getNumberOfConnections());
    sig->asyncEmit(3, 5);

    std::this_thread::sleep_for( std::chrono::seconds(1));

    CPPUNIT_ASSERT_EQUAL(1, testObject.m_methodSum);
    CPPUNIT_ASSERT_EQUAL(1, testObject.m_methodSquare);
    CPPUNIT_ASSERT_EQUAL(1, testObject.m_methodDoNothing);

    sig2->asyncEmit(8, 2, 'x');

    std::this_thread::sleep_for( std::chrono::seconds(1));

    CPPUNIT_ASSERT_EQUAL(2, testObject.m_methodSum);
    CPPUNIT_ASSERT_EQUAL(2, testObject.m_methodSquare);
    CPPUNIT_ASSERT_EQUAL(2, testObject.m_methodDoNothing);

    proxy->disconnect(CHANNEL, sig);
    proxy->disconnect(CHANNEL, sig2);
    proxy->disconnect(CHANNEL, slot);
    proxy->disconnect(CHANNEL, slot2);
    proxy->disconnect(CHANNEL, slot3);

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), sig->getNumberOfConnections());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), sig2->getNumberOfConnections());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), slot->getNumberOfConnections());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), slot2->getNumberOfConnections());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), slot3->getNumberOfConnections());
    worker->stop();
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace sight::service
