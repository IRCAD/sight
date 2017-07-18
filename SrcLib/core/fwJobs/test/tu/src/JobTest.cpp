/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// for std::async to use lambda's return types
#define BOOST_RESULT_OF_USE_DECLTYPE

#include "JobTest.hpp"

#include <fwJobs/Aggregator.hpp>
#include <fwJobs/exception/Waiting.hpp>
#include <fwJobs/IJob.hpp>
#include <fwJobs/Job.hpp>
#include <fwJobs/Observer.hpp>

#include <fwCore/spyLog.hpp>

#include <fwTest/Exception.hpp>

#include <fwThread/Worker.hpp>

#include <boost/chrono/duration.hpp>

#include <exception>
#include <functional>
#include <future>
#include <string>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwJobs::ut::JobTest );

namespace fwJobs
{
namespace ut
{

//------------------------------------------------------------------------------

void JobTest::setUp()
{
}

//------------------------------------------------------------------------------

void JobTest::tearDown()
{
}

//------------------------------------------------------------------------------

void algoMockGenericCallback(int n, std::function< void(int) > progress, std::function< bool() > shouldCancel,
                             std::function< void(const std::string&) > log = nullptr )
{
    auto div = n/15;

    for(std::uint64_t i = 0; i < n; ++i)
    {
        // algo ...

        if (shouldCancel())
        {
            break;
        }

        if(log && i%div == 0)
        {
            log("algoMockGenericCallback step " + i);
        }

        progress(i+1);
    }
}

//------------------------------------------------------------------------------

void JobTest::APIAndStateTest()
{
    {
        ::fwJobs::Job job( "Job", [](::fwJobs::Job& runningJob)
                {
                } );
        CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::WAITING, job.getState() );

        CPPUNIT_ASSERT_THROW( job.wait(), ::fwJobs::exception::Waiting  );

        CPPUNIT_ASSERT_EQUAL( false, job.cancelRequested() );
        CPPUNIT_ASSERT_EQUAL( false, job.cancelRequestedCallback()() );

        job.cancel();
        CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::CANCELED, job.getState() );

        CPPUNIT_ASSERT_EQUAL( true, job.cancelRequested() );
        CPPUNIT_ASSERT_EQUAL( true, job.cancelRequestedCallback()() );

        CPPUNIT_ASSERT_NO_THROW( job.wait() );

        CPPUNIT_ASSERT( job.run().valid() );
        CPPUNIT_ASSERT( job.run().is_ready() );
        CPPUNIT_ASSERT( !job.run().has_value() );
    }

    {
        ::fwJobs::Job job( "Job", [](::fwJobs::Job& runningJob)
                {
                    CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::RUNNING, runningJob.getState() );
                } );
        CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::WAITING, job.getState() );

        CPPUNIT_ASSERT_THROW( job.wait(), ::fwJobs::exception::Waiting );

        job.run();
        CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::FINISHED, job.getState() );

        job.cancel();
        CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::FINISHED, job.getState() );
        CPPUNIT_ASSERT_NO_THROW( job.wait() );
    }

    {
        ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();
        ::fwJobs::Job job( "Job", [](::fwJobs::Job& runningJob)
                {
                    ::boost::this_thread::sleep_for( ::boost::chrono::milliseconds(30) );
                    CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::CANCELING, runningJob.getState() );
                }, worker );
        CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::WAITING, job.getState() );

        CPPUNIT_ASSERT_THROW( job.wait(), ::fwJobs::exception::Waiting );

        job.run();
        job.cancel();
        CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::CANCELING, job.getState() );

        job.wait();

        CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::CANCELED, job.getState() );
        CPPUNIT_ASSERT_NO_THROW( job.wait() );
    }

    {
        ::fwJobs::Observer job( "Observer" );
        CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::RUNNING, job.getState() );

        auto future = ::std::async(
            [&job]() -> bool
                {
                    bool except = true;
                    try
                    {
                        job.wait();
                        except = false;
                    }
                    catch ( ::fwJobs::exception::Waiting& )
                    {
                        except = true;
                    }
                    return except;
                }
            );

        CPPUNIT_ASSERT_EQUAL( false, job.cancelRequested() );
        CPPUNIT_ASSERT_EQUAL( false, job.cancelRequestedCallback()() );

        job.cancel();
        CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::CANCELED, job.getState() );
        job.finish();
        CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::CANCELED, job.getState() );

        CPPUNIT_ASSERT_EQUAL( true, job.cancelRequested() );
        CPPUNIT_ASSERT_EQUAL( true, job.cancelRequestedCallback()() );

        CPPUNIT_ASSERT_NO_THROW( job.wait() );

        CPPUNIT_ASSERT_EQUAL( false, future.get() );
    }

    {
        ::fwJobs::Observer job( "Observer" );
        CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::RUNNING, job.getState() );

        job.finish();
        CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::FINISHED, job.getState() );
        job.cancel();
        CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::FINISHED, job.getState() );

        CPPUNIT_ASSERT_EQUAL( false, job.cancelRequested() );
        CPPUNIT_ASSERT_EQUAL( false, job.cancelRequestedCallback()() );

        CPPUNIT_ASSERT_NO_THROW( job.wait() );
    }

}

