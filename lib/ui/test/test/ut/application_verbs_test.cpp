/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "gui_test_support.hpp"

#include <core/com/signal.hpp>
#include <core/com/signals.hpp>
#include <core/thread/worker.hpp>

#include <data/integer.hpp>
#include <data/mt/locked_ptr.hpp>
#include <data/ptr.hpp>
#include <data/string.hpp>

#include <service/base.hpp>
#include <service/manager.hpp>
#include <service/registry.hpp>

#include <ui/test/detail/locator.hpp>
#include <ui/test/scenario.hpp>

#include <boost/property_tree/ptree.hpp>

#include <doctest/doctest.h>

#include <QAction>
#include <QApplication>
#include <QLabel>
#include <QMetaObject>
#include <QTimer>
#include <QToolBar>
#include <QWidget>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

namespace sight::ui::test::detail
{

namespace
{

/// A service of a configuration, as far as a scenario can see it.
class application_service final : public sight::service::base
{
public:

    SIGHT_DECLARE_SERVICE(application_service, sight::service::base);

    application_service()
    {
        m_changed = new_signal<sight::core::com::signal<void(int)> >("changed");
    }

    /// Emits its signal, as the service would.
    void change(int _value) const
    {
        m_changed->emit(_value);
    }

    /// Emits its signal to be delivered by the worker of each slot.
    void change_later(int _value) const
    {
        m_changed->async_emit(_value);
    }

protected:

    //------------------------------------------------------------------------------

    void configuring(const config_t& /*_config*/) override
    {
    }

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }

    //------------------------------------------------------------------------------

    void updating() override
    {
    }

private:

    sight::core::com::signal<void(int)>::sptr m_changed;
    sight::data::ptr<sight::data::integer, sight::data::access::in> m_waited_for {this, "waited_for"};
};

//------------------------------------------------------------------------------

/// A service that adds outputs on a worker of its own, as a reader with a worker of its own does while a test waits.
class producing_service final : public sight::service::base
{
public:

    SIGHT_DECLARE_SERVICE(producing_service, sight::service::base);

    /// Adds one output, on the worker of the service.
    void add_output(std::size_t _index)
    {
        m_outputs[_index] = std::make_shared<sight::data::integer>(static_cast<std::int64_t>(_index));
    }

protected:

    //------------------------------------------------------------------------------

    void configuring(const config_t& /*_config*/) override
    {
    }

    //------------------------------------------------------------------------------

    void starting() override
    {
    }

    //------------------------------------------------------------------------------

    void stopping() override
    {
    }

    //------------------------------------------------------------------------------

    void updating() override
    {
    }

private:

    sight::data::ptr_vector<sight::data::integer, sight::data::access::out> m_outputs {this, "outputs"};
};

//------------------------------------------------------------------------------

/// One service in the registry for one test, under the id a configuration gives it.
class registered_service
{
public:

    explicit registered_service(const std::string& _id) :
        m_service(std::make_shared<application_service>())
    {
        m_service->set_id(_id);
        sight::service::register_service(m_service);
    }

    registered_service(const registered_service&)            = delete;
    registered_service(registered_service&&)                 = delete;
    registered_service& operator=(const registered_service&) = delete;
    registered_service& operator=(registered_service&&)      = delete;

    ~registered_service()
    {
        remove();
    }

    //------------------------------------------------------------------------------

    void remove()
    {
        if(m_service != nullptr)
        {
            sight::service::unregister_service(m_service);
            m_service.reset();
        }
    }

    //------------------------------------------------------------------------------

    application_service& operator*() const
    {
        return *m_service;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] const application_service::sptr& get() const
    {
        return m_service;
    }

private:

    application_service::sptr m_service;
};

} // namespace

//------------------------------------------------------------------------------

static sight::data::integer::sptr object_with_id(const std::string& _id, int _value = 0)
{
    auto object = std::make_shared<sight::data::integer>(_value);
    object->set_id(_id);
    return object;
}

//------------------------------------------------------------------------------

/// What an operation started while the GUI thread is busy past its timeout threw, if anything.
static std::string while_the_gui_is_busy(QApplication& _application, const std::function<void()>& _operation)
{
    // Owned by the call too: it may still be queued when this returns.
    const auto busy = std::make_shared<std::atomic_bool>(false);
    QMetaObject::invokeMethod(
        &_application,
        [busy]
        {
            *busy = true;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        },
        Qt::QueuedConnection
    );

    const auto ready = std::chrono::steady_clock::now() + std::chrono::seconds(30);
    while(!*busy && std::chrono::steady_clock::now() < ready)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    REQUIRE(busy->load());
    try
    {
        _operation();
    }
    catch(const std::exception& exception)
    {
        return exception.what();
    }

    return {};
}

