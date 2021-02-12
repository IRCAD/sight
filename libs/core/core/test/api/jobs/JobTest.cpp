/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "JobTest.hpp"

#include <core/jobs/Aggregator.hpp>
#include <core/jobs/exception/Waiting.hpp>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Job.hpp>
#include <core/jobs/Observer.hpp>
#include <core/spyLog.hpp>
#include <core/thread/Worker.hpp>

#include <exception>
#include <functional>
#include <future>
#include <string>
#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::sight::core::jobs::ut::JobTest );

namespace sight::core::jobs
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
    int div = n/15;

    for(int i = 0; i < n; ++i)
    {
        // algo ...

        if (shouldCancel())
        {
            break;
        }

        if(log && i%div == 0)
        {
            log("algoMockGenericCallback step " + std::to_string(i));
        }

        progress(i+1);
    }
}

//------------------------------------------------------------------------------

void JobTest::APIAndStateTest()
{
    {
        core::jobs::Job job( "Job", [](core::jobs::Job&)
                    {
                    } );
        CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::WAITING, job.getState() );

        CPPUNIT_ASSERT_THROW( job.wait(), core::jobs::exception::Waiting  );

        CPPUNIT_ASSERT_EQUAL( false, job.cancelRequested() );
        CPPUNIT_ASSERT_EQUAL( false, job.cancelRequestedCallback()() );

        job.cancel();
        CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::CANCELED, job.getState() );

        CPPUNIT_ASSERT_EQUAL( true, job.cancelRequested() );
        CPPUNIT_ASSERT_EQUAL( true, job.cancelRequestedCallback()() );

        CPPUNIT_ASSERT_NO_THROW( job.wait() );

        CPPUNIT_ASSERT( job.run().valid() );
    }

    {
        core::jobs::Job job( "Job", [](core::jobs::Job& runningJob)
                    {
                             CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::RUNNING, runningJob.getState() );
                    } );
        CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::WAITING, job.getState() );

        CPPUNIT_ASSERT_THROW( job.wait(), core::jobs::exception::Waiting );

        job.run();
        CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::FINISHED, job.getState() );

        job.cancel();
        CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::FINISHED, job.getState() );
        CPPUNIT_ASSERT_NO_THROW( job.wait() );
    }

    {
        core::thread::Worker::sptr worker = core::thread::Worker::New();
        core::jobs::Job job( "Job", [](core::jobs::Job& runningJob)
                    {
                             std::this_thread::sleep_for( std::chrono::milliseconds(30) );
                             CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::CANCELING, runningJob.getState() );
                    }, worker );
        CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::WAITING, job.getState() );

        CPPUNIT_ASSERT_THROW( job.wait(), core::jobs::exception::Waiting );

        job.run();
        job.cancel();
        CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::CANCELING, job.getState() );

        job.wait();

        CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::CANCELED, job.getState() );
        CPPUNIT_ASSERT_NO_THROW( job.wait() );
        worker->stop();
    }

    {
        core::jobs::Observer job( "Observer" );
        CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::RUNNING, job.getState() );

        auto future = std::async(
            [&job]() -> bool
                {
                    bool except = true;
                    try
                    {
                        job.wait();
                        except = false;
                    }
                    catch ( core::jobs::exception::Waiting& )
                    {
                        except = true;
                    }
                    return except;
                }
            );

        CPPUNIT_ASSERT_EQUAL( false, job.cancelRequested() );
        CPPUNIT_ASSERT_EQUAL( false, job.cancelRequestedCallback()() );

        job.cancel();
        CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::CANCELED, job.getState() );
        job.finish();
        CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::CANCELED, job.getState() );

        CPPUNIT_ASSERT_EQUAL( true, job.cancelRequested() );
        CPPUNIT_ASSERT_EQUAL( true, job.cancelRequestedCallback()() );

        CPPUNIT_ASSERT_NO_THROW( job.wait() );

        CPPUNIT_ASSERT_EQUAL( false, future.get() );
    }

    {
        core::jobs::Observer job( "Observer" );
        CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::RUNNING, job.getState() );

        job.finish();
        CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::FINISHED, job.getState() );
        job.cancel();
        CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::FINISHED, job.getState() );

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
            core::jobs::Observer job( "GenericCallbackJob" );

            CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::RUNNING, job.getState() );

            algoMockGenericCallback(loops, job.progressCallback(), job.cancelRequestedCallback());

            CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::RUNNING, job.getState() );
            job.finish();

            CPPUNIT_ASSERT_EQUAL( std::uint64_t(loops), job.getDoneWorkUnits() );

            CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::FINISHED, job.getState() );
        }

        {
            core::jobs::Job::Task func = [loops](core::jobs::Job& runningJob)
                                         {
                                             algoMockGenericCallback(loops,
                                                                     runningJob.progressCallback(),
                                                                     runningJob.cancelRequestedCallback());
                                         };
            core::jobs::Job job( "GenericCallbackJob", func);

            CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::WAITING, job.getState() );

            job.run();
            CPPUNIT_ASSERT_EQUAL( std::uint64_t(loops), job.getDoneWorkUnits() );
            CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::FINISHED, job.getState() );
        }

        {
            core::thread::Worker::sptr worker = core::thread::Worker::New();

            loops = 1 << 30;
            core::jobs::Job job( "GenericCallbackJob",
                                 [loops](core::jobs::Job& runningJob)
                        {
                                 algoMockGenericCallback(loops,
                                                         runningJob.progressCallback(),
                                                         runningJob.cancelRequestedCallback());
                        },
                                 worker);
            job.setTotalWorkUnits( std::uint64_t(loops) );
            job.run();
            CPPUNIT_ASSERT_EQUAL( core::jobs::IJob::RUNNING, job.getState() );
            std::this_thread::sleep_for( std::chrono::milliseconds(30) );
            job.cancel();
            job.wait();
            CPPUNIT_ASSERT( core::jobs::IJob::CANCELING == job.getState()
                            || core::jobs::IJob::CANCELED == job.getState() );

            CPPUNIT_ASSERT( static_cast< std::uint64_t>(loops) > job.getDoneWorkUnits() );
            worker->stop();
        }

    }
}

