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

#include <core/com/has_signals.hpp>
#include <core/com/slot.hpp>
#include <core/notification/aggregator.hpp>
#include <core/notification/base.hpp>
#include <core/notification/has_monitors.hpp>
#include <core/notification/has_notifications.hpp>
#include <core/notification/information.hpp>
#include <core/notification/message.hpp>
#include <core/notification/monitor.hpp>
#include <core/notification/observer.hpp>
#include <core/notification/warning.hpp>
#include <core/runtime/types.hpp>
#include <core/thread/worker.hpp>

#include <utest/wait.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <doctest/doctest.h>

#include <chrono>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

//------------------------------------------------------------------------------

static void algo_mock_generic_callback(
    unsigned int _n,
    sight::core::notification::observer::sptr _progress,
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

namespace
{

struct notification_emitter final : public sight::core::com::has_signals,
                                    public sight::core::notification::has_notifications
{
    notification_emitter() noexcept :
        has_notifications(sight::core::com::has_signals::signals())
    {
    }

    using has_notifications::make_notification;
    using has_notifications::emit_notification_created;
    using has_notifications::instruct;
    using has_notifications::inform;
    using has_notifications::warn;
    using has_notifications::fail;
    using has_notifications::configure_notifications;
};

struct monitor_emitter final : public sight::core::com::has_signals,
                               public sight::core::notification::has_monitors
{
    monitor_emitter() noexcept :
        has_monitors(sight::core::com::has_signals::signals())
    {
    }

    using has_monitors::make_notification;
    using has_monitors::observe;
    using has_monitors::aggregate;
    using has_monitors::emit_notification_created;
};

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
        sight::core::notification::cancel_request_callback_t _canceled_callback
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

    const sight::core::notification::cancel_request_callback_t m_canceled_callback;
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
                m_obs->progress_notify((static_cast<double>(i + 1)) / _n);

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

} // namespace

TEST_SUITE("sight::core::notification")
{
    TEST_CASE("has_notifications_make_notification_emits_created_signal")
    {
        notification_emitter emitter;
        sight::core::notification::base::sptr created_notification;

        const auto signal = emitter.signal<sight::core::notification::has_notifications::signals::notification_created_t>(
            sight::core::notification::has_notifications::signals::NOTIFICATION_CREATED
        );
        REQUIRE(signal);

        const auto slot = sight::core::com::new_slot(
            [&created_notification](sight::core::notification::base::sptr _notification)
        {
            created_notification = std::move(_notification);
        });
        slot->set_worker(sight::core::thread::get_default_worker());
        signal->connect(slot);

        const auto notification =
            notification_emitter::make_notification<sight::core::notification::information>(
                "Info",
                "Body"
            );
        emitter.emit_notification_created(notification);

        REQUIRE(notification);
        CHECK_EQ(notification->title(), std::string("Info"));
        SIGHT_TEST_WAIT(created_notification != nullptr);
        CHECK(created_notification != nullptr);
        CHECK(created_notification == std::static_pointer_cast<sight::core::notification::base>(notification));
        CHECK(std::dynamic_pointer_cast<sight::core::notification::information>(created_notification) != nullptr);
    }

//------------------------------------------------------------------------------

    TEST_CASE("has_notifications_convenience_methods_emit_the_right_type")
    {
        notification_emitter emitter;
        std::vector<sight::core::notification::base::sptr> created_notifications;

        const auto signal = emitter.signal<sight::core::notification::has_notifications::signals::notification_created_t>(
            sight::core::notification::has_notifications::signals::NOTIFICATION_CREATED
        );
        REQUIRE(signal);

        const auto slot = sight::core::com::new_slot(
            [&created_notifications](sight::core::notification::base::sptr _notification)
        {
            created_notifications.push_back(std::move(_notification));
        });
        slot->set_worker(sight::core::thread::get_default_worker());
        signal->connect(slot);

        emitter.instruct("Instruction text");
        emitter.inform("Information text");
        emitter.warn("Warning text");
        emitter.fail(
            sight::core::notification::message::params {
            .text     = "Error text",
            .duration = std::chrono::milliseconds(0),
            .sound    = true
        });

        SIGHT_TEST_WAIT(created_notifications.size() == 4);
        CHECK_EQ(created_notifications.size(), std::size_t(4));

        const auto instruction = std::dynamic_pointer_cast<sight::core::notification::instruction>(
            created_notifications.at(0)
        );
        REQUIRE(instruction);
        CHECK_EQ(instruction->title(), std::string("Instruction"));
        CHECK_EQ(instruction->text(), std::string("Instruction text"));

        const auto information = std::dynamic_pointer_cast<sight::core::notification::information>(
            created_notifications.at(1)
        );
        REQUIRE(information);
        CHECK_EQ(information->title(), std::string("Information"));

        const auto warning = std::dynamic_pointer_cast<sight::core::notification::warning>(
            created_notifications.at(2)
        );
        REQUIRE(warning);
        CHECK_EQ(warning->title(), std::string("Warning"));

        const auto error = std::dynamic_pointer_cast<sight::core::notification::error>(created_notifications.at(3));
        REQUIRE(error);
        CHECK_EQ(error->title(), std::string("Error"));
        REQUIRE(error->duration().has_value());
        CHECK_EQ(error->duration()->count(), std::int64_t(0));
        REQUIRE(error->sound().has_value());
        CHECK_EQ(error->sound().value(), true);
    }

//------------------------------------------------------------------------------

    TEST_CASE("notification_id_defaults_to_empty_and_can_be_set")
    {
        const auto notification = std::make_shared<sight::core::notification::warning>("Warning", "Body");
        CHECK(notification->id().empty());

        notification->set_id("left_sensor_out_of_range");
        CHECK_EQ(notification->id(), std::string("left_sensor_out_of_range"));
    }

//------------------------------------------------------------------------------

    TEST_CASE("params_resolve_the_key_and_keep_the_default_title")
    {
        notification_emitter emitter;
        std::vector<sight::core::notification::base::sptr> created_notifications;

        const auto signal = emitter.signal<sight::core::notification::has_notifications::signals::notification_created_t>(
            sight::core::notification::has_notifications::signals::NOTIFICATION_CREATED
        );
        REQUIRE(signal);

        const auto slot = sight::core::com::new_slot(
            [&created_notifications](sight::core::notification::base::sptr _notification)
        {
            created_notifications.push_back(std::move(_notification));
        });
        slot->set_worker(sight::core::thread::get_default_worker());
        signal->connect(slot);

        // Only the relevant fields are named, the title is left to its default. This emitter has no
        // configuration, so the key resolves to no id at all.
        emitter.warn(
            sight::core::notification::message::params {
            .text = "Left sensor out of range",
            .key  = "out_of_range"
        });

        // An explicit title wins over the default one.
        emitter.inform(
            sight::core::notification::message::params {
            .title   = "Custom",
            .text    = "Body",
            .channel = "channel"
        });

        SIGHT_TEST_WAIT(created_notifications.size() == 2);
        REQUIRE_EQ(created_notifications.size(), std::size_t(2));

        const auto warning = std::dynamic_pointer_cast<sight::core::notification::warning>(
            created_notifications.at(0)
        );
        REQUIRE(warning);
        CHECK_EQ(warning->title(), std::string("Warning"));
        CHECK_EQ(warning->text(), std::string("Left sensor out of range"));
        CHECK(warning->id().empty());

        const auto information = std::dynamic_pointer_cast<sight::core::notification::information>(
            created_notifications.at(1)
        );
        REQUIRE(information);
        CHECK_EQ(information->title(), std::string("Custom"));
        CHECK_EQ(information->channel(), std::string("channel"));
    }

//------------------------------------------------------------------------------

    TEST_CASE("configure_notifications_maps_keys_to_ids")
    {
        notification_emitter emitter;

        std::stringstream xml;
        xml << "<service>"
               "<notification key=\"out_of_range\" id=\"left_sensor_out_of_range\"/>"
               "<notification key=\"disconnected\" id=\"left_sensor_disconnected\"/>"
        // Incomplete elements and other elements are ignored.
               "<notification key=\"no_id\"/>"
               "<notification id=\"no_key\"/>"
               "<properties foo=\"bar\"/>"
               "</service>";

        sight::core::runtime::config_t config;
        boost::property_tree::read_xml(xml, config);

        emitter.configure_notifications(config.get_child("service"));

        // The key is resolved when emitting, so that services never handle the id themselves.
        CHECK_EQ(emitter.warn({.text = "Body", .key = "out_of_range"})->id(), std::string("left_sensor_out_of_range"));
        CHECK_EQ(emitter.fail({.text = "Body", .key = "disconnected"})->id(), std::string("left_sensor_disconnected"));

        // An unmapped key simply yields an empty id: the notification stays unidentified.
        CHECK(emitter.inform({.text   = "Body", .key = "no_id"})->id().empty());
        CHECK(emitter.instruct({.text = "Body", .key = "unknown_key"})->id().empty());
        CHECK(emitter.warn("Body")->id().empty());
    }

//------------------------------------------------------------------------------

    TEST_CASE("has_monitors_make_notification_accepts_monitor_notifications")
    {
        monitor_emitter emitter;
        sight::core::notification::base::sptr created_notification;

        const auto signal = emitter.signal<sight::core::notification::has_notifications::signals::notification_created_t>(
            sight::core::notification::has_notifications::signals::NOTIFICATION_CREATED
        );
        REQUIRE(signal);

        const auto slot = sight::core::com::new_slot(
            [&created_notification](sight::core::notification::base::sptr _notification)
        {
            created_notification = std::move(_notification);
        });
        slot->set_worker(sight::core::thread::get_default_worker());
        signal->connect(slot);

        const auto observer = emitter.observe("Observer", false, nullptr, 42);

        REQUIRE(observer);
        CHECK_EQ(observer->name(), std::string("Observer"));
        SIGHT_TEST_WAIT(created_notification != nullptr);
        CHECK(created_notification != nullptr);
        CHECK(created_notification == std::static_pointer_cast<sight::core::notification::base>(observer));
        CHECK(std::dynamic_pointer_cast<sight::core::notification::observer>(created_notification) != nullptr);

        created_notification.reset();

        const auto aggregator = emitter.make_notification<sight::core::notification::aggregator>("Aggregator");
        REQUIRE(aggregator);
        CHECK_EQ(aggregator->name(), std::string("Aggregator"));

        emitter.emit_notification_created(aggregator);

        SIGHT_TEST_WAIT(created_notification != nullptr);
        CHECK(created_notification != nullptr);
        CHECK(created_notification == std::static_pointer_cast<sight::core::notification::base>(aggregator));
        CHECK(std::dynamic_pointer_cast<sight::core::notification::aggregator>(created_notification) != nullptr);
    }

//------------------------------------------------------------------------------

    TEST_CASE("has_monitors_aggregate_adds_monitors_and_emits_created_signal")
    {
        monitor_emitter emitter;
        sight::core::notification::base::sptr created_notification;

        const auto signal = emitter.signal<sight::core::notification::has_notifications::signals::notification_created_t>(
            sight::core::notification::has_notifications::signals::NOTIFICATION_CREATED
        );
        REQUIRE(signal);

        const auto slot = sight::core::com::new_slot(
            [&created_notification](sight::core::notification::base::sptr _notification)
        {
            created_notification = std::move(_notification);
        });
        slot->set_worker(sight::core::thread::get_default_worker());
        signal->connect(slot);

        const auto observer_1 = emitter.make_notification<sight::core::notification::observer>("Observer1");
        const auto observer_2 = emitter.make_notification<sight::core::notification::observer>("Observer2");

        const auto agg = emitter.aggregate("Aggregate", observer_1, observer_2);

        REQUIRE(agg);
        CHECK_EQ(agg->name(), std::string("Aggregate"));

        SIGHT_TEST_WAIT(created_notification != nullptr);
        CHECK(created_notification != nullptr);
        CHECK(created_notification == std::static_pointer_cast<sight::core::notification::base>(agg));

        observer_1->done_work(100);
        observer_2->done_work(100);

        CHECK_EQ(agg->get_total_work_units(), std::uint64_t(200));
        CHECK_EQ(agg->get_done_work_units(), std::uint64_t(200));
    }

//------------------------------------------------------------------------------

    TEST_CASE("has_monitors_aggregate_supports_weighted_monitors")
    {
        monitor_emitter emitter;

        const auto observer_1 = emitter.make_notification<sight::core::notification::observer>("Observer1");
        const auto observer_2 = emitter.make_notification<sight::core::notification::observer>("Observer2");

        observer_1->set_total_work_units(100);
        observer_2->set_total_work_units(100);

        const auto agg = emitter.aggregate(
            "Aggregate",
            observer_1,
            sight::core::notification::weighted_monitor(observer_2, 0.5)
        );

        REQUIRE(agg);
        CHECK_EQ(agg->get_total_work_units(), std::uint64_t(150));

        observer_1->done();
        observer_2->done();

        CHECK_EQ(agg->get_done_work_units(), std::uint64_t(150));
    }

//------------------------------------------------------------------------------

    TEST_CASE("api_and_state")
    {
        {
            sight::core::notification::observer progress("Observer");
            progress.set_cancelable(true);

            // Uncrustify weirdly format this block after macros
            /* *INDENT-OFF* */
            CHECK_EQ(progress.state(), sight::core::notification::monitor::running);

            CHECK_EQ(progress.cancel_requested(), false);
            CHECK_EQ(progress.cancel_requested_callback()(), false);

            progress.cancel();

            CHECK_EQ(progress.state(), sight::core::notification::monitor::canceled);
            progress.finish();
            CHECK_EQ(progress.state(), sight::core::notification::monitor::canceled);

            CHECK_EQ(progress.cancel_requested(), true);
            CHECK_EQ(progress.cancel_requested_callback()(), true);
            /* *INDENT-ON* */
        }

        {
            sight::core::notification::observer progress("Observer");
            CHECK_EQ(progress.state(), sight::core::notification::monitor::running);

            progress.finish();
            CHECK_EQ(progress.state(), sight::core::notification::monitor::finished);
            progress.cancel();
            CHECK_EQ(progress.state(), sight::core::notification::monitor::finished);

            CHECK_EQ(progress.cancel_requested(), false);
            CHECK_EQ(progress.cancel_requested_callback()(), false);
        }
    }

//------------------------------------------------------------------------------

    TEST_CASE("aggregation")
    {
        {
            auto progress1 = std::make_shared<sight::core::notification::observer>("GenericCallbackprogress1");
            auto progress2 = std::make_shared<sight::core::notification::observer>("GenericCallbackprogress2");
            auto progress3 = std::make_shared<sight::core::notification::observer>("GenericCallbackprogress3");

            auto agg_progress1 = std::make_shared<sight::core::notification::aggregator>("Aggregator1");

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
            auto progress1     = std::make_shared<sight::core::notification::observer>("GenericCallbackprogress1");
            auto progress2     = std::make_shared<sight::core::notification::observer>("GenericCallbackprogress2");
            auto progress3     = std::make_shared<sight::core::notification::observer>("GenericCallbackprogress3");
            auto progress4     = std::make_shared<sight::core::notification::observer>("GenericCallbackprogress4");
            auto agg_progress1 = std::make_shared<sight::core::notification::aggregator>("Aggregator1");
            auto agg_progress2 = std::make_shared<sight::core::notification::aggregator>("Aggregator2");

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
            auto progress1     = std::make_shared<sight::core::notification::observer>("GenericCallbackprogress1");
            auto progress2     = std::make_shared<sight::core::notification::observer>("GenericCallbackprogress2");
            auto progress3     = std::make_shared<sight::core::notification::observer>("GenericCallbackprogress3");
            auto progress4     = std::make_shared<sight::core::notification::observer>("GenericCallbackprogress4");
            auto agg_progress1 = std::make_shared<sight::core::notification::aggregator>("Aggregator1");
            auto agg_progress2 = std::make_shared<sight::core::notification::aggregator>("Aggregator2");

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

            auto progress1 = std::make_shared<sight::core::notification::observer>("GenericCallbackprogress1", wu1);
            auto progress2 = std::make_shared<sight::core::notification::observer>("GenericCallbackprogress2", wu2);
            auto progress3 = std::make_shared<sight::core::notification::observer>("GenericCallbackprogress3", wu3);

            progress1->set_total_work_units(wu1);
            progress3->set_total_work_units(wu3);

            auto agg_progress1 = std::make_shared<sight::core::notification::aggregator>("Aggregator1");
            auto agg_progress2 = std::make_shared<sight::core::notification::aggregator>("Aggregator2");

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
                sight::core::notification::observer progress("GenericCallbackprogress");

                auto f = [ =, &progress](double _d)
                         {
                             progress.done_work(static_cast<std::uint64_t>(_d * static_cast<double>(done)));
                         };
                algo_mock_observer algo(new progress_observer(f));
                algo.run(loops);

                CHECK_EQ(progress.get_done_work_units(), done);
            }

            {
                sight::core::notification::observer progress("GenericCallbackprogressWithCancel");
                progress.set_cancelable(true);

                auto f = [ =, &progress](double _d)
                         {
                             progress.done_work(static_cast<std::uint64_t>(_d * static_cast<double>(done)));
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
            auto progress = std::make_shared<sight::core::notification::observer>("GenericCallbackprogress1");
            progress->log("Test of GenericCallbackprogress1");
            run_log(progress);
        }

        {
            sight::core::notification::observer progress("GenericCallbackprogress2");

            progress.log("Test of GenericCallbackprogress2");
        }

        {
            auto progress1 = std::make_shared<sight::core::notification::observer>("GenericCallbackprogress1");
            auto progress2 = std::make_shared<sight::core::notification::observer>("GenericCallbackprogress2");
            auto progress3 = std::make_shared<sight::core::notification::observer>("GenericCallbackprogress3");
            auto progress4 = std::make_shared<sight::core::notification::observer>("GenericCallbackprogress4");

            auto agg_progress1 = std::make_shared<sight::core::notification::aggregator>("Aggregator1");
            auto agg_progress2 = std::make_shared<sight::core::notification::aggregator>("Aggregator2");

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
