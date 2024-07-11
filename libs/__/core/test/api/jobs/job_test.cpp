/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "job_test.hpp"

#include <core/jobs/aggregator.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/exception/waiting.hpp>
#include <core/jobs/job.hpp>
#include <core/jobs/observer.hpp>
#include <core/spy_log.hpp>
#include <core/thread/worker.hpp>

#include <exception>
#include <functional>
#include <future>
#include <string>
#include <thread>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::core::jobs::ut::job_test);

namespace sight::core::jobs::ut
{

//------------------------------------------------------------------------------

void job_test::setUp()
{
}

//------------------------------------------------------------------------------

void job_test::tearDown()
{
}

//------------------------------------------------------------------------------

void algo_mock_generic_callback(
    int _n,
    std::function<void(int)> _progress,
    std::function<bool()> _should_cancel,
    std::function<void(const std::string&)> _log = nullptr
)
{
    int div = _n / 15;

    for(int i = 0 ; i < _n ; ++i)
    {
        // algo ...

        if(_should_cancel())
        {
            break;
        }

        if(_log && i % div == 0)
        {
            _log("algoMockGenericCallback step " + std::to_string(i));
        }

        _progress(i + 1);
    }
}

//------------------------------------------------------------------------------

void job_test::apiand_state_test()
{
    {
        core::jobs::job job("Job", [](core::jobs::job&)
                {
                });
        CPPUNIT_ASSERT_EQUAL(core::jobs::base::waiting, job.get_state());

        CPPUNIT_ASSERT_THROW(job.wait(), core::jobs::exception::waiting);

        CPPUNIT_ASSERT_EQUAL(false, job.cancel_requested());
        CPPUNIT_ASSERT_EQUAL(false, job.cancel_requested_callback()());

                             job.cancel();
        CPPUNIT_ASSERT_EQUAL(core::jobs::base::canceled, job.get_state());

        CPPUNIT_ASSERT_EQUAL(true, job.cancel_requested());
        CPPUNIT_ASSERT_EQUAL(true, job.cancel_requested_callback()());

                             CPPUNIT_ASSERT_NO_THROW(job.wait());

        CPPUNIT_ASSERT(job.run().valid());
    }

    {
        core::jobs::job job("Job", [](core::jobs::job& _running_job)
                {
                            CPPUNIT_ASSERT_EQUAL(core::jobs::base::running, _running_job.get_state());
                });
        CPPUNIT_ASSERT_EQUAL(core::jobs::base::waiting, job.get_state());

        CPPUNIT_ASSERT_THROW(job.wait(), core::jobs::exception::waiting);

        job.run();
        CPPUNIT_ASSERT_EQUAL(core::jobs::base::finished, job.get_state());

        job.cancel();
        CPPUNIT_ASSERT_EQUAL(core::jobs::base::finished, job.get_state());
        CPPUNIT_ASSERT_NO_THROW(job.wait());
    }

    {
        core::thread::worker::sptr worker = core::thread::worker::make();
        core::jobs::job job("Job", [](core::jobs::job& _running_job)
                {
                            std::this_thread::sleep_for(std::chrono::milliseconds(30));
                            CPPUNIT_ASSERT_EQUAL(core::jobs::base::canceling, _running_job.get_state());
                }, worker);
        CPPUNIT_ASSERT_EQUAL(core::jobs::base::waiting, job.get_state());

        CPPUNIT_ASSERT_THROW(job.wait(), core::jobs::exception::waiting);

        job.run();
        job.cancel();
        CPPUNIT_ASSERT_EQUAL(core::jobs::base::canceling, job.get_state());

        job.wait();

        CPPUNIT_ASSERT_EQUAL(core::jobs::base::canceled, job.get_state());
        CPPUNIT_ASSERT_NO_THROW(job.wait());
        worker->stop();
    }

    {
        core::jobs::observer job("Observer");
        CPPUNIT_ASSERT_EQUAL(core::jobs::base::running, job.get_state());

        auto future = std::async(
            [&job]() -> bool
            {
                bool except = true;
                try
                {
                    job.wait();
                    except = false;
                }
                catch(core::jobs::exception::waiting&)
                {
                    except = true;
                }
                return except;
            });

        CPPUNIT_ASSERT_EQUAL(false, job.cancel_requested());
        CPPUNIT_ASSERT_EQUAL(false, job.cancel_requested_callback()());

                             job.cancel();
        CPPUNIT_ASSERT_EQUAL(core::jobs::base::canceled, job.get_state());
        job.finish();
        CPPUNIT_ASSERT_EQUAL(core::jobs::base::canceled, job.get_state());

        CPPUNIT_ASSERT_EQUAL(true, job.cancel_requested());
        CPPUNIT_ASSERT_EQUAL(true, job.cancel_requested_callback()());

                             CPPUNIT_ASSERT_NO_THROW(job.wait());

        CPPUNIT_ASSERT_EQUAL(false, future.get());
    }

    {
        core::jobs::observer job("Observer");
        CPPUNIT_ASSERT_EQUAL(core::jobs::base::running, job.get_state());

        job.finish();
        CPPUNIT_ASSERT_EQUAL(core::jobs::base::finished, job.get_state());
        job.cancel();
        CPPUNIT_ASSERT_EQUAL(core::jobs::base::finished, job.get_state());

        CPPUNIT_ASSERT_EQUAL(false, job.cancel_requested());
        CPPUNIT_ASSERT_EQUAL(false, job.cancel_requested_callback()());

                             CPPUNIT_ASSERT_NO_THROW(job.wait());
    }
}

//------------------------------------------------------------------------------

void job_test::generic_callback_test()
{
    for(int i = 0 ; i < 10 ; ++i)
    {
        int loops = 100;
        {
            core::jobs::observer job("GenericCallbackJob");

            CPPUNIT_ASSERT_EQUAL(core::jobs::base::running, job.get_state());

            algo_mock_generic_callback(loops, job.progress_callback(), job.cancel_requested_callback());

            CPPUNIT_ASSERT_EQUAL(core::jobs::base::running, job.get_state());
            job.finish();

            CPPUNIT_ASSERT_EQUAL(std::uint64_t(loops), job.get_done_work_units());

            CPPUNIT_ASSERT_EQUAL(core::jobs::base::finished, job.get_state());
        }

        {
            core::jobs::job::task func = [loops](core::jobs::job& _running_job)
                                         {
                                             algo_mock_generic_callback(
                                                 loops,
                                                 _running_job.progress_callback(),
                                                 _running_job.cancel_requested_callback()
                                             );
                                         };
            core::jobs::job job("GenericCallbackJob", func);

            CPPUNIT_ASSERT_EQUAL(core::jobs::base::waiting, job.get_state());

            job.run();
            CPPUNIT_ASSERT_EQUAL(std::uint64_t(loops), job.get_done_work_units());
            CPPUNIT_ASSERT_EQUAL(core::jobs::base::finished, job.get_state());
        }

        {
            core::thread::worker::sptr worker = core::thread::worker::make();

            loops = 1 << 30;
            core::jobs::job job("GenericCallbackJob",
                                [loops](core::jobs::job& _running_job)
                    {
                                algo_mock_generic_callback(
                                    loops,
                                    _running_job.progress_callback(),
                                    _running_job.cancel_requested_callback()
                                );
                    },
                                worker);
            job.set_total_work_units(std::uint64_t(loops));
            job.run();
            CPPUNIT_ASSERT_EQUAL(core::jobs::base::running, job.get_state());
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            job.cancel();
            job.wait();
            CPPUNIT_ASSERT(
                core::jobs::base::canceling == job.get_state()
                || core::jobs::base::canceled == job.get_state()
            );

            CPPUNIT_ASSERT(static_cast<std::uint64_t>(loops) > job.get_done_work_units());
            worker->stop();
        }
    }
}

//------------------------------------------------------------------------------

void job_test::aggregation_test()
{
    int loops = 100;

    {
        // Job aggregation test
        core::jobs::job::task func = [loops](core::jobs::job& _running_job)
                                     {
                                         algo_mock_generic_callback(
                                             loops,
                                             _running_job.progress_callback(),
                                             _running_job.cancel_requested_callback()
                                         );
                                     };
        auto job1 = std::make_shared<core::jobs::job>("GenericCallbackJob1", func);
        auto job2 = std::make_shared<core::jobs::job>("GenericCallbackJob2", func);
        auto job3 = std::make_shared<core::jobs::job>("GenericCallbackJob3", func);

        auto jobs1 = std::make_shared<core::jobs::aggregator>("Aggregator1");

        jobs1->add(job1);
        jobs1->add(job2);
        jobs1->add(job3);

        CPPUNIT_ASSERT_EQUAL(std::uint64_t(0), jobs1->get_done_work_units());

        jobs1->run();
        jobs1->wait();

        CPPUNIT_ASSERT_EQUAL(std::uint64_t(300), jobs1->get_total_work_units());
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(300), jobs1->get_done_work_units());
    }