//------------------------------------------------------------------------------

void JobTest::AggregationTest()
{
    int loops = 100;

    { // Job aggregation test
        core::jobs::Job::Task func = [loops](core::jobs::Job& runningJob)
                                     {
                                         algoMockGenericCallback(loops,
                                                                 runningJob.progressCallback(),
                                                                 runningJob.cancelRequestedCallback());
                                     };
        auto job1 = core::jobs::Job::New( "GenericCallbackJob1", func );
        auto job2 = core::jobs::Job::New( "GenericCallbackJob2", func );
        auto job3 = core::jobs::Job::New( "GenericCallbackJob3", func );

        auto jobs1 = core::jobs::Aggregator::New( "Aggregator1" );

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
        core::jobs::Job::Task func = [loops](core::jobs::Job& runningJob)
                                     {
                                         algoMockGenericCallback(loops,
                                                                 runningJob.progressCallback(),
                                                                 runningJob.cancelRequestedCallback());
                                     };
        auto job1 = core::jobs::Job::New( "GenericCallbackJob1", func );
        auto job2 = core::jobs::Job::New( "GenericCallbackJob2", func );
        auto job3 = core::jobs::Job::New( "GenericCallbackJob3", func );
        auto job4 = core::jobs::Job::New( "GenericCallbackJob4", func );

        auto jobs1 = core::jobs::Aggregator::New( "Aggregator1" );
        auto jobs2 = core::jobs::Aggregator::New( "Aggregator2" );

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
        auto funcGen = []( int progressSteps ) -> core::jobs::Job::Task
                       {
                           return [ = ](core::jobs::Job& runningJob)
                                  {
                                      algoMockGenericCallback(progressSteps, runningJob.progressCallback(),
                                                              runningJob.cancelRequestedCallback(),
                                                              nullptr);
                                  };
                       };

        auto job1 = core::jobs::Job::New( "GenericCallbackJob1", funcGen(55) );
        auto job2 = core::jobs::Job::New( "GenericCallbackJob2", funcGen(145) );
        auto job3 = core::jobs::Job::New( "GenericCallbackJob3", funcGen(9999) );
        auto job4 = core::jobs::Job::New( "GenericCallbackJob4", funcGen(3) );

        auto jobs1 = core::jobs::Aggregator::New( "Aggregator1" );
        auto jobs2 = core::jobs::Aggregator::New( "Aggregator2" );

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
        core::thread::Worker::sptr worker = core::thread::Worker::New();

        core::jobs::Job::Task func = [loops](core::jobs::Job& runningJob)
                                     {
                                         algoMockGenericCallback(loops,
                                                                 runningJob.progressCallback(),
                                                                 runningJob.cancelRequestedCallback());
                                     };

        auto job1 = core::jobs::Job::New( "GenericCallbackJob1", func, worker );
        auto job2 = core::jobs::Job::New( "GenericCallbackJob2", func, worker );
        auto job3 = core::jobs::Job::New( "GenericCallbackJob3", func, worker );

        loops = 1 << 30;
        func  = [loops](core::jobs::Job& runningJob)
                {
                    algoMockGenericCallback(loops, runningJob.progressCallback(), runningJob.cancelRequestedCallback());
                };
        auto job4 = core::jobs::Job::New( "GenericCallbackJob4", func, worker );
        job4->setTotalWorkUnits(std::uint64_t(loops));

        auto jobs1 = core::jobs::Aggregator::New( "Aggregator1" );
        auto jobs2 = core::jobs::Aggregator::New( "Aggregator2" );

        jobs1->add(job1);
        jobs1->add(job2);
        jobs1->add(job3);

        jobs2->add(jobs1);
        jobs2->add(job4);

        auto future = jobs2->run();
        std::this_thread::sleep_for( std::chrono::milliseconds(30) );
        jobs2->cancel();
        jobs2->wait();

        CPPUNIT_ASSERT_EQUAL(
            100 * ( (job4->getDoneWorkUnits() / job4->getTotalWorkUnits())
                    + ((job3->getDoneWorkUnits() / job3->getTotalWorkUnits())
                       + (job2->getDoneWorkUnits() / job2->getTotalWorkUnits())
                       + (job1->getDoneWorkUnits() / job1->getTotalWorkUnits()))/3 ),
            jobs2->getDoneWorkUnits());
        worker->stop();

    }

    {
        core::thread::Worker::sptr worker = core::thread::Worker::New();

        int index = -1;

        core::jobs::Job::Task func1 = [&index](core::jobs::Job&)
                                      {
                                          CPPUNIT_ASSERT_EQUAL(-1, index);
                                          index = 0;
                                      };

        core::jobs::Job::Task func2 = [&index](core::jobs::Job&)
                                      {
                                          CPPUNIT_ASSERT_EQUAL(0, index);
                                          index = 1;
                                      };

        core::jobs::Job::Task func3 = [&index](core::jobs::Job&)
                                      {
                                          CPPUNIT_ASSERT_EQUAL(1, index);
                                          index = 2;
                                      };

        auto job1 = std::make_shared< core::jobs::Job >( "GenericCallbackJob1", func1, worker );
        auto job2 = std::make_shared< core::jobs::Job >( "GenericCallbackJob2", func2, worker );
        auto job3 = std::make_shared< core::jobs::Job >( "GenericCallbackJob3", func3, worker );

        auto jobs1 = std::make_shared< core::jobs::Aggregator >( "Aggregator1" );
        jobs1->add(job1);
        jobs1->add(job1);
        jobs1->add(job2);
        jobs1->add(job3);
        jobs1->add(job3);
        jobs1->add(job2);

        CPPUNIT_ASSERT_EQUAL((decltype(jobs1->getSubJobs().size())) 3, jobs1->getSubJobs().size());

        jobs1->run().get();
        worker->stop();
    }

    { // weight test
        auto funcGen = []() -> core::jobs::Job::Task
                       {
                           return [ = ](core::jobs::Job& runningJob)
                                  {
                                      runningJob.done();
                                  };
                       };

        std::uint64_t norm = 100;
        std::uint64_t wu1 = 55, wu2 = 444, wu3 = 9999;
        double w1 = 2, w2 = 42, w3 = 0.5;

        auto job1 = core::jobs::Job::New( "GenericCallbackJob1", funcGen() );
        auto job2 = core::jobs::Observer::New( "GenericCallbackJob2", wu2 );
        auto job3 = core::jobs::Job::New( "GenericCallbackJob3", funcGen() );

        job1->setTotalWorkUnits(wu1);
        job3->setTotalWorkUnits(wu3);

        auto jobs1 = core::jobs::Aggregator::New( "Aggregator1" );
        auto jobs2 = core::jobs::Aggregator::New( "Aggregator2" );

        const double fNorm = static_cast<double>(norm);
        jobs1->add(job1, w1);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(w1*fNorm), jobs1->getTotalWorkUnits());

        jobs1->add(job2, w2);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t((w2+w1)*fNorm), jobs1->getTotalWorkUnits());

        jobs2->add(job3, w3);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(fNorm*w3), jobs2->getTotalWorkUnits());

        wu2 = 145;
        job2->setTotalWorkUnits(wu2);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(fNorm*(w2+w1)), jobs1->getTotalWorkUnits());

        double jobs1w = 2015;
        jobs2->add(jobs1, jobs1w);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(fNorm * (jobs1w+w3)), jobs2->getTotalWorkUnits());

        wu3 = 1111;
        job3->setTotalWorkUnits(wu3);
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(fNorm * (jobs1w+w3)), jobs2->getTotalWorkUnits());

        jobs2->run();
        job2->done();
        job2->finish();
        jobs2->wait();

        CPPUNIT_ASSERT_EQUAL( std::uint64_t(fNorm*(w2+w1)), jobs1->getDoneWorkUnits());
        CPPUNIT_ASSERT_EQUAL( std::uint64_t(fNorm*(jobs1w+w3)), jobs2->getDoneWorkUnits());
    }
}