//------------------------------------------------------------------------------

void JobTest::GenericCallbackTest()
{
    for (int i = 0; i < 10; ++i)
    {
        int loops = 100;
        {
            ::fwJobs::Observer job( "GenericCallbackJob" );

            CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::RUNNING, job.getState() );

            algoMockGenericCallback(loops, job.progressCallback(), job.cancelRequestedCallback());

            CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::RUNNING, job.getState() );
            job.finish();

            CPPUNIT_ASSERT_EQUAL( std::uint64_t(loops), job.getDoneWorkUnits() );

            CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::FINISHED, job.getState() );
        }

        {
            ::fwJobs::Job::Task func = [loops](::fwJobs::Job& runningJob)
                                       {
                                           algoMockGenericCallback(loops,
                                                                   runningJob.progressCallback(),
                                                                   runningJob.cancelRequestedCallback());
                                       };
            ::fwJobs::Job job( "GenericCallbackJob", func);

            CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::WAITING, job.getState() );

            job.run();
            CPPUNIT_ASSERT_EQUAL( std::uint64_t(loops), job.getDoneWorkUnits() );
            CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::FINISHED, job.getState() );
        }

        {
            ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();

            loops = 1 << 30;
            ::fwJobs::Job job( "GenericCallbackJob",
                               [loops](::fwJobs::Job& runningJob)
                    {
                        algoMockGenericCallback(loops,
                                                runningJob.progressCallback(), runningJob.cancelRequestedCallback());
                    },
                               worker);
            job.setTotalWorkUnits( loops );
            job.run();
            CPPUNIT_ASSERT_EQUAL( ::fwJobs::IJob::RUNNING, job.getState() );
            ::boost::this_thread::sleep_for( ::boost::chrono::milliseconds(30) );
            job.cancel();
            job.wait();
            CPPUNIT_ASSERT( ::fwJobs::IJob::CANCELING == job.getState()
                            || ::fwJobs::IJob::CANCELED == job.getState() );

            CPPUNIT_ASSERT( loops > job.getDoneWorkUnits() );
        }

    }
}

//------------------------------------------------------------------------------