    {
        // Job and aggregator aggregation test
        core::jobs::job::task func = [loops](core::jobs::job& _running_job)
                                     {
                                         algo_mock_generic_callback(
                                             loops,
                                             _running_job.progress_callback(),
                                             _running_job.cancel_requested_callback()
                                         );
                                     };
        auto job1 = std::make_shared<core::jobs::job>("GenericCallbackJob1", func);
        auto job2 = std::make_shared<core::jobs::job>("GenericCallbackJob2", func);
        auto job3 = std::make_shared<core::jobs::job>("GenericCallbackJob3", func);
        auto job4 = std::make_shared<core::jobs::job>("GenericCallbackJob4", func);

        auto jobs1 = std::make_shared<core::jobs::aggregator>("Aggregator1");
        auto jobs2 = std::make_shared<core::jobs::aggregator>("Aggregator2");

        jobs1->add(job1);
        jobs1->add(job2);
        jobs1->add(job3);

        jobs2->add(jobs1);
        jobs2->add(job4);

        jobs2->run();
        jobs2->wait();

        CPPUNIT_ASSERT_EQUAL(std::uint64_t(100), job1->get_done_work_units());

        CPPUNIT_ASSERT_EQUAL(std::uint64_t(200), jobs2->get_total_work_units());
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(200), jobs2->get_done_work_units());
    }

    {
        // total work units update test
        auto func_gen = [](int _progress_steps) -> core::jobs::job::task
                        {
                            return [ = ](core::jobs::job& _running_job)
                                   {
                                       algo_mock_generic_callback(
                                           _progress_steps,
                                           _running_job.progress_callback(),
                                           _running_job.cancel_requested_callback(),
                                           nullptr
                                       );
                                   };
                        };

        auto job1 = std::make_shared<core::jobs::job>("GenericCallbackJob1", func_gen(55));
        auto job2 = std::make_shared<core::jobs::job>("GenericCallbackJob2", func_gen(145));
        auto job3 = std::make_shared<core::jobs::job>("GenericCallbackJob3", func_gen(9999));
        auto job4 = std::make_shared<core::jobs::job>("GenericCallbackJob4", func_gen(3));

        auto jobs1 = std::make_shared<core::jobs::aggregator>("Aggregator1");
        auto jobs2 = std::make_shared<core::jobs::aggregator>("Aggregator2");

        CPPUNIT_ASSERT_EQUAL(std::uint64_t(0), jobs1->get_total_work_units());
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(0), jobs2->get_total_work_units());

        job1->set_total_work_units(55);

        jobs1->add(job1);
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(100), jobs1->get_total_work_units());

        jobs1->add(job2);
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(200), jobs1->get_total_work_units());

        jobs1->add(job3);
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(300), jobs1->get_total_work_units());

        job2->set_total_work_units(145);
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(300), jobs1->get_total_work_units());

        job2->set_total_work_units(0);
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(200), jobs1->get_total_work_units());

        job2->set_total_work_units(100);
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(300), jobs1->get_total_work_units());

        // set back to 145 to be consistent with job's task
        job2->set_total_work_units(145);
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(300), jobs1->get_total_work_units());

        jobs2->add(jobs1);
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(100), jobs2->get_total_work_units());

        jobs2->add(job4);
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(200), jobs2->get_total_work_units());

        job3->set_total_work_units(9999);
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(200), jobs2->get_total_work_units());

        job4->set_total_work_units(3);
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(200), jobs2->get_total_work_units());

        jobs2->run();
        jobs2->wait();

        CPPUNIT_ASSERT_EQUAL(std::uint64_t(200), jobs2->get_total_work_units());

        CPPUNIT_ASSERT_EQUAL(std::uint64_t(300), jobs1->get_done_work_units());

        CPPUNIT_ASSERT_EQUAL(std::uint64_t(200), jobs2->get_done_work_units());
    }

    {
        core::thread::worker::sptr worker = core::thread::worker::make();

        core::jobs::job::task func = [loops](core::jobs::job& _running_job)
                                     {
                                         algo_mock_generic_callback(
                                             loops,
                                             _running_job.progress_callback(),
                                             _running_job.cancel_requested_callback()
                                         );
                                     };

        auto job1 = std::make_shared<core::jobs::job>("GenericCallbackJob1", func, worker);
        auto job2 = std::make_shared<core::jobs::job>("GenericCallbackJob2", func, worker);
        auto job3 = std::make_shared<core::jobs::job>("GenericCallbackJob3", func, worker);

        loops = 1 << 30;
        func  = [loops](core::jobs::job& _running_job)
                {
                    algo_mock_generic_callback(
                        loops,
                        _running_job.progress_callback(),
                        _running_job.cancel_requested_callback()
                    );
                };
        auto job4 = std::make_shared<core::jobs::job>("GenericCallbackJob4", func, worker);
        job4->set_total_work_units(std::uint64_t(loops));

        auto jobs1 = std::make_shared<core::jobs::aggregator>("Aggregator1");
        auto jobs2 = std::make_shared<core::jobs::aggregator>("Aggregator2");

        jobs1->add(job1);
        jobs1->add(job2);
        jobs1->add(job3);

        jobs2->add(jobs1);
        jobs2->add(job4);

        auto future = jobs2->run();
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        jobs2->cancel();
        jobs2->wait();

        CPPUNIT_ASSERT_EQUAL(
            100 * ((job4->get_done_work_units() / job4->get_total_work_units())
                   + ((job3->get_done_work_units() / job3->get_total_work_units())
                      + (job2->get_done_work_units() / job2->get_total_work_units())
                      + (job1->get_done_work_units() / job1->get_total_work_units())) / 3),
            jobs2->get_done_work_units()
        );
        worker->stop();
    }

    {
        core::thread::worker::sptr worker = core::thread::worker::make();

        int index = -1;

        core::jobs::job::task func1 = [&index](core::jobs::job&)
                                      {
                                          CPPUNIT_ASSERT_EQUAL(-1, index);
                                          index = 0;
                                      };

        core::jobs::job::task func2 = [&index](core::jobs::job&)
                                      {
                                          CPPUNIT_ASSERT_EQUAL(0, index);
                                          index = 1;
                                      };

        core::jobs::job::task func3 = [&index](core::jobs::job&)
                                      {
                                          CPPUNIT_ASSERT_EQUAL(1, index);
                                          index = 2;
                                      };

        auto job1 = std::make_shared<core::jobs::job>("GenericCallbackJob1", func1, worker);
        auto job2 = std::make_shared<core::jobs::job>("GenericCallbackJob2", func2, worker);
        auto job3 = std::make_shared<core::jobs::job>("GenericCallbackJob3", func3, worker);

        auto jobs1 = std::make_shared<core::jobs::aggregator>("Aggregator1");
        jobs1->add(job1);
        jobs1->add(job1);
        jobs1->add(job2);
        jobs1->add(job3);
        jobs1->add(job3);
        jobs1->add(job2);

        CPPUNIT_ASSERT_EQUAL((decltype(jobs1->get_sub_jobs().size())) 3, jobs1->get_sub_jobs().size());

        jobs1->run().get();
        worker->stop();
    }

    {
        // weight test
        auto func_gen = []() -> core::jobs::job::task
                        {
                            return [ = ](core::jobs::job& _running_job)
                                   {
                                       _running_job.done();
                                   };
                        };

        std::uint64_t norm = 100;
        std::uint64_t wu1  = 55;
        std::uint64_t wu2  = 444;
        std::uint64_t wu3  = 9999;
        double w1          = 2;
        double w2          = 42;
        double w3          = 0.5;

        auto job1 = std::make_shared<core::jobs::job>("GenericCallbackJob1", func_gen());
        auto job2 = std::make_shared<core::jobs::observer>("GenericCallbackJob2", wu2);
        auto job3 = std::make_shared<core::jobs::job>("GenericCallbackJob3", func_gen());

        job1->set_total_work_units(wu1);
        job3->set_total_work_units(wu3);

        auto jobs1 = std::make_shared<core::jobs::aggregator>("Aggregator1");
        auto jobs2 = std::make_shared<core::jobs::aggregator>("Aggregator2");

        const auto f_norm = static_cast<double>(norm);
        jobs1->add(job1, w1);
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(w1 * f_norm), jobs1->get_total_work_units());

        jobs1->add(job2, w2);
        CPPUNIT_ASSERT_EQUAL(std::uint64_t((w2 + w1) * f_norm), jobs1->get_total_work_units());

        jobs2->add(job3, w3);
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(f_norm * w3), jobs2->get_total_work_units());

        wu2 = 145;
        job2->set_total_work_units(wu2);
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(f_norm * (w2 + w1)), jobs1->get_total_work_units());

        double jobs1w = 2015;
        jobs2->add(jobs1, jobs1w);
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(f_norm * (jobs1w + w3)), jobs2->get_total_work_units());

        wu3 = 1111;
        job3->set_total_work_units(wu3);
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(f_norm * (jobs1w + w3)), jobs2->get_total_work_units());

        jobs2->run();
        job2->done();
        job2->finish();
        jobs2->wait();

        CPPUNIT_ASSERT_EQUAL(std::uint64_t(f_norm * (w2 + w1)), jobs1->get_done_work_units());
        CPPUNIT_ASSERT_EQUAL(std::uint64_t(f_norm * (jobs1w + w3)), jobs2->get_done_work_units());
    }
}