//------------------------------------------------------------------------------

class ProgressObserver
{
public:
    virtual void progressNotify( double p ) = 0;
    virtual bool canceled() = 0;
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
    const std::uint64_t progress(100);
    const double fProgress = static_cast<double>(progress);
    for (int i = 0; i < 10; ++i)
    {
        int loops = 100;
        {
            core::jobs::Observer job( "GenericCallbackJob" );

            auto f = [ =, &job](double d)
                     {
                         job.doneWork(std::uint64_t(d * fProgress));
                     };
            algoMockObserver algo( new JobObserver( f ) );

            algo.run(loops);
            job.finish();

            CPPUNIT_ASSERT_EQUAL( progress, job.getDoneWorkUnits() );
        }

        {
            core::jobs::Job job( "GenericCallbackJob",
                                 [ = ](core::jobs::Job& job)
                        {
                                 auto f = [ =, &job](double d)
                            {
                                          job.doneWork(std::uint64_t(d* fProgress));
                            };
                                 algoMockObserver algo( new JobObserver( f ) );
                                 algo.run(loops);
                        });
            job.run();
            CPPUNIT_ASSERT_EQUAL( progress, job.getDoneWorkUnits() );
        }

        {
            core::thread::Worker::sptr worker = core::thread::Worker::New();

            loops = 1 << 30;
            core::jobs::Job job( "GenericCallbackJob",
                                 [ = ](core::jobs::Job& runningJob)
                        {
                                 auto f = [ =, &runningJob](double d)
                            {
                                          runningJob.doneWork(std::uint64_t(d* fProgress));
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
            job.setTotalWorkUnits(std::uint64_t(loops));
            job.run();
            std::this_thread::sleep_for( std::chrono::milliseconds(30) );
            job.cancel().wait();
            CPPUNIT_ASSERT( progress > job.getDoneWorkUnits() );
            worker->stop();
        }

        {
            core::thread::Worker::sptr worker = core::thread::Worker::New();

            loops = 1 << 30;
            core::jobs::Job job( "GenericCallbackJob",
                                 [ = ](core::jobs::Job& runningJob)
                        {
                                 auto f = [ =, &runningJob](double d)
                            {
                                          runningJob.doneWork(std::uint64_t(d* fProgress));
                            };
                                 algoMockObserver algo( new JobObserverCanceler( f, runningJob.cancelRequested() ) );
                                 algo.run(loops);
                        },
                                 worker
                                 );
            job.setTotalWorkUnits(std::uint64_t(loops));
            job.run();
            std::this_thread::sleep_for( std::chrono::milliseconds(30) );
            job.cancel().wait();
            CPPUNIT_ASSERT( progress > job.getDoneWorkUnits() );
            worker->stop();
        }
    }
}

//------------------------------------------------------------------------------

void JobTest::LogTest()
{
    int loops = 100;
    {
        core::jobs::Observer job( "GenericCallbackJob1" );

        job.log("Test of GenericCallbackJob1");
        algoMockGenericCallback(loops, job.progressCallback(), job.cancelRequestedCallback(),
                                [&job](const std::string& message)
                {
                    job.log(message);
                });

        job.finish();
    }

    {
        core::jobs::Job::Task func = [loops](core::jobs::Job& runningJob)
                                     {
                                         algoMockGenericCallback(loops,
                                                                 runningJob.progressCallback(),
                                                                 runningJob.cancelRequestedCallback(),
                                                                 [&runningJob](const std::string message)
                    {
                        runningJob.log(message);
                    });
                                     };
        core::jobs::Job job( "GenericCallbackJob2", func);

        job.log("Test of GenericCallbackJob2");

        job.run();
    }

    {
        core::jobs::Job::Task func = [loops](core::jobs::Job& runningJob)
                                     {
                                         algoMockGenericCallback(loops,
                                                                 runningJob.progressCallback(),
                                                                 runningJob.cancelRequestedCallback(),
                                                                 [&runningJob](const std::string message)
                    {
                        runningJob.log(message);
                    });
                                     };
        auto job1 = core::jobs::Job::New( "GenericCallbackJob1", func );
        auto job2 = core::jobs::Job::New( "GenericCallbackJob2", func );
        auto job3 = core::jobs::Job::New( "GenericCallbackJob3", func );
        auto job4 = core::jobs::Job::New( "GenericCallbackJob4", func );

        auto jobs1 = core::jobs::Aggregator::New( "Aggregator1" );
        auto jobs2 = core::jobs::Aggregator::New( "Aggregator2" );

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
} //namespace sight::core::jobs