void JobTest::AggregationTest()
{
    int loops = 100;

    { // Job aggregation test
        ::fwJobs::Job::Task func = [loops](::fwJobs::Job& runningJob)
                                   {
                                       algoMockGenericCallback(loops,
                                                               runningJob.progressCallback(),
                                                               runningJob.cancelRequestedCallback());
                                   };
        auto job1 = ::fwJobs::Job::New( "GenericCallbackJob1", func );
        auto job2 = ::fwJobs::Job::New( "GenericCallbackJob2", func );
        auto job3 = ::fwJobs::Job::New( "GenericCallbackJob3", func );

        auto jobs1 = ::fwJobs::Aggregator::New( "Aggregator1" );

        jobs1->add(job1);
        jobs1->add(job2);
        jobs1->add(job3);

        CPPUNIT_ASSERT_EQUAL( std::uint64_t(0), jobs1->getDoneWorkUnits() );

        jobs1->run();
        jobs1->wait();

        CPPUNIT_ASSERT_EQUAL( std::uint64_t(300), jobs1->getTotalWorkUnits());
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(300), jobs1->getDoneWorkUnits() );
    }

    { // Job and aggregator aggregation test
        ::fwJobs::Job::Task func = [loops](::fwJobs::Job& runningJob)
                                   {
                                       algoMockGenericCallback(loops,
                                                               runningJob.progressCallback(),
                                                               runningJob.cancelRequestedCallback());
                                   };
        auto job1 = ::fwJobs::Job::New( "GenericCallbackJob1", func );
        auto job2 = ::fwJobs::Job::New( "GenericCallbackJob2", func );
        auto job3 = ::fwJobs::Job::New( "GenericCallbackJob3", func );
        auto job4 = ::fwJobs::Job::New( "GenericCallbackJob4", func );

        auto jobs1 = ::fwJobs::Aggregator::New( "Aggregator1" );
        auto jobs2 = ::fwJobs::Aggregator::New( "Aggregator2" );

        jobs1->add(job1);
        jobs1->add(job2);
        jobs1->add(job3);

        jobs2->add(jobs1);
        jobs2->add(job4);

        jobs2->run();
        jobs2->wait();

        CPPUNIT_ASSERT_EQUAL( std::uint64_t(100), job1->getDoneWorkUnits());

        CPPUNIT_ASSERT_EQUAL( std::uint64_t(200), jobs2->getTotalWorkUnits());
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(200), jobs2->getDoneWorkUnits());
    }

    { // total work units update test
        auto funcGen = []( int progressSteps ) -> ::fwJobs::Job::Task
                       {
                           return [ = ](::fwJobs::Job &runningJob)
                                  {
                                      algoMockGenericCallback(progressSteps, runningJob.progressCallback(),
                                                              runningJob.cancelRequestedCallback(),
                                                              nullptr);
                                  };
                       };

        auto job1 = ::fwJobs::Job::New( "GenericCallbackJob1", funcGen(55) );
        auto job2 = ::fwJobs::Job::New( "GenericCallbackJob2", funcGen(145) );
        auto job3 = ::fwJobs::Job::New( "GenericCallbackJob3", funcGen(9999) );
        auto job4 = ::fwJobs::Job::New( "GenericCallbackJob4", funcGen(3) );

        auto jobs1 = ::fwJobs::Aggregator::New( "Aggregator1" );
        auto jobs2 = ::fwJobs::Aggregator::New( "Aggregator2" );

        CPPUNIT_ASSERT_EQUAL( std::uint64_t(0), jobs1->getTotalWorkUnits());
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(0), jobs2->getTotalWorkUnits());

        job1->setTotalWorkUnits(55);

        jobs1->add(job1);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(100), jobs1->getTotalWorkUnits());

        jobs1->add(job2);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(200), jobs1->getTotalWorkUnits());

        jobs1->add(job3);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(300), jobs1->getTotalWorkUnits());

        job2->setTotalWorkUnits(145);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(300), jobs1->getTotalWorkUnits());

        job2->setTotalWorkUnits(0);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(200), jobs1->getTotalWorkUnits());

        job2->setTotalWorkUnits(100);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(300), jobs1->getTotalWorkUnits());

        // set back to 145 to be consistent with job's task
        job2->setTotalWorkUnits(145);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(300), jobs1->getTotalWorkUnits());

        jobs2->add(jobs1);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(100), jobs2->getTotalWorkUnits());

        jobs2->add(job4);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(200), jobs2->getTotalWorkUnits());

        job3->setTotalWorkUnits(9999);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(200), jobs2->getTotalWorkUnits());

        job4->setTotalWorkUnits(3);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(200), jobs2->getTotalWorkUnits());

        jobs2->run();
        jobs2->wait();

        CPPUNIT_ASSERT_EQUAL( std::uint64_t(200), jobs2->getTotalWorkUnits());

        CPPUNIT_ASSERT_EQUAL( std::uint64_t(300), jobs1->getDoneWorkUnits());

        CPPUNIT_ASSERT_EQUAL( std::uint64_t(200), jobs2->getDoneWorkUnits());
    }

    {
        ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();

        ::fwJobs::Job::Task func = [loops](::fwJobs::Job& runningJob)
                                   {
                                       algoMockGenericCallback(loops,
                                                               runningJob.progressCallback(),
                                                               runningJob.cancelRequestedCallback());
                                   };

        auto job1 = ::fwJobs::Job::New( "GenericCallbackJob1", func, worker );
        auto job2 = ::fwJobs::Job::New( "GenericCallbackJob2", func, worker );
        auto job3 = ::fwJobs::Job::New( "GenericCallbackJob3", func, worker );

        loops = 1 << 30;
        func  = [loops](::fwJobs::Job& runningJob)
                {
                    algoMockGenericCallback(loops, runningJob.progressCallback(), runningJob.cancelRequestedCallback());
                };
        auto job4 = ::fwJobs::Job::New( "GenericCallbackJob4", func, worker );
        job4->setTotalWorkUnits(loops);

        auto jobs1 = ::fwJobs::Aggregator::New( "Aggregator1" );
        auto jobs2 = ::fwJobs::Aggregator::New( "Aggregator2" );

        jobs1->add(job1);
        jobs1->add(job2);
        jobs1->add(job3);

        jobs2->add(jobs1);
        jobs2->add(job4);

        auto future = jobs2->run();
        ::boost::this_thread::sleep_for( ::boost::chrono::milliseconds(30) );
        jobs2->cancel();
        jobs2->wait();

        CPPUNIT_ASSERT_EQUAL(
            100 * ( (job4->getDoneWorkUnits() / job4->getTotalWorkUnits())
                    + ((job3->getDoneWorkUnits() / job3->getTotalWorkUnits())
                       + (job2->getDoneWorkUnits() / job2->getTotalWorkUnits())
                       + (job1->getDoneWorkUnits() / job1->getTotalWorkUnits()))/3 ),
            jobs2->getDoneWorkUnits());

    }

    {
        ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();

        int index = -1;

        ::fwJobs::Job::Task func1 = [&index](::fwJobs::Job& runningJob)
                                    {
                                        CPPUNIT_ASSERT_EQUAL(-1, index);
                                        index = 0;
                                    };

        ::fwJobs::Job::Task func2 = [&index](::fwJobs::Job& runningJob)
                                    {
                                        CPPUNIT_ASSERT_EQUAL(0, index);
                                        index = 1;
                                    };

        ::fwJobs::Job::Task func3 = [&index](::fwJobs::Job& runningJob)
                                    {
                                        CPPUNIT_ASSERT_EQUAL(1, index);
                                        index = 2;
                                    };

        auto job1 = std::make_shared< ::fwJobs::Job >( "GenericCallbackJob1", func1, worker );
        auto job2 = std::make_shared< ::fwJobs::Job >( "GenericCallbackJob2", func2, worker );
        auto job3 = std::make_shared< ::fwJobs::Job >( "GenericCallbackJob3", func3, worker );

        auto jobs1 = std::make_shared< ::fwJobs::Aggregator >( "Aggregator1" );
        jobs1->add(job1);
        jobs1->add(job1);
        jobs1->add(job2);
        jobs1->add(job3);
        jobs1->add(job3);
        jobs1->add(job2);

        CPPUNIT_ASSERT_EQUAL((decltype(jobs1->getSubJobs().size())) 3, jobs1->getSubJobs().size());

        jobs1->run().get();

    }

    { // weight test
        auto funcGen = []() -> ::fwJobs::Job::Task
                       {
                           return [ = ](::fwJobs::Job &runningJob)
                                  {
                                      runningJob.done();
                                  };
                       };

        std::uint64_t norm = 100;
        std::uint64_t wu1  = 55, wu2 = 444, wu3 = 9999;
        double w1          = 2, w2 = 42, w3 = 0.5;

        auto job1 = ::fwJobs::Job::New( "GenericCallbackJob1", funcGen() );
        auto job2 = ::fwJobs::Observer::New( "GenericCallbackJob2", wu2 );
        auto job3 = ::fwJobs::Job::New( "GenericCallbackJob3", funcGen() );

        job1->setTotalWorkUnits(wu1);
        job3->setTotalWorkUnits(wu3);

        auto jobs1 = ::fwJobs::Aggregator::New( "Aggregator1" );
        auto jobs2 = ::fwJobs::Aggregator::New( "Aggregator2" );

        jobs1->add(job1, w1);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(w1*norm), jobs1->getTotalWorkUnits());

        jobs1->add(job2, w2);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t((w2+w1)*norm), jobs1->getTotalWorkUnits());

        jobs2->add(job3, w3);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(norm*w3), jobs2->getTotalWorkUnits());

        wu2 = 145;
        job2->setTotalWorkUnits(wu2);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(norm*(w2+w1)), jobs1->getTotalWorkUnits());

        double jobs1w = 2015;
        jobs2->add(jobs1, jobs1w);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(norm * (jobs1w+w3)), jobs2->getTotalWorkUnits());

        wu3 = 1111;
        job3->setTotalWorkUnits(wu3);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(norm * (jobs1w+w3)), jobs2->getTotalWorkUnits());

        jobs2->run();
        job2->done();
        job2->finish();
        jobs2->wait();

        CPPUNIT_ASSERT_EQUAL( std::uint64_t(norm*(w2+w1)), jobs1->getDoneWorkUnits());
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(norm*(jobs1w+w3)), jobs2->getDoneWorkUnits());
    }
}

