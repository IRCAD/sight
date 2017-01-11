/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/spyLog.hpp>

#include <fwThread/Pool.hpp>
#include <fwThread/Worker.hpp>

#include "PoolTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwThread::ut::PoolTest );

namespace fwThread
{
namespace ut
{

void PoolTest::setUp()
{
    // Set up context before running a test.
}
void PoolTest::tearDown()
{
    // Clean up after the test run.
}


//-----------------------------------------------------------------------------

struct PoolTestHandler
{
    PoolTestHandler() : m_step(0), m_threadCheckOk(true)
    {
        m_constructorThreadId = ::fwThread::getCurrentThreadId();
    }

    void nextStep()
    {
        ::std::this_thread::sleep_for( ::std::chrono::milliseconds(10));
        this->nextStepNoSleep();
    }

    void nextStepNoSleep()
    {
        m_threadCheckOk &= (m_constructorThreadId != ::fwThread::getCurrentThreadId());

        std::unique_lock<std::mutex> lock(m_mutex);
        ++m_step;
    }

    int m_step;
    bool m_threadCheckOk;
    std::mutex m_mutex;
    ::fwThread::ThreadIdType m_constructorThreadId;
};

//-----------------------------------------------------------------------------

void PoolTest::basicTest()
{
    {
        PoolTestHandler handler;
        {
            ::fwThread::Pool pool(1);

            pool.post( ::std::bind( &PoolTestHandler::nextStep, &handler) );
            pool.post( ::std::bind( &PoolTestHandler::nextStep, &handler) );
            pool.post( ::std::bind( &PoolTestHandler::nextStep, &handler) );
        }
        CPPUNIT_ASSERT_EQUAL(3, handler.m_step);
        CPPUNIT_ASSERT_EQUAL(true, handler.m_threadCheckOk);
    }

    {
        PoolTestHandler handler;
        {
            ::fwThread::Pool pool(10);

            for(int i = 0; i < 50; ++i)
            {
                pool.post( ::std::bind( &PoolTestHandler::nextStep, &handler) );
            }
        }
        CPPUNIT_ASSERT_EQUAL(50, handler.m_step);
        CPPUNIT_ASSERT_EQUAL(true, handler.m_threadCheckOk);
    }

}

//-----------------------------------------------------------------------------

void PoolTest::defaultPoolTest()
{
    // Create the pool
    ::fwThread::createDefaultPool();

    // Use the pool
    PoolTestHandler handler;
    ::fwThread::Pool& pool = ::fwThread::getDefaultPool();
    pool.post( ::std::bind( &PoolTestHandler::nextStep, &handler) );
    pool.post( ::std::bind( &PoolTestHandler::nextStep, &handler) );
    pool.post( ::std::bind( &PoolTestHandler::nextStep, &handler) );

    // Delete the default ppol
    ::fwThread::deleteDefaultPool();

    CPPUNIT_ASSERT_EQUAL(3, handler.m_step);
    CPPUNIT_ASSERT_EQUAL(true, handler.m_threadCheckOk);

}
//-----------------------------------------------------------------------------

} //namespace ut
} //namespace fwThread
