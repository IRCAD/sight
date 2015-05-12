/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/type_traits/is_same.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/chrono/duration.hpp>

#include <cppunit/Exception.h>

#include <QApplication>
#include <QTimer>

#include <fwThread/Timer.hpp>

#include <fwThread/Worker.hpp>
#include <fwThread/Worker.hxx>
#include <fwGuiQt/config.hpp>

#include "fwGuiQt/App.hpp"
#include "fwGuiQt/util/FuncSlot.hpp"

#include "WorkerQtTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwGuiQt::ut::WorkerQtTest );

namespace fwGuiQt
{

// Defined in WorkerQt.cpp
class WorkerQt;
struct FWGUIQT_CLASS_API WorkerQtInstanciator
{

    FWGUIQT_API WorkerQtInstanciator(bool reg = true );

    FWGUIQT_API SPTR(::fwThread::Worker) getWorker();

    SPTR(WorkerQt) m_qtWorker;

    FWGUIQT_API static int s_argc;
    FWGUIQT_API static char **s_argv;
    FWGUIQT_API static bool s_GUIenabled;
};



namespace ut
{

struct TestHandler
{
    TestHandler() : m_step(0), m_threadCheckOk(true)
    {
    }

    void nextStep()
    {
        ::boost::this_thread::sleep_for( ::boost::chrono::milliseconds(50));
        this->nextStepNoSleep();
    }

    void nextStepNoSleep()
    {
        m_threadCheckOk &= (m_workerThreadId == ::fwThread::getCurrentThreadId());
        ++m_step;
    }

    void setWorkerId(::fwThread::ThreadIdType id)
    {
        m_workerThreadId = id;
    }