//------------------------------------------------------------------------------

class ProgressObserver
{
public:
    virtual void progressNotify( double p ) = 0;
    virtual bool canceled()                 = 0;
    virtual ~ProgressObserver()
    {
    }
};

class algoMockObserver
{
public:
    algoMockObserver( ProgressObserver* obs ) :
        m_obs(obs),
        m_canceled(false)
    {
    }

    ~algoMockObserver()
    {
        if(m_obs)
        {
            delete m_obs;
        }
    }

    //------------------------------------------------------------------------------

    void run(int n)
    {
        for(int i = 0; i < n; i++)
        {
            // algo ...
            if (m_obs)
            {
                m_obs->progressNotify(((double)(i+1))/n);

                if (m_obs->canceled())
                {
                    break;
                }
            }

            if (m_canceled)
            {
                break;
            }
        }
    }

    //------------------------------------------------------------------------------

    void cancel()
    {
        m_canceled = true;
    }

protected:

    ProgressObserver* m_obs;
    bool m_canceled;

};

struct JobObserver : public ProgressObserver
{
    JobObserver( std::function< void ( double ) > func) :
        m_callback( func )
    {
    }

    //------------------------------------------------------------------------------

    void progressNotify( double p )
    {
        m_callback( p );
    }

    //------------------------------------------------------------------------------

