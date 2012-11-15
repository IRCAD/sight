/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <exception>

#include <fwCore/spyLog.hpp>

#include <fwTest/Exception.hpp>

#include <fwThread/Worker.hpp>

#include "WorkerTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwThread::ut::WorkerTest );

namespace fwThread
{
namespace ut
{

static ::fwTest::Exception e(""); // force link with fwTest

void WorkerTest::setUp()
{
    // Set up context before running a test.
}
void WorkerTest::tearDown()
{
    // Clean up after the test run.
}


//-----------------------------------------------------------------------------

struct TestHandler
{
    TestHandler() : m_step(0), m_threadCheckOk(true)
    {
        m_constructorThreadId = ::fwThread::getCurrentThreadId();
    }

    void nextStep()
    {
        ::boost::this_thread::sleep(::boost::posix_time::seconds(1));
        m_threadCheckOk &= (m_constructorThreadId != ::fwThread::getCurrentThreadId());
        m_threadCheckOk &= (m_workerThreadId == ::fwThread::getCurrentThreadId());
        ++m_step;
    }

    void setWorkerId(::fwThread::ThreadIdType id)
    {
        m_workerThreadId = id;
    }

    int m_step;
    bool m_threadCheckOk;
    ::fwThread::ThreadIdType m_constructorThreadId;
    ::fwThread::ThreadIdType m_workerThreadId;
};

//-----------------------------------------------------------------------------

void WorkerTest::basicTest()
{
    ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();

    TestHandler handler;
    handler.setWorkerId(worker->getThreadId());
    worker->post( ::boost::bind( &TestHandler::nextStep, &handler) );
    worker->post( ::boost::bind( &TestHandler::nextStep, &handler) );
    worker->post( ::boost::bind( &TestHandler::nextStep, &handler) );

    worker->stop();
    CPPUNIT_ASSERT_EQUAL(3, handler.m_step);
    CPPUNIT_ASSERT_EQUAL(true, handler.m_threadCheckOk);
}

//-----------------------------------------------------------------------------

} //namespace ut
} //namespace fwThread
