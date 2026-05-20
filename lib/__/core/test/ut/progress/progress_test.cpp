/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <core/progress/aggregator.hpp>
#include <core/progress/exception/waiting.hpp>
#include <core/progress/monitor.hpp>
#include <core/progress/observer.hpp>
#include <core/spy_log.hpp>
#include <core/thread/worker.hpp>

#include <doctest/doctest.h>

#include <functional>
#include <future>
#include <string>
#include <thread>

//------------------------------------------------------------------------------

static void algo_mock_generic_callback(
    unsigned int _n,
    sight::core::progress::observer::sptr _progress,
    std::function<void(const std::string&)> _log = nullptr
)
{
    unsigned int div = _n / 15;

    for(unsigned int i = 0 ; i < _n ; ++i)
    {
        // algo ...
        if(_log && i % div == 0)
        {
            _log("algoMockGenericCallback step " + std::to_string(i));
        }

        _progress->done_work(static_cast<std::uint64_t>(i) + 1);
    }
}

//------------------------------------------------------------------------------

class progress_observer_base
{
public:

    virtual void progress_notify(double _p) = 0;
    virtual bool canceled()                 = 0;
    virtual ~progress_observer_base()       = default;
};

struct progress_observer : public progress_observer_base
{
    explicit progress_observer(std::function<void(double)> _func) :
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

struct progress_observer_canceler : public progress_observer
{
    explicit progress_observer_canceler(
        std::function<void(double)> _func,
        sight::core::progress::cancel_request_callback_t _canceled_callback
    ) :
        progress_observer(std::move(_func)),
        m_canceled_callback(std::move(_canceled_callback))
    {
    }

    //------------------------------------------------------------------------------

    bool canceled() override
    {
        return m_canceled_callback();
    }

    const sight::core::progress::cancel_request_callback_t m_canceled_callback;
};

class algo_mock_observer
{
public:

    explicit algo_mock_observer(progress_observer_base* _obs) :
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

