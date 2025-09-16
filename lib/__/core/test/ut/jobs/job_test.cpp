/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <core/jobs/aggregator.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/exception/waiting.hpp>
#include <core/jobs/job.hpp>
#include <core/jobs/observer.hpp>
#include <core/spy_log.hpp>
#include <core/thread/worker.hpp>

#include <doctest/doctest.h>

#include <exception>
#include <functional>
#include <future>
#include <string>
#include <thread>

TEST_SUITE("sight::core::jobs")
{
//------------------------------------------------------------------------------

    static void algo_mock_generic_callback(
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

    TEST_CASE("apiand_state")
    {
        {
            sight::core::jobs::job job("Job", [](sight::core::jobs::job&)
                {
                });
            CHECK_EQ(job.get_state(), sight::core::jobs::base::waiting);

            CHECK_THROWS_AS(job.wait(), sight::core::jobs::exception::waiting);

            CHECK_EQ(job.cancel_requested(), false);
            CHECK_EQ(job.cancel_requested_callback()(), false);

                     job.cancel();
            CHECK_EQ(job.get_state(), sight::core::jobs::base::canceled);

            CHECK_EQ(job.cancel_requested(), true);
            CHECK_EQ(job.cancel_requested_callback()(), true);

                     CHECK_NOTHROW(job.wait());

            CHECK(job.run().valid());
        }

        {
            sight::core::jobs::job job("Job", [](sight::core::jobs::job& _running_job)
                {
                                       CHECK_EQ(_running_job.get_state(), sight::core::jobs::base::running);
                });
            CHECK_EQ(job.get_state(), sight::core::jobs::base::waiting);

            CHECK_THROWS_AS(job.wait(), sight::core::jobs::exception::waiting);

            job.run();
            CHECK_EQ(job.get_state(), sight::core::jobs::base::finished);

            job.cancel();
            CHECK_EQ(job.get_state(), sight::core::jobs::base::finished);
            CHECK_NOTHROW(job.wait());
        }

        {
            auto worker = sight::core::thread::worker::make();
            sight::core::jobs::job job("Job", [](sight::core::jobs::job& _running_job)
                {
                                       std::this_thread::sleep_for(std::chrono::milliseconds(30));
                                       CHECK_EQ(_running_job.get_state(), sight::core::jobs::base::canceling);
                }, worker);
            CHECK_EQ(job.get_state(), sight::core::jobs::base::waiting);

            CHECK_THROWS_AS(job.wait(), sight::core::jobs::exception::waiting);

            job.run();
            job.cancel();
            CHECK_EQ(job.get_state(), sight::core::jobs::base::canceling);

            job.wait();

            CHECK_EQ(job.get_state(), sight::core::jobs::base::canceled);
            CHECK_NOTHROW(job.wait());
            worker->stop();
        }

        {
            sight::core::jobs::observer job("Observer");
            CHECK_EQ(job.get_state(), sight::core::jobs::base::running);

            auto future = std::async(
                [&job]() -> bool
            {
                bool except = true;
                try
                {
                    job.wait();
                    except = false;
                }
                catch(sight::core::jobs::exception::waiting&)
                {
                    except = true;
                }
                return except;
            });

            CHECK_EQ(job.cancel_requested(), false);
            CHECK_EQ(job.cancel_requested_callback()(), false);

                     job.cancel();
            CHECK_EQ(job.get_state(), sight::core::jobs::base::canceled);
            job.finish();
            CHECK_EQ(job.get_state(), sight::core::jobs::base::canceled);

            CHECK_EQ(job.cancel_requested(), true);
            CHECK_EQ(job.cancel_requested_callback()(), true);

                     CHECK_NOTHROW(job.wait());

            CHECK_EQ(future.get(), false);
        }

        {
            sight::core::jobs::observer job("Observer");
            CHECK_EQ(job.get_state(), sight::core::jobs::base::running);

            job.finish();
            CHECK_EQ(job.get_state(), sight::core::jobs::base::finished);
            job.cancel();
            CHECK_EQ(job.get_state(), sight::core::jobs::base::finished);

            CHECK_EQ(job.cancel_requested(), false);
            CHECK_EQ(job.cancel_requested_callback()(), false);

                     CHECK_NOTHROW(job.wait());
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("generic_callback")
    {
        for(int i = 0 ; i < 10 ; ++i)
        {
            int loops = 100;
            {
                sight::core::jobs::observer job("GenericCallbackJob");

                CHECK_EQ(job.get_state(), sight::core::jobs::base::running);

                algo_mock_generic_callback(loops, job.progress_callback(), job.cancel_requested_callback());

                CHECK_EQ(job.get_state(), sight::core::jobs::base::running);
                job.finish();

                CHECK_EQ(job.get_done_work_units(), std::uint64_t(loops));

                CHECK_EQ(job.get_state(), sight::core::jobs::base::finished);
            }

            {
                sight::core::jobs::job::task func = [loops](sight::core::jobs::job& _running_job)
                                                    {
                                                        algo_mock_generic_callback(
                                                            loops,
                                                            _running_job.progress_callback(),
                                                            _running_job.cancel_requested_callback()
                                                        );
                                                    };
                sight::core::jobs::job job("GenericCallbackJob", func);

                CHECK_EQ(job.get_state(), sight::core::jobs::base::waiting);

                job.run();
                CHECK_EQ(job.get_done_work_units(), std::uint64_t(loops));
                CHECK_EQ(job.get_state(), sight::core::jobs::base::finished);
            }

            {
                auto worker = sight::core::thread::worker::make();

                loops = 1 << 30;
                sight::core::jobs::job job("GenericCallbackJob",
                                           [loops](sight::core::jobs::job& _running_job)
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
                CHECK_EQ(job.get_state(), sight::core::jobs::base::running);
                std::this_thread::sleep_for(std::chrono::milliseconds(30));
                job.cancel();
                job.wait();
                const bool cancel_state = (sight::core::jobs::base::canceling == job.get_state()
                                           || sight::core::jobs::base::canceled == job.get_state());
                CHECK(cancel_state);

                CHECK(static_cast<std::uint64_t>(loops) > job.get_done_work_units());
                worker->stop();
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("aggregation")
    {
        int loops = 100;

        {
            // Job aggregation test
            sight::core::jobs::job::task func = [loops](sight::core::jobs::job& _running_job)
                                                {
                                                    algo_mock_generic_callback(
                                                        loops,
                                                        _running_job.progress_callback(),
                                                        _running_job.cancel_requested_callback()
                                                    );
                                                };
            auto job1 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob1", func);
            auto job2 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob2", func);
            auto job3 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob3", func);

            auto jobs1 = std::make_shared<sight::core::jobs::aggregator>("Aggregator1");

            jobs1->add(job1);
            jobs1->add(job2);
            jobs1->add(job3);

            CHECK_EQ(jobs1->get_done_work_units(), std::uint64_t(0));

            jobs1->run();
            jobs1->wait();

            CHECK_EQ(jobs1->get_total_work_units(), std::uint64_t(300));
            CHECK_EQ(jobs1->get_done_work_units(), std::uint64_t(300));
        }

        {
            // Job and aggregator aggregation test
            sight::core::jobs::job::task func = [loops](sight::core::jobs::job& _running_job)
                                                {
                                                    algo_mock_generic_callback(
                                                        loops,
                                                        _running_job.progress_callback(),
                                                        _running_job.cancel_requested_callback()
                                                    );
                                                };
            auto job1 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob1", func);
            auto job2 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob2", func);
            auto job3 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob3", func);
            auto job4 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob4", func);

            auto jobs1 = std::make_shared<sight::core::jobs::aggregator>("Aggregator1");
            auto jobs2 = std::make_shared<sight::core::jobs::aggregator>("Aggregator2");

            jobs1->add(job1);
            jobs1->add(job2);
            jobs1->add(job3);

            jobs2->add(jobs1);
            jobs2->add(job4);

            jobs2->run();
            jobs2->wait();

            CHECK_EQ(job1->get_done_work_units(), std::uint64_t(100));

            CHECK_EQ(jobs2->get_total_work_units(), std::uint64_t(200));
            CHECK_EQ(jobs2->get_done_work_units(), std::uint64_t(200));
        }

        {
            // total work units update test
            auto func_gen = [](int _progress_steps) -> sight::core::jobs::job::task
                            {
                                return [ = ](sight::core::jobs::job& _running_job)
                                       {
                                           algo_mock_generic_callback(
                                               _progress_steps,
                                               _running_job.progress_callback(),
                                               _running_job.cancel_requested_callback(),
                                               nullptr
                                           );
                                       };
                            };

            auto job1 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob1", func_gen(55));
            auto job2 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob2", func_gen(145));
            auto job3 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob3", func_gen(9999));
            auto job4 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob4", func_gen(3));

            auto jobs1 = std::make_shared<sight::core::jobs::aggregator>("Aggregator1");
            auto jobs2 = std::make_shared<sight::core::jobs::aggregator>("Aggregator2");

            CHECK_EQ(jobs1->get_total_work_units(), std::uint64_t(0));
            CHECK_EQ(jobs2->get_total_work_units(), std::uint64_t(0));

            job1->set_total_work_units(55);

            jobs1->add(job1);
            CHECK_EQ(jobs1->get_total_work_units(), std::uint64_t(100));

            jobs1->add(job2);
            CHECK_EQ(jobs1->get_total_work_units(), std::uint64_t(200));

            jobs1->add(job3);
            CHECK_EQ(jobs1->get_total_work_units(), std::uint64_t(300));

            job2->set_total_work_units(145);
            CHECK_EQ(jobs1->get_total_work_units(), std::uint64_t(300));

            job2->set_total_work_units(0);
            CHECK_EQ(jobs1->get_total_work_units(), std::uint64_t(200));

            job2->set_total_work_units(100);
            CHECK_EQ(jobs1->get_total_work_units(), std::uint64_t(300));

            // set back to 145 to be consistent with job's task
            job2->set_total_work_units(145);
            CHECK_EQ(jobs1->get_total_work_units(), std::uint64_t(300));

            jobs2->add(jobs1);
            CHECK_EQ(jobs2->get_total_work_units(), std::uint64_t(100));

            jobs2->add(job4);
            CHECK_EQ(jobs2->get_total_work_units(), std::uint64_t(200));

            job3->set_total_work_units(9999);
            CHECK_EQ(jobs2->get_total_work_units(), std::uint64_t(200));

            job4->set_total_work_units(3);
            CHECK_EQ(jobs2->get_total_work_units(), std::uint64_t(200));

            jobs2->run();
            jobs2->wait();

            CHECK_EQ(jobs2->get_total_work_units(), std::uint64_t(200));

            CHECK_EQ(jobs1->get_done_work_units(), std::uint64_t(300));

            CHECK_EQ(jobs2->get_done_work_units(), std::uint64_t(200));
        }

        {
            auto worker = sight::core::thread::worker::make();

            sight::core::jobs::job::task func = [loops](sight::core::jobs::job& _running_job)
                                                {
                                                    algo_mock_generic_callback(
                                                        loops,
                                                        _running_job.progress_callback(),
                                                        _running_job.cancel_requested_callback()
                                                    );
                                                };

            auto job1 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob1", func, worker);
            auto job2 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob2", func, worker);
            auto job3 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob3", func, worker);

            loops = 1 << 30;
            func  = [loops](sight::core::jobs::job& _running_job)
                    {
                        algo_mock_generic_callback(
                            loops,
                            _running_job.progress_callback(),
                            _running_job.cancel_requested_callback()
                        );
                    };
            auto job4 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob4", func, worker);
            job4->set_total_work_units(std::uint64_t(loops));

            auto jobs1 = std::make_shared<sight::core::jobs::aggregator>("Aggregator1");
            auto jobs2 = std::make_shared<sight::core::jobs::aggregator>("Aggregator2");

            jobs1->add(job1);
            jobs1->add(job2);
            jobs1->add(job3);

            jobs2->add(jobs1);
            jobs2->add(job4);

            auto future = jobs2->run();
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            jobs2->cancel();
            jobs2->wait();

            CHECK_EQ(
                jobs2->get_done_work_units(),
                100 * ((job4->get_done_work_units() / job4->get_total_work_units())
                       + ((job3->get_done_work_units() / job3->get_total_work_units())
                          + (job2->get_done_work_units() / job2->get_total_work_units())
                          + (job1->get_done_work_units() / job1->get_total_work_units())) / 3)
            );
            worker->stop();
        }

        {
            auto worker = sight::core::thread::worker::make();

            int index = -1;

            sight::core::jobs::job::task func1 = [&index](sight::core::jobs::job&)
                                                 {
                                                     CHECK_EQ(index, -1);
                                                     index = 0;
                                                 };

            sight::core::jobs::job::task func2 = [&index](sight::core::jobs::job&)
                                                 {
                                                     CHECK_EQ(index, 0);
                                                     index = 1;
                                                 };

            sight::core::jobs::job::task func3 = [&index](sight::core::jobs::job&)
                                                 {
                                                     CHECK_EQ(index, 1);
                                                     index = 2;
                                                 };

            auto job1 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob1", func1, worker);
            auto job2 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob2", func2, worker);
            auto job3 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob3", func3, worker);

            auto jobs1 = std::make_shared<sight::core::jobs::aggregator>("Aggregator1");
            jobs1->add(job1);
            jobs1->add(job1);
            jobs1->add(job2);
            jobs1->add(job3);
            jobs1->add(job3);
            jobs1->add(job2);

            CHECK_EQ(jobs1->get_sub_jobs().size(), 3);

            jobs1->run().get();
            worker->stop();
        }

        {
            // weight test
            auto func_gen = []() -> sight::core::jobs::job::task
                            {
                                return [ = ](sight::core::jobs::job& _running_job)
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

            auto job1 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob1", func_gen());
            auto job2 = std::make_shared<sight::core::jobs::observer>("GenericCallbackJob2", wu2);
            auto job3 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob3", func_gen());

            job1->set_total_work_units(wu1);
            job3->set_total_work_units(wu3);

            auto jobs1 = std::make_shared<sight::core::jobs::aggregator>("Aggregator1");
            auto jobs2 = std::make_shared<sight::core::jobs::aggregator>("Aggregator2");

            const auto f_norm = static_cast<double>(norm);
            jobs1->add(job1, w1);
            CHECK_EQ(jobs1->get_total_work_units(), std::uint64_t(w1 * f_norm));

            jobs1->add(job2, w2);
            CHECK_EQ(jobs1->get_total_work_units(), std::uint64_t((w2 + w1) * f_norm));

            jobs2->add(job3, w3);
            CHECK_EQ(jobs2->get_total_work_units(), std::uint64_t(f_norm * w3));

            wu2 = 145;
            job2->set_total_work_units(wu2);
            CHECK_EQ(jobs1->get_total_work_units(), std::uint64_t(f_norm * (w2 + w1)));

            double jobs1w = 2015;
            jobs2->add(jobs1, jobs1w);
            CHECK_EQ(jobs2->get_total_work_units(), std::uint64_t(f_norm * (jobs1w + w3)));

            wu3 = 1111;
            job3->set_total_work_units(wu3);
            CHECK_EQ(jobs2->get_total_work_units(), std::uint64_t(f_norm * (jobs1w + w3)));

            jobs2->run();
            job2->done();
            job2->finish();
            jobs2->wait();

            CHECK_EQ(jobs1->get_done_work_units(), std::uint64_t(f_norm * (w2 + w1)));
            CHECK_EQ(jobs2->get_done_work_units(), std::uint64_t(f_norm * (jobs1w + w3)));
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
            sight::core::jobs::base::cancel_request_callback _canceled_callback
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

        const sight::core::jobs::base::cancel_request_callback m_canceled_callback;
    };

//------------------------------------------------------------------------------

    TEST_CASE("observer")
    {
        const std::uint64_t progress(100);
        const auto f_progress = static_cast<double>(progress);
        for(int i = 0 ; i < 10 ; ++i)
        {
            int loops = 100;
            {
                sight::core::jobs::observer job("GenericCallbackJob");

                auto f = [ =, &job](double _d)
                         {
                             job.done_work(std::uint64_t(_d * f_progress));
                         };
                algo_mock_observer algo(new job_observer(f));

                algo.run(loops);
                job.finish();

                CHECK_EQ(job.get_done_work_units(), progress);
            }

            {
                sight::core::jobs::job job("GenericCallbackJob",
                                           [ = ](sight::core::jobs::job& _job)
                    {
                                           auto f = [ =, &_job](double _d)
                        {
                                                    _job.done_work(std::uint64_t(_d * f_progress));
                        };
                                           algo_mock_observer algo(new job_observer(f));
                                           algo.run(loops);
                    });
                job.run();
                CHECK_EQ(job.get_done_work_units(), progress);
            }

            {
                auto worker = sight::core::thread::worker::make();

                loops = 1 << 30;
                sight::core::jobs::job job("GenericCallbackJob",
                                           [ = ](sight::core::jobs::job& _running_job)
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
                CHECK(progress > job.get_done_work_units());
                worker->stop();
            }

            {
                auto worker = sight::core::thread::worker::make();

                loops = 1 << 30;
                sight::core::jobs::job job(
                    "GenericCallbackJob",
                    [ = ](sight::core::jobs::job& _running_job)
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
                CHECK(progress > job.get_done_work_units());
                worker->stop();
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("log")
    {
        int loops = 100;
        {
            sight::core::jobs::observer job("GenericCallbackJob1");

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
            sight::core::jobs::job::task func = [loops](sight::core::jobs::job& _running_job)
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
            sight::core::jobs::job job("GenericCallbackJob2", func);

            job.log("Test of GenericCallbackJob2");

            job.run();
        }

        {
            sight::core::jobs::job::task func = [loops](sight::core::jobs::job& _running_job)
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
            auto job1 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob1", func);
            auto job2 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob2", func);
            auto job3 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob3", func);
            auto job4 = std::make_shared<sight::core::jobs::job>("GenericCallbackJob4", func);

            auto jobs1 = std::make_shared<sight::core::jobs::aggregator>("Aggregator1");
            auto jobs2 = std::make_shared<sight::core::jobs::aggregator>("Aggregator2");

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
            CHECK_EQ(job1->get_logs().size(), std::size_t(nb_logs));
            CHECK_EQ(job2->get_logs().size(), std::size_t(nb_logs));
            CHECK_EQ(job3->get_logs().size(), std::size_t(nb_logs));
            CHECK_EQ(job4->get_logs().size(), std::size_t(nb_logs));
            CHECK_EQ(jobs1->get_logs().size(), std::size_t(3 * nb_logs + 1));
            CHECK_EQ(jobs2->get_logs().size(), std::size_t((3 * nb_logs + 1) + nb_logs + 1));
        }
    }
} // TEST_SUITE
