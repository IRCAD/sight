/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "MTLockTest.hpp"

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/mt/ObjectReadToWriteLock.hpp>

#include <fwTest/helper/Thread.hpp>

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <functional>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::MTLockTest );

namespace fwData
{
namespace ut
{

//-----------------------------------------------------------------------------

void MTLockTest::setUp()
{
    // Set up context before running a test.
    m_string = ::fwData::String::New();
}

//-----------------------------------------------------------------------------

void MTLockTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void MTLockTest::lockTest()
{

    ::fwTest::helper::Thread thread(std::bind(&MTLockTest::runLock, this));

    CPPUNIT_ASSERT(thread.timedJoin(2000));


}

//-----------------------------------------------------------------------------

void MTLockTest::runLock()
{
    {
        ::fwData::mt::ObjectReadLock readLock(m_string);
    }

    {
        ::fwData::mt::ObjectWriteLock writeLock(m_string);

        writeLock.unlock();

        ::fwData::mt::ObjectReadToWriteLock updrageLock(m_string);

        updrageLock.unlock();

        writeLock.lock();

        writeLock.unlock();

        updrageLock.lock();

        updrageLock.upgrade();

        updrageLock.downgrade();

        ::fwData::mt::ObjectReadLock readLock(m_string);

        readLock.unlock();

        updrageLock.unlock();

        writeLock.lock();
    }
}

//-----------------------------------------------------------------------------

void MTLockTest::multipleLockTest()
{
    ::fwTest::helper::Thread thread(std::bind(&MTLockTest::runMultipleLock1, this));
    ::fwTest::helper::Thread thread2(std::bind(&MTLockTest::runMultipleLock2, this));

    CPPUNIT_ASSERT(thread.timedJoin(2500));
    CPPUNIT_ASSERT(thread2.timedJoin(2500));

    CPPUNIT_ASSERT_MESSAGE(m_string->value(), m_string->value()=="lili" ||  m_string->value()=="toto");
}

//-----------------------------------------------------------------------------

void MTLockTest::runMultipleLock1()
{
    {
        ::fwData::mt::ObjectWriteLock writeLock(m_string);
        ::boost::this_thread::sleep_for( ::boost::chrono::milliseconds(100));
        m_string->value() += "t";
        ::boost::this_thread::sleep_for( ::boost::chrono::milliseconds(100));
        m_string->value() += "o";
        ::boost::this_thread::sleep_for( ::boost::chrono::milliseconds(100));
        m_string->value() += "t";
        ::boost::this_thread::sleep_for( ::boost::chrono::milliseconds(100));
        m_string->value() += "o";
    }
    ::boost::this_thread::sleep_for( ::boost::chrono::milliseconds(100));

    {
        ::fwData::mt::ObjectReadLock readLock(m_string);
        CPPUNIT_ASSERT(m_string->value().find("toto") != std::string::npos);
    }
    ::boost::this_thread::sleep_for( ::boost::chrono::milliseconds(100));

    {
        ::fwData::mt::ObjectReadToWriteLock lock(m_string);
        if (m_string->value() == "totolili")
        {
            ::boost::this_thread::sleep_for( ::boost::chrono::milliseconds(100));
            lock.upgrade();
            m_string->value() = "toto";
        }
    }
}

//-----------------------------------------------------------------------------

void MTLockTest::runMultipleLock2()
{
    {
        ::fwData::mt::ObjectWriteLock writeLock(m_string);
        ::boost::this_thread::sleep_for( ::boost::chrono::milliseconds(100));
        m_string->value() += "l";
        ::boost::this_thread::sleep_for( ::boost::chrono::milliseconds(100));
        m_string->value() += "i";
        ::boost::this_thread::sleep_for( ::boost::chrono::milliseconds(100));
        m_string->value() += "l";
        ::boost::this_thread::sleep_for( ::boost::chrono::milliseconds(100));
        m_string->value() += "i";
    }
    ::boost::this_thread::sleep_for( ::boost::chrono::milliseconds(100));

    {
        ::fwData::mt::ObjectReadLock readLock(m_string);
        CPPUNIT_ASSERT(m_string->value().find("lili") != std::string::npos);
    }
    ::boost::this_thread::sleep_for( ::boost::chrono::milliseconds(100));

    {
        ::fwData::mt::ObjectReadToWriteLock lock(m_string);
        if (m_string->value() == "lilitoto")
        {
            ::boost::this_thread::sleep_for( ::boost::chrono::milliseconds(100));
            lock.upgrade();
            m_string->value() = "lili";
        }
    }
}

//-----------------------------------------------------------------------------


} //namespace ut
} //namespace fwData