//------------------------------------------------------------------------------

TEST_SUITE("sight::ui::test::application_verbs")
{
    TEST_CASE_FIXTURE(gui_dispatch_fixture, "holds_for_a_reason")
    {
        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                CHECK_THROWS_AS(ui.hold(std::chrono::milliseconds(1), ""), std::invalid_argument);
                CHECK_THROWS_AS(ui.hold(std::chrono::milliseconds(0), "why"), std::invalid_argument);

                const auto start = std::chrono::steady_clock::now();
                ui.hold(std::chrono::milliseconds(200), "the paused video must not move");
                CHECK(std::chrono::steady_clock::now() - start >= std::chrono::milliseconds(200));

                const auto& steps = ui.step_log();
                CHECK(
                    std::ranges::any_of(
                        steps,
                        [](const std::string& _step)
                {
                    return _step.find("the paused video must not move") != std::string::npos;
                })
                );
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "tells_whether_a_target_exists_without_failing")
    {
        QLabel late("late");
        late.setObjectName("late");
        QToolBar toolbar;
        QAction* const action = toolbar.addAction("action");
        action->setObjectName("toolbar/action");
        toolbar.show();
        QTimer::singleShot(std::chrono::milliseconds(100), &late, [&late]{late.show();});

        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                CHECK_FALSE(ui.exists("missing", std::chrono::milliseconds(20)));
                CHECK(ui.exists("late", std::chrono::seconds(30)));
                CHECK(ui.exists("toolbar/action", std::chrono::seconds(30)));
                CHECK_THROWS_AS(
                    std::ignore = ui.exists("missing", std::chrono::milliseconds(0)),
                    std::invalid_argument
                );
                CHECK_THROWS_AS(std::ignore = ui.exists("a >> ", std::chrono::seconds(1)), std::invalid_argument);
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "finds_an_object_created_with_its_configuration")
    {
        const registered_service in_configuration("config_1-reader");
        const auto image = object_with_id("config_1-image", 42);
        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                CHECK(ui.data<sight::data::integer>("image") == image);
                CHECK(ui.data("image") == image);
                CHECK_THROWS_AS(std::ignore = ui.data<sight::data::string>("image"), std::invalid_argument);
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "waits_for_a_deferred_object")
    {
        const registered_service waiting("config_1-viewer");
        sight::service::base::sptr service = waiting.get();
        sight::service::manager::set_deferred_id(service, "waited_for", "config_1-image", std::nullopt);
        const auto image = std::make_shared<sight::data::integer>(7);

        scenario ui(application);
        QTimer::singleShot(
            std::chrono::milliseconds(100),
            &application,
            [&]{(*waiting).set_input(image, "waited_for");});
        run_scenario(*this, [&]{CHECK(ui.data<sight::data::integer>("image") == image);});
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "reads_a_service_on_its_own_worker")
    {
        // Its worker adds outputs, one task each, while the scenario looks objects up: a read from another thread
        // races with these additions, which the thread sanitizer reports.
        const auto producer = std::make_shared<producing_service>();
        producer->set_id("config_1-producer");
        const auto worker = sight::core::thread::worker::make();
        producer->set_worker(worker);
        sight::service::register_service(producer);
        const auto image = object_with_id("config_1-image", 42);

        constexpr std::size_t outputs = 2000;
        std::vector<std::shared_future<void> > added;
        added.reserve(outputs);
        for(std::size_t index = 0 ; index < outputs ; ++index)
        {
            added.push_back(worker->post_task<void>([&producer, index]{producer->add_output(index);}));
        }

        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                while(added.back().wait_for(std::chrono::seconds(0)) != std::future_status::ready)
                {
                    CHECK(ui.data<sight::data::integer>("image") == image);
                }
            });

        added.back().wait();
        sight::service::unregister_service(producer);
        worker->stop();
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "fails_when_an_object_never_comes")
    {
        const registered_service in_configuration("config_1-reader");
        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                const auto failure = conclusive_failure(
                    [&](std::chrono::milliseconds _timeout){std::ignore = ui.data("image", _timeout);},
                    [](const std::string& _message){return _message.find("'image'") != std::string::npos;});
                CHECK(failure.find("'image'") != std::string::npos);
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "refuses_to_choose_between_objects_of_two_configurations")
    {
        const registered_service first("config_1-slicer");
        const registered_service second("config_2-slicer");
        const auto first_image  = object_with_id("config_1-image", 1);
        const auto second_image = object_with_id("config_2-image", 2);

        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                CHECK_THROWS_WITH_AS(
                    std::ignore = ui.data("image"),
                    doctest::Contains("scope it"),
                    std::invalid_argument
                );
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "expects_data_until_it_holds")
    {
        const registered_service in_configuration("config_1-reader");
        const auto count = object_with_id("config_1-count", 0);
        QTimer::singleShot(
            std::chrono::milliseconds(100),
            &application,
            [&]
            {
                const sight::data::mt::locked_ptr lock(count);
                lock->value() = 2;
            });

        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                ui.expect_data<sight::data::integer>("count").that(
                    "is 2",
                    [](const sight::data::integer& _count){return _count.value() == 2;});

                const auto failure = conclusive_failure(
                    [&](std::chrono::milliseconds _timeout)
                {
                    ui.expect_data<sight::data::integer>("count").within(_timeout).that(
                        "is 3",
                        [](const sight::data::integer& _count){return _count.value() == 3;});
                },
                    [](const std::string& _message){return _message.find("is 3") != std::string::npos;});
                CHECK(failure.find("is 3") != std::string::npos);
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "refuses_an_expectation_on_an_object_of_another_type_at_once")
    {
        const registered_service in_configuration("config_1-reader");
        const auto count = object_with_id("config_1-count", 2);
        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                // A typo in the type, not an object still to come.
                CHECK_THROWS_WITH_AS(
                    ui.expect_data<sight::data::string>("count").within(std::chrono::seconds(2)).that(
                        "is empty",
                        [](const sight::data::string& _text){return _text.value().empty();}),
                    doctest::Contains("is not a"),
                    std::invalid_argument
                );
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "refuses_an_object_the_xml_does_not_declare")
    {
        boost::property_tree::ptree configuration;
        boost::property_tree::ptree object;
        object.put("<xmlattr>.uid", "image");
        object.put("<xmlattr>.type", "sight::data::image");
        configuration.add_child("object", object);
        boost::property_tree::ptree parameter;
        parameter.put("<xmlattr>.name", "passed");
        configuration.add_child("parameters.param", parameter);
        boost::property_tree::ptree templated;
        templated.put("<xmlattr>.uid", "${passed}");
        configuration.add_child("object", templated);

        locator_catalog catalog;
        catalog.add_configuration("config", configuration);
        CHECK(catalog.knows_object("image"));
        CHECK(catalog.knows_object("passed"));
        CHECK_FALSE(catalog.knows_object("${passed}"));
        CHECK_FALSE(catalog.knows_object("imag"));

        scenario ui(application, catalog);
        run_scenario(
            *this,
            [&]
            {
                // At once, whatever the deadline.
                CHECK_THROWS_WITH_AS(
                    std::ignore = ui.data("imag", std::chrono::hours(1)),
                    doctest::Contains("imag"),
                    std::invalid_argument
                );
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "watches_a_signal_of_a_service")
    {
        const registered_service watched("config_1-reader");
        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                auto watch = ui.watch_signal(ui.service("reader"), "changed");

                // The emission counts even before wait, and its arguments do not matter.
                (*watched).change(3);
                watch.wait();

                // One emission satisfies one wait.
                CHECK_THROWS_AS(watch.wait(), std::logic_error);
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "does_not_count_an_emission_before_the_watch")
    {
        const registered_service watched("config_1-reader");
        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                (*watched).change(1);
                auto watch = ui.watch_signal(ui.service("reader"), "changed");
                CHECK_THROWS_WITH_AS(
                    watch.wait(std::chrono::milliseconds(100)),
                    doctest::Contains("timed out"),
                    std::runtime_error
                );
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "refuses_a_service_signal_it_cannot_watch")
    {
        const registered_service first("config_1-reader");
        const registered_service second("config_2-reader");
        const registered_service alone("config_3-alone");
        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                CHECK_THROWS_WITH_AS(
                    std::ignore = ui.watch_signal(ui.service("alone"), "nothing"),
                    doctest::Contains("nothing"),
                    std::invalid_argument
                );
                CHECK_THROWS_WITH_AS(
                    std::ignore = ui.watch_signal(ui.service("reader"), "changed"),
                    doctest::Contains("scope it"),
                    std::invalid_argument
                );
                CHECK_THROWS_AS(
                    std::ignore = ui.watch_signal(ui.service("missing"), "changed", std::chrono::milliseconds(50)),
                    std::runtime_error
                );
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "sees_an_emission_queued_before_the_service_went_away")
    {
        registered_service watched("config_1-reader");
        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                auto watch = ui.watch_signal(ui.service("reader"), "changed");

                // The worker of the watch is busy: the emission waits there while the service goes away.
                std::atomic_bool released {false};
                const auto busy = sight::core::thread::get_default_worker()->post_task<void>(
                    [&released]
                {
                    const auto end = std::chrono::steady_clock::now() + std::chrono::seconds(30);
                    while(!released && std::chrono::steady_clock::now() < end)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
                });
                (*watched).change_later(3);
                watched.remove();
                std::thread releasing(
                    [&released]
                {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    released = true;
                });

                std::string failure;
                failure = failure_of([&]{watch.wait();});

                releasing.join();
                busy.wait();
                CHECK_MESSAGE(failure.empty(), failure);
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "times_out_while_the_worker_of_a_removed_service_still_holds_its_tasks")
    {
        registered_service watched("config_1-reader");
        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                auto watch = ui.watch_signal(ui.service("reader"), "changed");

                // The emission waits on the busy worker of the watch past the deadline: it may still come.
                std::atomic_bool released {false};
                const auto busy = sight::core::thread::get_default_worker()->post_task<void>(
                    [&released]
                {
                    const auto end = std::chrono::steady_clock::now() + std::chrono::seconds(30);
                    while(!released && std::chrono::steady_clock::now() < end)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
                });
                (*watched).change_later(3);
                watched.remove();

                std::string failure;
                failure = failure_of([&]{watch.wait(std::chrono::milliseconds(300));});

                released = true;
                busy.wait();
                CHECK_MESSAGE(failure.find("timed out") != std::string::npos, failure);
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "stops_waiting_for_the_worker_of_a_removed_service_when_the_loop_stops")
    {
        registered_service watched("config_1-reader");
        scenario ui(application);
        std::string failure;
        std::chrono::steady_clock::duration elapsed {};
        run_scenario(
            *this,
            [&]
            {
                auto watch = ui.watch_signal(ui.service("reader"), "changed");

                // The worker of the watch stays busy until the wait returns, then the loop stops under the wait.
                std::atomic_bool released {false};
                const auto busy = sight::core::thread::get_default_worker()->post_task<void>(
                    [&released]
                {
                    const auto end = std::chrono::steady_clock::now() + std::chrono::seconds(60);
                    while(!released && std::chrono::steady_clock::now() < end)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    }
                });
                watched.remove();
                const bool queued = QMetaObject::invokeMethod(
                    &application,
                    []{QTimer::singleShot(std::chrono::milliseconds(500), []{QCoreApplication::exit(0);});},
                    Qt::QueuedConnection
                );
                if(!queued)
                {
                    throw std::runtime_error("could not stop the event loop");
                }

                const auto start = std::chrono::steady_clock::now();
                failure          = failure_of([&]{watch.wait();});

                elapsed  = std::chrono::steady_clock::now() - start;
                released = true;
                busy.wait();
            });

        CHECK(failure == "the application event loop has stopped");
        // Not a measure: the deadline of the wait is 30 s, the loop stops after half a second.
        CHECK(elapsed < std::chrono::seconds(15));
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "tells_when_the_watched_service_goes_away")
    {
        registered_service watched("config_1-reader");
        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                auto watch = ui.watch_signal(ui.service("reader"), "changed");
                watched.remove();
                CHECK_THROWS_WITH_AS(watch.wait(), doctest::Contains("destroyed"), std::runtime_error);
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "waits_for_a_busy_gui_to_answer_an_observation")
    {
        const registered_service watched("config_1-reader");
        const auto count = object_with_id("config_1-count", 2);
        scenario ui(application);
        run_scenario(
            *this,
            [&]
            {
                // The object exists and the signal is there: the GUI answers after the timeout, and that counts.
                constexpr auto timeout = std::chrono::milliseconds(300);
                const std::vector<std::string> failures {
                    while_the_gui_is_busy(application, [&]{std::ignore = ui.data("count", timeout);}),
                    while_the_gui_is_busy(
                        application,
                        [&]
                    {
                        ui.expect_data<sight::data::integer>("count").within(timeout).that(
                            "is 2",
                            [](const sight::data::integer& _count){return _count.value() == 2;});
                    }),
                    while_the_gui_is_busy(
                        application,
                        [&]{std::ignore = ui.watch_signal(ui.service("reader"), "changed", timeout);}),
                };
                for(const auto& failure : failures)
                {
                    CHECK_MESSAGE(failure.empty(), failure);
                }
            });
    }

    TEST_CASE_FIXTURE(gui_dispatch_fixture, "refuses_a_service_the_xml_does_not_declare")
    {
        boost::property_tree::ptree configuration;
        boost::property_tree::ptree declared;
        declared.put("<xmlattr>.uid", "reader");
        declared.put("<xmlattr>.type", "sight::module::io::reader");
        configuration.add_child("service", declared);
        locator_catalog catalog;
        catalog.add_configuration("config", configuration);

        scenario ui(application, catalog);
        run_scenario(
            *this,
            [&]
            {
                CHECK_NOTHROW(std::ignore   = ui.service("reader"));
                CHECK_THROWS_AS(std::ignore = ui.service("writer"), std::invalid_argument);
            });
    }
}

} // namespace sight::ui::test::detail
