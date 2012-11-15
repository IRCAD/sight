/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <exception>

#include <fwCore/spyLog.hpp>

#include <fwThread/Worker.hpp>
#include <fwThread/TaskHandler.hpp>

#include "TaskHandlerTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwThread::ut::TaskHandlerTest );

namespace fwThread
{
namespace ut
{

void TaskHandlerTest::setUp()
{
    // Set up context before running a test.
}
void TaskHandlerTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

int copy(int val)
{
    ::boost::this_thread::sleep(::boost::posix_time::seconds(1));
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

    ::boost::packaged_task<int> task( ::boost::bind( &copy, 5) );
    ::boost::unique_future< int > future = task.get_future();
    ::boost::function< void () > f = moveTaskIntoFunction(task);

    ::boost::packaged_task<int> task2( ::boost::bind( &copy, 8) );
    ::boost::unique_future< int > future2 = task2.get_future();
    ::boost::function< void () > f2 = moveTaskIntoFunction(task2);

    worker->post(f);
    worker->post(f2);

    future.wait();
    CPPUNIT_ASSERT( future.is_ready() );
    CPPUNIT_ASSERT( future.has_value() );
    CPPUNIT_ASSERT( !future.has_exception() );
    CPPUNIT_ASSERT_EQUAL( 5, future.get() );

    future2.wait();
    CPPUNIT_ASSERT( future2.is_ready() );
    CPPUNIT_ASSERT( future2.has_value() );
    CPPUNIT_ASSERT( !future2.has_exception() );
    CPPUNIT_ASSERT_EQUAL( 8, future2.get() );


    ::boost::packaged_task<int> task3( ::boost::bind( &copy, 5) );
    ::boost::unique_future< int > future3 = task3.get_future();
    ::boost::function< void () > f3 = moveTaskIntoFunction(task3);

    ::boost::packaged_task<int> task4( ::boost::bind( &copy, 8) );
    ::boost::unique_future< int > future4 = task4.get_future();
    ::boost::function< void () > f4 = moveTaskIntoFunction(task4);

    worker->post(f3);
    worker->post(f4);

    worker->stop();

    CPPUNIT_ASSERT( future3.is_ready() );
    CPPUNIT_ASSERT( future3.has_value() );
    CPPUNIT_ASSERT( !future3.has_exception() );
    CPPUNIT_ASSERT_EQUAL( 5, future3.get() );

    CPPUNIT_ASSERT( future4.is_ready() );
    CPPUNIT_ASSERT( future4.has_value() );
    CPPUNIT_ASSERT( !future4.has_exception() );
    CPPUNIT_ASSERT_EQUAL( 8, future4.get() );
}

//-----------------------------------------------------------------------------

void TaskHandlerTest::exceptionTest()
{
    ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();

    ::boost::packaged_task<void> task( ::boost::bind( &throwException ) );
    ::boost::unique_future< void > future = task.get_future();
    ::boost::function< void () > f = moveTaskIntoFunction(task);

    worker->post(f);

    future.wait();
    worker->stop();

    bool exceptionIsCatched = false;
    CPPUNIT_ASSERT( future.has_exception() );
    try
    {
        future.get();
    }
    catch (std::exception &)
    {
        exceptionIsCatched = true;
    }
    CPPUNIT_ASSERT(exceptionIsCatched);
}

//-----------------------------------------------------------------------------

} //namespace ut
} //namespace fwThread