//------------------------------------------------------------------------------

class progress_observer
{
public:

    virtual void progress_notify(double _p) = 0;
    virtual bool canceled()                 = 0;
    virtual ~progress_observer()
    = default;
};

class algo_mock_observer
{
public:

    explicit algo_mock_observer(progress_observer* _obs) :
        m_obs(_obs)
    {
    }

    ~algo_mock_observer()
    {
        delete m_obs;
    }

    //------------------------------------------------------------------------------

    void run(int _n)
    {
        for(int i = 0 ; i < _n ; i++)
        {
            // algo ...
            if(m_obs != nullptr)
            {
                m_obs->progress_notify(((double) (i + 1)) / _n);

                if(m_obs->canceled())
                {
                    break;
                }
            }

            if(m_canceled)
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

private:

    progress_observer* m_obs;
    std::atomic_bool m_canceled {false};
};

struct job_observer : public progress_observer
{
    explicit job_observer(std::function<void(double)> _func) :
        m_callback(std::move(_func))
    {
    }

    //------------------------------------------------------------------------------

    void progress_notify(double _p) override
    {
        m_callback(_p);
    }

    //------------------------------------------------------------------------------

    bool canceled() override
    {
        return false;
    }

    std::function<void(double)> m_callback;
};

struct job_observer_canceler : public job_observer
{
    explicit job_observer_canceler(
        std::function<void(double)> _func,
        base::cancel_request_callback _canceled_callback
    ) :
        job_observer(std::move(_func)),
        m_canceled_callback(std::move(_canceled_callback))
    {
    }

    //------------------------------------------------------------------------------

    bool canceled() override
    {
        return m_canceled_callback();
    }

    const base::cancel_request_callback m_canceled_callback;
};

//------------------------------------------------------------------------------

void job_test::observer_test()
{
    const std::uint64_t progress(100);
    const auto f_progress = static_cast<double>(progress);
    for(int i = 0 ; i < 10 ; ++i)
    {
        int loops = 100;
        {
            core::jobs::observer job("GenericCallbackJob");

            auto f = [ =, &job](double _d)
                     {
                         job.done_work(std::uint64_t(_d * f_progress));
                     };
            algo_mock_observer algo(new job_observer(f));

            algo.run(loops);
            job.finish();

            CPPUNIT_ASSERT_EQUAL(progress, job.get_done_work_units());
        }

        {
            core::jobs::job job("GenericCallbackJob",
                                [ = ](core::jobs::job& _job)
                    {
                                auto f = [ =, &_job](double _d)
                        {
                                         _job.done_work(std::uint64_t(_d * f_progress));
                        };
                                algo_mock_observer algo(new job_observer(f));
                                algo.run(loops);
                    });
            job.run();
            CPPUNIT_ASSERT_EQUAL(progress, job.get_done_work_units());
        }

        {
            core::thread::worker::sptr worker = core::thread::worker::make();

            loops = 1 << 30;
            core::jobs::job job("GenericCallbackJob",
                                [ = ](core::jobs::job& _running_job)
                    {
                                auto f = [ =, &_running_job](double _d)
                        {
                                         _running_job.done_work(std::uint64_t(_d * f_progress));
                        };
                                algo_mock_observer algo(new job_observer(f));
                                _running_job.add_simple_cancel_hook(
                                    [&]()
                        {
                                    algo.cancel();
                        });
                                algo.run(loops);
                    },
                                worker
            );
            job.set_total_work_units(std::uint64_t(loops));
            job.run();
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            job.cancel().wait();
            CPPUNIT_ASSERT(progress > job.get_done_work_units());
            worker->stop();
        }

        {
            core::thread::worker::sptr worker = core::thread::worker::make();

            loops = 1 << 30;
            core::jobs::job job(
                "GenericCallbackJob",
                [ = ](core::jobs::job& _running_job)
                    {
                auto f =
                    [ =, &_running_job](double _d)
                        {
                    _running_job.done_work(std::uint64_t(_d * f_progress));
                        };

                algo_mock_observer algo(new job_observer_canceler(f, _running_job.cancel_requested_callback()));
                algo.run(loops);
                    },
                worker
            );

            job.set_total_work_units(std::uint64_t(loops));
            job.run();
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            job.cancel().wait();
            CPPUNIT_ASSERT(progress > job.get_done_work_units());
            worker->stop();
        }
    }
}

//------------------------------------------------------------------------------

void job_test::log_test()
{
    int loops = 100;
    {
        core::jobs::observer job("GenericCallbackJob1");

        job.log("Test of GenericCallbackJob1");
        algo_mock_generic_callback(
            loops,
            job.progress_callback(),
            job.cancel_requested_callback(),
            [&job](const std::string& _message)
            {
                job.log(_message);
            });

        job.finish();
    }

    {
        core::jobs::job::task func = [loops](core::jobs::job& _running_job)
                                     {
                                         algo_mock_generic_callback(
                                             loops,
                                             _running_job.progress_callback(),
                                             _running_job.cancel_requested_callback(),
                                             [&_running_job](const std::string _message)
                {
                    _running_job.log(_message);
                });
                                     };
        core::jobs::job job("GenericCallbackJob2", func);

        job.log("Test of GenericCallbackJob2");

        job.run();
    }

    {
        core::jobs::job::task func = [loops](core::jobs::job& _running_job)
                                     {
                                         algo_mock_generic_callback(
                                             loops,
                                             _running_job.progress_callback(),
                                             _running_job.cancel_requested_callback(),
                                             [&_running_job](const std::string _message)
                {
                    _running_job.log(_message);
                });
                                     };
        auto job1 = std::make_shared<core::jobs::job>("GenericCallbackJob1", func);
        auto job2 = std::make_shared<core::jobs::job>("GenericCallbackJob2", func);
        auto job3 = std::make_shared<core::jobs::job>("GenericCallbackJob3", func);
        auto job4 = std::make_shared<core::jobs::job>("GenericCallbackJob4", func);

        auto jobs1 = std::make_shared<core::jobs::aggregator>("Aggregator1");
        auto jobs2 = std::make_shared<core::jobs::aggregator>("Aggregator2");

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

        auto nb_logs = 18;
        CPPUNIT_ASSERT_EQUAL(std::size_t(nb_logs), job1->get_logs().size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(nb_logs), job2->get_logs().size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(nb_logs), job3->get_logs().size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(nb_logs), job4->get_logs().size());
        CPPUNIT_ASSERT_EQUAL(std::size_t(3 * nb_logs + 1), jobs1->get_logs().size());
        CPPUNIT_ASSERT_EQUAL(std::size_t((3 * nb_logs + 1) + nb_logs + 1), jobs2->get_logs().size());
    }
}

} // namespace sight::core::jobs::ut
