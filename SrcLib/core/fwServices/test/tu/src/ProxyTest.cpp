/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ProxyTest.hpp"

#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/Proxy.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwServices::ut::ProxyTest );

namespace fwServices
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
        OSLM_INFO("SUM " << a << " + " << b);
        ++m_methodSum;
        return a+b;
    }

    //------------------------------------------------------------------------------

    int square (int a)
    {
        OSLM_INFO("SQUARE " << a );
        ++m_methodSquare;
        return a*a;
    }

    //------------------------------------------------------------------------------

    void doNothing()
    {
        OSLM_INFO("DO NOTHING");
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

    ::fwServices::registry::Proxy::sptr proxy = ::fwServices::registry::Proxy::getDefault();

    ::fwCom::Signal< void(int, int) >::sptr sig        = ::fwCom::Signal< void(int, int) >::New();
    ::fwCom::Signal< void(int, int, char) >::sptr sig2 = ::fwCom::Signal< void(int, int, char) >::New();

    ProxyTestClass testObject;
    ::fwCom::Slot< int (int, int) >::sptr slot = ::fwCom::newSlot( &ProxyTestClass::sum, &testObject );
    ::fwCom::Slot< int (int) >::sptr slot2     = ::fwCom::newSlot( &ProxyTestClass::square, &testObject );
    ::fwCom::Slot< void() >::sptr slot3        = ::fwCom::newSlot( &ProxyTestClass::doNothing, &testObject );
    ::fwThread::Worker::sptr worker            = ::fwThread::Worker::New();
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
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace fwServices
