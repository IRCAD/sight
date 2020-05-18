/************************************************************************
 *
 * Copyright (C) 2015-2018 IRCAD France
 * Copyright (C) 2015-2018 IHU Strasbourg
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

#include "PoolTest.hpp"

#include <fwThread/Pool.hpp>
#include <fwThread/Worker.hpp>

#include <fwCore/spyLog.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwThread::ut::PoolTest );

namespace fwThread
{
namespace ut
{

//------------------------------------------------------------------------------

void PoolTest::setUp()
{
    // Set up context before running a test.
}
//------------------------------------------------------------------------------

void PoolTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

struct PoolTestHandler
{
    PoolTestHandler() :
        m_step(0),
        m_threadCheckOk(true)
    {
        m_constructorThreadId = ::fwThread::getCurrentThreadId();
    }

    //------------------------------------------------------------------------------

    void nextStep()
    {
        ::std::this_thread::sleep_for( ::std::chrono::milliseconds(10));
        this->nextStepNoSleep();
    }

    //------------------------------------------------------------------------------

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
        // Single thread test
        PoolTestHandler handler;

        ::fwThread::Pool pool(1);

        std::vector< ::std::shared_future<void> > futures;
        futures.push_back( pool.post( ::std::bind( &PoolTestHandler::nextStep, &handler) ));
        futures.push_back( pool.post( ::std::bind( &PoolTestHandler::nextStep, &handler) ));
        futures.push_back( pool.post( ::std::bind( &PoolTestHandler::nextStep, &handler) ));

        std::for_each(futures.begin(), futures.end(), std::mem_fn(&::std::shared_future<void>::wait));

        CPPUNIT_ASSERT_EQUAL(3, handler.m_step);
        CPPUNIT_ASSERT_EQUAL(true, handler.m_threadCheckOk);
    }

    {
        // Multiple threads test
        PoolTestHandler handler;

        ::fwThread::Pool pool(10);

        std::vector< ::std::shared_future<void> > futures;
        for(int i = 0; i < 50; ++i)
        {
            futures.push_back( pool.post( ::std::bind( &PoolTestHandler::nextStep, &handler) ));
        }
        std::for_each(futures.begin(), futures.end(), std::mem_fn(&::std::shared_future<void>::wait));

        CPPUNIT_ASSERT_EQUAL(50, handler.m_step);
        CPPUNIT_ASSERT_EQUAL(true, handler.m_threadCheckOk);
    }

    {
        // Check that the destructor of the pool waits for the end of all tasks
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
    // Use the pool
    PoolTestHandler handler;
    ::fwThread::Pool& pool = ::fwThread::getDefaultPool();

    std::vector< ::std::shared_future<void> > futures;
    futures.push_back( pool.post( ::std::bind( &PoolTestHandler::nextStep, &handler) ));
    futures.push_back( pool.post( ::std::bind( &PoolTestHandler::nextStep, &handler) ));
    futures.push_back( pool.post( ::std::bind( &PoolTestHandler::nextStep, &handler) ));

    std::for_each(futures.begin(), futures.end(), std::mem_fn(&::std::shared_future<void>::wait));

    CPPUNIT_ASSERT_EQUAL(3, handler.m_step);
    CPPUNIT_ASSERT_EQUAL(true, handler.m_threadCheckOk);

}
//-----------------------------------------------------------------------------

} //namespace ut
} //namespace fwThread
