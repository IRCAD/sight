/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <exception>

#include <boost/thread.hpp>

#include <fwCore/util/LazyInstantiator.hpp>
#include <fwCore/mt/types.hpp>

#include "LazyInstantiatorTest.hpp"


// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwCore::ut::LazyInstantiatorTest );

namespace fwCore
{
namespace ut
{

void LazyInstantiatorTest::setUp()
{
    // Set up context before running a test.
}

//-----------------------------------------------------------------------------

void LazyInstantiatorTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

template < int SLEEP = 0 >
class StaticCounter
{
public:
    typedef ::boost::shared_ptr< StaticCounter > sptr;

    StaticCounter()
    {
        ::fwCore::mt::ScopedLock lock(s_mutex);
        ++s_counter;
        ::boost::this_thread::sleep(::boost::posix_time::seconds(SLEEP));
    }

    static int s_counter;
    static ::fwCore::mt::Mutex s_mutex;
};

template < int SLEEP >
int StaticCounter<SLEEP>::s_counter = 0;

template < int SLEEP >
::fwCore::mt::Mutex StaticCounter<SLEEP>::s_mutex;

struct second_counter{};

//-----------------------------------------------------------------------------

void LazyInstantiatorTest::lazyTest()
{
    StaticCounter<>::s_counter = 0;

    StaticCounter<> cpt;

    CPPUNIT_ASSERT_EQUAL(1, cpt.s_counter);

    StaticCounter<>::sptr counter1 = ::fwCore::util::LazyInstantiator< StaticCounter<> >::getInstance();
    CPPUNIT_ASSERT_EQUAL(2, counter1->s_counter);

    StaticCounter<>::sptr counter2 = ::fwCore::util::LazyInstantiator< StaticCounter<> >::getInstance();
    CPPUNIT_ASSERT_EQUAL(2, counter2->s_counter);

    StaticCounter<>::sptr counter3 = ::fwCore::util::LazyInstantiator< StaticCounter<>, second_counter >::getInstance();
    CPPUNIT_ASSERT_EQUAL(3, counter3->s_counter);

    StaticCounter<>::s_counter = 0;
}

//-----------------------------------------------------------------------------

struct thread_counter_tag {} ;

struct CounterThread
{
    typedef StaticCounter<5> CounterType;

    typedef ::boost::shared_ptr< CounterThread > sptr;
    void run ()
    {
        CounterType::sptr counter;
        counter = ::fwCore::util::LazyInstantiator< CounterType , thread_counter_tag >::getInstance();
    }
};

//-----------------------------------------------------------------------------

void LazyInstantiatorTest::threadSafetyTest()
{
    const int NB_THREAD(100);
    CounterThread::CounterType::s_counter = 0;

    ::boost::thread_group tg;
    for(size_t i = 0; i <= NB_THREAD; i++)
    {
        CounterThread::sptr ct = ::boost::make_shared<CounterThread>();
        ::boost::thread* t  = new ::boost::thread(::boost::bind(&CounterThread::run, ct) );
        tg.add_thread(t);
    }
    tg.join_all();

    CounterThread::CounterType::sptr counter;
    counter = ::fwCore::util::LazyInstantiator< CounterThread::CounterType, thread_counter_tag >::getInstance();
    CPPUNIT_ASSERT_EQUAL(1, counter->s_counter);

    CounterThread::CounterType::s_counter = 0;
}

} //namespace ut
} //namespace fwCore
