/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "TaskHandlerTest.hpp"

#include <fwThread/TaskHandler.hpp>
#include <fwThread/Worker.hpp>

#include <fwCore/spyLog.hpp>

#include <exception>
#include <iostream>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwThread::ut::TaskHandlerTest );

namespace fwThread
{
namespace ut
{

//------------------------------------------------------------------------------

void TaskHandlerTest::setUp()
{
    // Set up context before running a test.
}
//------------------------------------------------------------------------------

void TaskHandlerTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

int copy(int val)
{
    std::this_thread::sleep_for( std::chrono::seconds(1));
    return val;
}

//-----------------------------------------------------------------------------

void throwException()
{
    throw std::exception();
}

//-----------------------------------------------------------------------------

void TaskHandlerTest::basicTest()
{
    ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();

    std::packaged_task<int()> task( std::bind( &copy, 5) );
    std::future< int > future = task.get_future();
    std::function< void() > f = moveTaskIntoFunction(task);

    std::packaged_task<int()> task2( std::bind( &copy, 8) );
    std::future< int > future2 = task2.get_future();
    std::function< void() > f2 = moveTaskIntoFunction(task2);

    worker->post(f);
    worker->post(f2);

    CPPUNIT_ASSERT( future.valid() );
    CPPUNIT_ASSERT( future2.valid() );

    future.wait();
    CPPUNIT_ASSERT( future.valid() );
    CPPUNIT_ASSERT_EQUAL( 5, future.get() );
    CPPUNIT_ASSERT( !future.valid() );

    future2.wait();
    CPPUNIT_ASSERT( future2.valid() );
    CPPUNIT_ASSERT_EQUAL( 8, future2.get() );
    CPPUNIT_ASSERT( !future2.valid() );

    std::packaged_task<int()> task3( std::bind( &copy, 5) );
    std::future< int > future3 = task3.get_future();
    std::function< void() > f3 = moveTaskIntoFunction(task3);

    std::packaged_task<int()> task4( std::bind( &copy, 8) );
    std::future< int > future4 = task4.get_future();
    std::function< void() > f4 = moveTaskIntoFunction(task4);

    worker->post(f3);
    worker->post(f4);

    CPPUNIT_ASSERT( future3.valid() );
    CPPUNIT_ASSERT( future4.valid() );

    worker->stop();

    CPPUNIT_ASSERT( future3.valid() );
    CPPUNIT_ASSERT_EQUAL( 5, future3.get() );
    CPPUNIT_ASSERT( !future3.valid() );

    CPPUNIT_ASSERT( future4.valid() );
    CPPUNIT_ASSERT_EQUAL( 8, future4.get() );
    CPPUNIT_ASSERT( !future4.valid() );
}

//-----------------------------------------------------------------------------

void TaskHandlerTest::exceptionTest()
{
    ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();

    std::packaged_task<void()> task( std::bind( &throwException ) );
    std::future< void > future = task.get_future();
    std::function< void() > f  = moveTaskIntoFunction(task);

    worker->post(f);

    future.wait();
    worker->stop();

    bool exceptionIsCatched = false;
    try
    {
        future.get();
    }
    catch (std::exception&)
    {
        exceptionIsCatched = true;
    }
    CPPUNIT_ASSERT(exceptionIsCatched);
}

//-----------------------------------------------------------------------------

} //namespace ut
} //namespace fwThread