    progress_observer_base* m_obs;
    std::atomic_bool m_canceled {false};
};
TEST_SUITE("sight::core::progress")
{
//------------------------------------------------------------------------------

    TEST_CASE("api_and_state")
    {
        {
            sight::core::progress::observer progress("Observer");
            progress.set_cancelable(true);

            // Uncrustify weirdly format this block after macros
            /* *INDENT-OFF* */
            CHECK_EQ(progress.get_state(), sight::core::progress::monitor::running);

            CHECK_EQ(progress.cancel_requested(), false);
            CHECK_EQ(progress.cancel_requested_callback()(), false);

            progress.cancel();

            CHECK_EQ(progress.get_state(), sight::core::progress::monitor::canceled);
            progress.finish();
            CHECK_EQ(progress.get_state(), sight::core::progress::monitor::canceled);

            CHECK_EQ(progress.cancel_requested(), true);
            CHECK_EQ(progress.cancel_requested_callback()(), true);
            /* *INDENT-ON* */
        }

        {
            sight::core::progress::observer progress("Observer");
            CHECK_EQ(progress.get_state(), sight::core::progress::monitor::running);

            progress.finish();
            CHECK_EQ(progress.get_state(), sight::core::progress::monitor::finished);
            progress.cancel();
            CHECK_EQ(progress.get_state(), sight::core::progress::monitor::finished);

            CHECK_EQ(progress.cancel_requested(), false);
            CHECK_EQ(progress.cancel_requested_callback()(), false);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("aggregation")
    {
        {
            auto progress1 = std::make_shared<sight::core::progress::observer>("GenericCallbackprogress1");
            auto progress2 = std::make_shared<sight::core::progress::observer>("GenericCallbackprogress2");
            auto progress3 = std::make_shared<sight::core::progress::observer>("GenericCallbackprogress3");

            auto agg_progress1 = std::make_shared<sight::core::progress::aggregator>("Aggregator1");

            agg_progress1->add(progress1);
            agg_progress1->add(progress2);
            agg_progress1->add(progress3);

            CHECK_EQ(agg_progress1->get_done_work_units(), std::uint64_t(0));

            progress1->done_work(100);
            progress2->done_work(100);
            progress3->add_done_work(30);
            progress3->add_done_work(70);

            CHECK_EQ(agg_progress1->get_total_work_units(), std::uint64_t(300));
            CHECK_EQ(agg_progress1->get_done_work_units(), std::uint64_t(300));
        }

        {
            auto progress1     = std::make_shared<sight::core::progress::observer>("GenericCallbackprogress1");
            auto progress2     = std::make_shared<sight::core::progress::observer>("GenericCallbackprogress2");
            auto progress3     = std::make_shared<sight::core::progress::observer>("GenericCallbackprogress3");
            auto progress4     = std::make_shared<sight::core::progress::observer>("GenericCallbackprogress4");
            auto agg_progress1 = std::make_shared<sight::core::progress::aggregator>("Aggregator1");
            auto agg_progress2 = std::make_shared<sight::core::progress::aggregator>("Aggregator2");

            agg_progress1->add(progress1);
            agg_progress1->add(progress2);
            agg_progress1->add(progress3);

            agg_progress2->add(agg_progress1);
            agg_progress2->add(progress4);

            progress1->done_work(100);
            progress2->done_work(100);
            progress3->done_work(100);
            CHECK_EQ(progress1->get_done_work_units(), std::uint64_t(100));

            progress4->done_work(100);

            CHECK_EQ(agg_progress2->get_total_work_units(), std::uint64_t(200));
            CHECK_EQ(agg_progress2->get_done_work_units(), std::uint64_t(200));
        }

        {
            auto progress1     = std::make_shared<sight::core::progress::observer>("GenericCallbackprogress1");
            auto progress2     = std::make_shared<sight::core::progress::observer>("GenericCallbackprogress2");
            auto progress3     = std::make_shared<sight::core::progress::observer>("GenericCallbackprogress3");
            auto progress4     = std::make_shared<sight::core::progress::observer>("GenericCallbackprogress4");
            auto agg_progress1 = std::make_shared<sight::core::progress::aggregator>("Aggregator1");
            auto agg_progress2 = std::make_shared<sight::core::progress::aggregator>("Aggregator2");

            CHECK_EQ(agg_progress1->get_total_work_units(), std::uint64_t(0));
            CHECK_EQ(agg_progress2->get_total_work_units(), std::uint64_t(0));

            progress1->set_total_work_units(55);

            agg_progress1->add(progress1);
            CHECK_EQ(agg_progress1->get_total_work_units(), std::uint64_t(100));

            agg_progress1->add(progress2);
            CHECK_EQ(agg_progress1->get_total_work_units(), std::uint64_t(200));

            agg_progress1->add(progress3);
            CHECK_EQ(agg_progress1->get_total_work_units(), std::uint64_t(300));

            progress2->set_total_work_units(145);
            CHECK_EQ(agg_progress1->get_total_work_units(), std::uint64_t(300));

            progress2->set_total_work_units(0);
            CHECK_EQ(agg_progress1->get_total_work_units(), std::uint64_t(200));

            progress2->set_total_work_units(100);
            CHECK_EQ(agg_progress1->get_total_work_units(), std::uint64_t(300));

            // set back to 145 to be consistent with progress's task
            progress2->set_total_work_units(145);
            CHECK_EQ(agg_progress1->get_total_work_units(), std::uint64_t(300));

            agg_progress2->add(agg_progress1);
            CHECK_EQ(agg_progress2->get_total_work_units(), std::uint64_t(100));

            agg_progress2->add(progress4);
            CHECK_EQ(agg_progress2->get_total_work_units(), std::uint64_t(200));

            progress3->set_total_work_units(9999);
            CHECK_EQ(agg_progress2->get_total_work_units(), std::uint64_t(200));

            progress4->set_total_work_units(3);
            CHECK_EQ(agg_progress2->get_total_work_units(), std::uint64_t(200));

            CHECK_EQ(agg_progress2->get_total_work_units(), std::uint64_t(200));

            progress1->done();
            progress2->done();
            progress3->done();
            progress4->done();
            CHECK_EQ(agg_progress1->get_done_work_units(), std::uint64_t(300));
            CHECK_EQ(agg_progress2->get_done_work_units(), std::uint64_t(200));
        }

        {
            std::uint64_t norm = 100;
            std::uint64_t wu1  = 55;
            std::uint64_t wu2  = 444;
            std::uint64_t wu3  = 9999;
            double w1          = 2;
            double w2          = 42;
            double w3          = 0.5;

            auto progress1 = std::make_shared<sight::core::progress::observer>("GenericCallbackprogress1", wu1);
            auto progress2 = std::make_shared<sight::core::progress::observer>("GenericCallbackprogress2", wu2);
            auto progress3 = std::make_shared<sight::core::progress::observer>("GenericCallbackprogress3", wu3);

            progress1->set_total_work_units(wu1);
            progress3->set_total_work_units(wu3);

            auto agg_progress1 = std::make_shared<sight::core::progress::aggregator>("Aggregator1");
            auto agg_progress2 = std::make_shared<sight::core::progress::aggregator>("Aggregator2");

            const auto f_norm = static_cast<double>(norm);
            agg_progress1->add(progress1, w1);
            CHECK_EQ(agg_progress1->get_total_work_units(), std::uint64_t(w1 * f_norm));

            agg_progress1->add(progress2, w2);
            CHECK_EQ(agg_progress1->get_total_work_units(), std::uint64_t((w2 + w1) * f_norm));

            agg_progress2->add(progress3, w3);
            CHECK_EQ(agg_progress2->get_total_work_units(), std::uint64_t(f_norm * w3));

            wu2 = 145;
            progress2->set_total_work_units(wu2);
            CHECK_EQ(agg_progress1->get_total_work_units(), std::uint64_t(f_norm * (w2 + w1)));

            double agg_progress1w = 2015;
            agg_progress2->add(agg_progress1, agg_progress1w);
            CHECK_EQ(agg_progress2->get_total_work_units(), std::uint64_t(f_norm * (agg_progress1w + w3)));

            wu3 = 1111;
            progress3->set_total_work_units(wu3);
            CHECK_EQ(agg_progress2->get_total_work_units(), std::uint64_t(f_norm * (agg_progress1w + w3)));

            progress1->done();
            progress2->done();
            progress3->done();

            CHECK_EQ(agg_progress1->get_done_work_units(), std::uint64_t(f_norm * (w2 + w1)));
            CHECK_EQ(agg_progress2->get_done_work_units(), std::uint64_t(f_norm * (agg_progress1w + w3)));
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("observer")
    {
        const std::uint64_t done(100);
        for(int i = 0 ; i < 10 ; ++i)
        {
            const int loops = done;
            {
                sight::core::progress::observer progress("GenericCallbackprogress");

                auto f = [ =, &progress](double _d)
                         {
                             progress.done_work(std::uint64_t(_d * static_cast<double>(done)));
                         };
                algo_mock_observer algo(new progress_observer(f));
                algo.run(loops);

                CHECK_EQ(progress.get_done_work_units(), done);
            }

            {
                sight::core::progress::observer progress("GenericCallbackprogressWithCancel");
                progress.set_cancelable(true);

                auto f = [ =, &progress](double _d)
                         {
                             progress.done_work(std::uint64_t(_d * static_cast<double>(done)));
                             std::this_thread::sleep_for(std::chrono::milliseconds(20));
                         };
                auto cancel_callback = [&progress]() -> bool
                                       {
                                           return progress.cancel_requested();
                                       };
                algo_mock_observer algo(new progress_observer_canceler(f, cancel_callback));
                std::thread algo_thread([&algo](){algo.run(loops);});

                progress.cancel();

                algo_thread.join();

                CHECK_LT(progress.get_done_work_units(), done);
            }
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("log")
    {
        const auto run_log =
            [](auto _progress)
            {
                algo_mock_generic_callback(
                    100,
                    _progress,
                    [&_progress](const std::string& _message)
            {
                _progress->log(_message);
            });
            };
        {
            auto progress = std::make_shared<sight::core::progress::observer>("GenericCallbackprogress1");
            progress->log("Test of GenericCallbackprogress1");
            run_log(progress);
        }

        {
            sight::core::progress::observer progress("GenericCallbackprogress2");

            progress.log("Test of GenericCallbackprogress2");
        }

        {
            auto progress1 = std::make_shared<sight::core::progress::observer>("GenericCallbackprogress1");
            auto progress2 = std::make_shared<sight::core::progress::observer>("GenericCallbackprogress2");
            auto progress3 = std::make_shared<sight::core::progress::observer>("GenericCallbackprogress3");
            auto progress4 = std::make_shared<sight::core::progress::observer>("GenericCallbackprogress4");

            auto agg_progress1 = std::make_shared<sight::core::progress::aggregator>("Aggregator1");
            auto agg_progress2 = std::make_shared<sight::core::progress::aggregator>("Aggregator2");

            agg_progress1->add(progress1);
            agg_progress1->add(progress2);
            agg_progress1->add(progress3);

            progress1->log("Log test of GenericCallbackprogress1");
            progress2->log("Log test of GenericCallbackprogress2");

            agg_progress2->add(agg_progress1);
            agg_progress2->add(progress4);

            progress3->log("Log test of GenericCallbackprogress3");
            progress4->log("Log test of GenericCallbackprogress4");

            agg_progress1->log("Aggregator1");
            agg_progress2->log("Aggregator2");

            run_log(progress1);
            run_log(progress2);
            run_log(progress3);
            run_log(progress4);

            progress1->done();
            progress2->done();
            progress3->done();
            progress4->done();

            auto nb_logs = 18;
            CHECK_EQ(progress1->get_logs().size(), std::size_t(nb_logs));
            CHECK_EQ(progress2->get_logs().size(), std::size_t(nb_logs));
            CHECK_EQ(progress3->get_logs().size(), std::size_t(nb_logs));
            CHECK_EQ(progress4->get_logs().size(), std::size_t(nb_logs));
            CHECK_EQ(agg_progress1->get_logs().size(), std::size_t(3 * nb_logs + 1));
            CHECK_EQ(agg_progress2->get_logs().size(), std::size_t((3 * nb_logs + 1) + nb_logs + 1));
        }
    }
} // TEST_SUITE