    bool canceled()
    {
        return false;
    }

    std::function< void ( double ) > m_callback;
};

struct JobObserverCanceler : public JobObserver
{
    JobObserverCanceler( std::function< void ( double ) > func, const bool& canceled = false) :
        JobObserver(func),
        m_canceled( canceled )
    {
    }

    //------------------------------------------------------------------------------

    bool canceled()
    {
        return m_canceled;
    }

    const bool& m_canceled;
};

//------------------------------------------------------------------------------

void JobTest::ObserverTest()
{
    std::uint64_t progress(100);
    for (int i = 0; i < 10; ++i)
    {
        int loops = 100;
        {
            ::fwJobs::Observer job( "GenericCallbackJob" );

            auto f = [ =, &job](double d)
                     {
                         job.doneWork(std::uint64_t(d * progress));
                     };
            algoMockObserver algo( new JobObserver( f ) );

            algo.run(loops);
            job.finish();

            CPPUNIT_ASSERT_EQUAL( progress, job.getDoneWorkUnits() );
        }

        {
            ::fwJobs::Job job( "GenericCallbackJob",
                               [ = ](::fwJobs::Job& job)
                    {
                        auto f = [ =, &job](double d)
                                 {
                                     job.doneWork(std::uint64_t(d * progress));
                                 };
                        algoMockObserver algo( new JobObserver( f ) );
                        algo.run(loops);
                    });
            job.run();
            CPPUNIT_ASSERT_EQUAL( progress, job.getDoneWorkUnits() );
        }

        {
            ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();

            loops = 1 << 30;
            ::fwJobs::Job job( "GenericCallbackJob",
                               [ = ](::fwJobs::Job& runningJob)
                    {
                        auto f = [ =, &runningJob](double d)
                                 {
                                     runningJob.doneWork(std::uint64_t(d * progress));
                                 };
                        algoMockObserver algo( new JobObserver( f ) );
                        runningJob.addSimpleCancelHook( [&]()
                        {
                            algo.cancel();
                        } );
                        algo.run(loops);
                    },
                               worker
                               );
            job.setTotalWorkUnits(loops);
            job.run();
            ::boost::this_thread::sleep_for( ::boost::chrono::milliseconds(30) );
            job.cancel().wait();
            CPPUNIT_ASSERT( progress > job.getDoneWorkUnits() );
        }

        {
            ::fwThread::Worker::sptr worker = ::fwThread::Worker::New();

            loops = 1 << 30;
            ::fwJobs::Job job( "GenericCallbackJob",
                               [ = ](::fwJobs::Job& runningJob)
                    {
                        auto f = [ =, &runningJob](double d)
                                 {
                                     runningJob.doneWork(std::uint64_t(d * progress));
                                 };
                        algoMockObserver algo( new JobObserverCanceler( f, runningJob.cancelRequested() ) );
                        algo.run(loops);
                    },
                               worker
                               );
            job.setTotalWorkUnits(loops);
            job.run();
            ::boost::this_thread::sleep_for( ::boost::chrono::milliseconds(30) );
            job.cancel().wait();
            CPPUNIT_ASSERT( progress > job.getDoneWorkUnits() );
        }
    }
}