    int m_step;
    bool m_threadCheckOk;
    ::fwThread::ThreadIdType m_workerThreadId;
};

//-----------------------------------------------------------------------------



void WorkerQtTest::setUp()
{
    // Set up context before running a test.
    char arg1[] = "fwGuiQtTest";
#if defined(__linux)
    char arg2[]        = "-platform";
    char arg3[]        = "offscreen";
    static char* arg[] = {arg1, arg2, arg3, 0};

    WorkerQtInstanciator::s_argc = 3;

#else
    static char* arg[] = {arg1, 0};
    WorkerQtInstanciator::s_argc = 1;
#endif

#if QT_VERSION < 0x050000
    WorkerQtInstanciator::s_GUIenabled = false;
#endif

    WorkerQtInstanciator::s_argv = arg;
    CPPUNIT_ASSERT(qApp == NULL);
    WorkerQtInstanciator instanciator(false);
    m_worker = instanciator.getWorker();

}
void WorkerQtTest::tearDown()
{
    // Clean up after the test run.
    m_worker.reset();
    CPPUNIT_ASSERT(qApp == NULL);
}

//-----------------------------------------------------------------------------

void WorkerQtTest::initTest()
{
    CPPUNIT_ASSERT(qApp != NULL);
}

//-----------------------------------------------------------------------------

void WorkerQtTest::twiceInitTest()
{
    CPPUNIT_ASSERT(qApp != NULL);
}

//-----------------------------------------------------------------------------

void runBasicTest(TestHandler &handler, ::fwThread::Worker::sptr worker)
{

    handler.setWorkerId(worker->getThreadId());
    worker->post( ::boost::bind( &TestHandler::nextStep, &handler) );
    worker->post( ::boost::bind( &TestHandler::nextStep, &handler) );
    worker->post( ::boost::bind( &TestHandler::nextStep, &handler) );

    worker->post( ::boost::bind( &QApplication::quit ) );

}


#define RUN_BASIC_TEST_CHECKS(handler) \
    CPPUNIT_ASSERT_EQUAL(3, handler.m_step); \
    CPPUNIT_ASSERT_EQUAL(true, handler.m_threadCheckOk)



void WorkerQtTest::basicTest()
{
    TestHandler handler;

    runBasicTest(handler, m_worker);

    m_worker->getFuture().wait();

    RUN_BASIC_TEST_CHECKS(handler);
}

//-----------------------------------------------------------------------------

void WorkerQtTest::postFromInsideTest()
{
    TestHandler handler;

    m_worker->post( boost::bind(&runBasicTest, boost::ref(handler), m_worker) );

    m_worker->getFuture().wait();

    RUN_BASIC_TEST_CHECKS(handler);
}

//-----------------------------------------------------------------------------
void doNothing()
{
}

void runFromOutsideTest(TestHandler &handler, ::fwThread::Worker::sptr worker)
{
    //waiting for WorkerQt to start
    worker->postTask<void>( ::boost::bind( &doNothing ) ).wait();

    runBasicTest(handler, worker);
}

void WorkerQtTest::postFromOutsideTest()
{
    TestHandler handler;

    ::boost::thread testThread(
        boost::bind(&runFromOutsideTest, boost::ref(handler), m_worker)
        );

    m_worker->getFuture().wait();

    RUN_BASIC_TEST_CHECKS(handler);
}

//-----------------------------------------------------------------------------

#define QT_TEST_START \
    try


#define QT_TEST_END \
    catch(CppUnit::Exception &e) \
    { \
        std::cerr << e.what() << std::endl; \
        exception = e; \
        QApplication::exit(1); \
        return; \
    }

static CppUnit::Exception exception;

void echo()
{
}



void runBasicTimerTest(
    TestHandler &handler,
    const ::fwThread::Timer::sptr &timer,
    ::fwThread::Timer::TimeDurationType duration
    )
{
    timer->start();

    QT_TEST_START
    {
        CPPUNIT_ASSERT(timer->isRunning());
        CPPUNIT_ASSERT(handler.m_threadCheckOk);
        CPPUNIT_ASSERT_EQUAL(0, handler.m_step);
    }
    QT_TEST_END
}


void oneShotBasicTimerTest(
    int &i,
    TestHandler &handler,
    const ::fwThread::Timer::sptr &timer,
    ::fwThread::Timer::TimeDurationType duration,
    const ::fwThread::Worker::sptr &worker
    )
{

    handler.nextStepNoSleep();

    QT_TEST_START
    {
        CPPUNIT_ASSERT(timer->isRunning());
        CPPUNIT_ASSERT(handler.m_threadCheckOk);
        CPPUNIT_ASSERT_EQUAL(i, handler.m_step);
    }
    QT_TEST_END;


    if(++i == 50)
    {
        timer->stop();

        QT_TEST_START
        {
            CPPUNIT_ASSERT(!timer->isRunning());
            CPPUNIT_ASSERT(handler.m_threadCheckOk);
            CPPUNIT_ASSERT_EQUAL(49, handler.m_step);
        }
        QT_TEST_END;
        worker->post( ::boost::bind( &QApplication::quit ) );
    }
}

void WorkerQtTest::basicTimerTest()
{
    {

        TestHandler handler;
        handler.setWorkerId(m_worker->getThreadId());

        ::fwThread::Timer::sptr timer = m_worker->createTimer();

        ::fwThread::Timer::TimeDurationType duration = ::boost::chrono::milliseconds(10);

        int i = 1;
        timer->setFunction(
            ::boost::bind(
                &oneShotBasicTimerTest,
                boost::ref(i), handler, ::boost::ref(timer), duration, ::boost::ref(m_worker) )
            );
        timer->setDuration(duration);

        CPPUNIT_ASSERT(!timer->isRunning());
        CPPUNIT_ASSERT(handler.m_threadCheckOk);
        CPPUNIT_ASSERT_EQUAL(0, handler.m_step);

        m_worker->post( boost::bind(&runBasicTimerTest, ::boost::ref(handler), ::boost::ref(timer), duration) );


        ::fwThread::Worker::FutureType future = m_worker->getFuture();
        future.wait();

        CPPUNIT_ASSERT( future.has_value() );
        CPPUNIT_ASSERT_EQUAL( 0, boost::any_cast<int>( future.get() ) );
    }


}

//-----------------------------------------------------------------------------



} //namespace ut
} //namespace fwGuiQt