//------------------------------------------------------------------------------

void JobTest::LogTest()
{
    int loops = 100;
    {
        ::fwJobs::Observer job( "GenericCallbackJob1" );

        job.log("Test of GenericCallbackJob1");
        algoMockGenericCallback(loops, job.progressCallback(), job.cancelRequestedCallback(),
                                [&job](const std::string& message)
                {
                    job.log(message);
                });

        job.finish();
    }

    {
        ::fwJobs::Job::Task func = [loops](::fwJobs::Job& runningJob)
                                   {
                                       algoMockGenericCallback(loops,
                                                               runningJob.progressCallback(),
                                                               runningJob.cancelRequestedCallback(),
                                                               [&runningJob](const std::string& message)
                    {
                        runningJob.log(message);
                    });
                                   };
        ::fwJobs::Job job( "GenericCallbackJob2", func);

        job.log("Test of GenericCallbackJob2");

        job.run();
    }

    {
        ::fwJobs::Job::Task func = [loops](::fwJobs::Job& runningJob)
                                   {
                                       algoMockGenericCallback(loops,
                                                               runningJob.progressCallback(),
                                                               runningJob.cancelRequestedCallback(),
                                                               [&runningJob](const std::string& message)
                    {
                        runningJob.log(message);
                    });
                                   };
        auto job1 = ::fwJobs::Job::New( "GenericCallbackJob1", func );
        auto job2 = ::fwJobs::Job::New( "GenericCallbackJob2", func );
        auto job3 = ::fwJobs::Job::New( "GenericCallbackJob3", func );
        auto job4 = ::fwJobs::Job::New( "GenericCallbackJob4", func );

        auto jobs1 = ::fwJobs::Aggregator::New( "Aggregator1" );
        auto jobs2 = ::fwJobs::Aggregator::New( "Aggregator2" );

        jobs1->add(job1);
        jobs1->add(job2);
        jobs1->add(job3);

        job1->log("Log test of GenericCallbackJob1");
        job2->log("Log test of GenericCallbackJob2");

        jobs2->add(jobs1);
        jobs2->add(job4);

        job3->log("Log test of GenericCallbackJob3");
        job4->log("Log test of GenericCallbackJob4");

        jobs1->log("Aggregator1");
        jobs2->log("Aggregator2");

        jobs2->run();
        jobs2->wait();

        auto nbLogs = 18;
        CPPUNIT_ASSERT_EQUAL( size_t(nbLogs), job1->getLogs().size() );
        CPPUNIT_ASSERT_EQUAL( size_t(nbLogs), job2->getLogs().size() );
        CPPUNIT_ASSERT_EQUAL( size_t(nbLogs), job3->getLogs().size() );
        CPPUNIT_ASSERT_EQUAL( size_t(nbLogs), job4->getLogs().size() );
        CPPUNIT_ASSERT_EQUAL( size_t(3*nbLogs+1), jobs1->getLogs().size() );
        CPPUNIT_ASSERT_EQUAL( size_t((3*nbLogs+1)+nbLogs+1), jobs2->getLogs().size() );
    }
}

} //namespace ut
} //namespace fwJobs
