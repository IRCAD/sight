/************************************************************************
 *
 * Copyright (C) 2023-2026 IRCAD France
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

#include <doctest/doctest.h>

#include "loader.hpp"

#include <core/runtime/module.hpp>
#include <core/runtime/runtime.hpp>
#include <core/thread/worker.hpp>

#include <utest/wait.hpp>

#include <QLineEdit>
#include <QTest>

namespace sight::module::ui::qt::ut
{

namespace
{

class signal_shortcut_test
{
public:

    signal_shortcut_test()
    {
        // Necessary to start with a fresh qApp, if other tests were launched
        sight::core::runtime::unload_module("sight::module::ui::qt");

        m_module = sight::core::runtime::load_module("sight::module::ui::qt");
        m_module->start();

        m_worker = sight::core::thread::worker::make();

        std::tie(m_container, m_child_uid) = make_container();
    }

    ~signal_shortcut_test()
    {
        // Destroy container.
        destroy_container(m_container);
        m_container.reset();

        m_worker->stop();
        m_worker.reset();

        m_module->stop();
    }

    //------------------------------------------------------------------------------

    void single_shortcut_test()
    {
        test_context context(m_child_uid, "S");

        bool called = false;

        auto slot = sight::core::com::new_slot(
            [&]()
                {
                    called = true;
                });

        slot->set_worker(m_worker);
        context.signal_shortcut_srv->signal("activated")->connect(slot);

        // This is essential to get the key shortcut processed
        auto* const active_window = qApp->activeWindow();
        REQUIRE(active_window != nullptr);
        active_window->activateWindow();
        active_window->setFocus(Qt::ActiveWindowFocusReason);
        bool active = QTest::qWaitForWindowActive(active_window);
        REQUIRE(active);

        QTest::keyClick(active_window, Qt::Key_S, Qt::NoModifier);
        SIGHT_TEST_WAIT(called);
        CHECK_EQ(true, called);
    }

    //------------------------------------------------------------------------------

    void multiple_shortcuts_test()
    {
        test_context context(m_child_uid, "S;CTRL+F2");

        bool called = false;

        auto slot = sight::core::com::new_slot(
            [&]()
                {
                    called = true;
                });

        slot->set_worker(m_worker);
        context.signal_shortcut_srv->signal("activated")->connect(slot);

        // This is essential to get the key shortcut processed
        auto* const active_window = qApp->activeWindow();
        REQUIRE(active_window != nullptr);
        active_window->activateWindow();
        active_window->setFocus(Qt::ActiveWindowFocusReason);
        bool active = QTest::qWaitForWindowActive(active_window);
        REQUIRE(active);

        QTest::keyClick(active_window, Qt::Key_S, Qt::NoModifier);
        SIGHT_TEST_WAIT(called);
        CHECK_EQ(true, called);

        called = false;
        QTest::keyClick(active_window, Qt::Key_F2, Qt::ControlModifier);
        SIGHT_TEST_WAIT(called);
        CHECK_EQ(true, called);
    }

    //------------------------------------------------------------------------------

    void enable_disable_test()
    {
        test_context context(m_child_uid, "SHIFT+S;CTRL+F2;L");

        bool called = false;

        auto slot = sight::core::com::new_slot(
            [&]()
                {
                    called = true;
                });

        slot->set_worker(m_worker);
        context.signal_shortcut_srv->signal("activated")->connect(slot);

        // This is essential to get the key shortcut processed
        auto* const active_window = qApp->activeWindow();
        REQUIRE(active_window != nullptr);
        active_window->activateWindow();
        active_window->setFocus(Qt::ActiveWindowFocusReason);
        bool active = QTest::qWaitForWindowActive(active_window);
        REQUIRE(active);

        QTest::keyClick(active_window, Qt::Key_S, Qt::ShiftModifier);
        SIGHT_TEST_WAIT(called);
        CHECK_EQ(true, called);

        context.signal_shortcut_srv->slot("disable")->run();
        called = false;
        QTest::keyClick(active_window, Qt::Key_S, Qt::ShiftModifier);
        SIGHT_TEST_FAIL_WAIT(not called);
        CHECK_EQ(false, called);

        context.signal_shortcut_srv->slot("enable")->run();
        called = false;
        QTest::keyClick(active_window, Qt::Key_L, Qt::NoModifier);
        SIGHT_TEST_WAIT(called);
        CHECK_EQ(true, called);
    }

    //------------------------------------------------------------------------------

    void check_uncheck_test()
    {
        test_context context(m_child_uid, "SHIFT+S;CTRL+F2;L");

        std::atomic_bool called = false;

        auto slot = sight::core::com::new_slot(
            [&called]()
                {
                    called = true;
                });
        slot->set_worker(m_worker);
        context.signal_shortcut_srv->signal("activated")->connect(slot);

        std::atomic_bool checked = false;
        auto checked_slot        = sight::core::com::new_slot(
            [&checked]()
                {
                    checked = true;
                });
        checked_slot->set_worker(m_worker);
        context.signal_shortcut_srv->signal("checked")->connect(checked_slot);

        auto unchecked_slot = sight::core::com::new_slot(
            [&checked]()
                {
                    checked = false;
                });
        unchecked_slot->set_worker(m_worker);
        context.signal_shortcut_srv->signal("unchecked")->connect(unchecked_slot);

        // This is essential to get the key shortcut processed
        auto* const active_window = qApp->activeWindow();
        REQUIRE(active_window != nullptr);
        active_window->activateWindow();
        active_window->setFocus(Qt::ActiveWindowFocusReason);
        bool active = QTest::qWaitForWindowActive(active_window);
        REQUIRE(active);

        QTest::keyClick(active_window, Qt::Key_F2, Qt::ControlModifier);
        SIGHT_TEST_WAIT(called);
        CHECK_EQ(true, called.load());
        SIGHT_TEST_WAIT(checked);
        CHECK_EQ(true, checked.load());

        called = false;
        QTest::keyClick(active_window, Qt::Key_S, Qt::ShiftModifier);
        SIGHT_TEST_WAIT(called);
        CHECK_EQ(true, called.load());
        SIGHT_TEST_WAIT(not checked);
        CHECK_EQ(false, checked.load());

        called = false;
        context.signal_shortcut_srv->slot("check")->run();
        CHECK_EQ(false, called.load());
        SIGHT_TEST_WAIT(checked);
        CHECK_EQ(true, checked.load());

        called = false;
        QTest::keyClick(active_window, Qt::Key_L, Qt::NoModifier);
        SIGHT_TEST_WAIT(called);
        CHECK_EQ(true, called.load());
        SIGHT_TEST_WAIT(not checked);
        CHECK_EQ(false, checked.load());

        QTest::keyClick(active_window, Qt::Key_F2, Qt::ControlModifier);
        SIGHT_TEST_WAIT(called);
        CHECK_EQ(true, called.load());
        SIGHT_TEST_WAIT(checked);
        CHECK_EQ(true, checked.load());

        called = false;
        context.signal_shortcut_srv->slot("uncheck")->run();
        CHECK_EQ(false, called.load());
        SIGHT_TEST_WAIT(not checked);
        CHECK_EQ(false, checked.load());

        called = false;
        context.signal_shortcut_srv->slot("set_checked")->run(true);
        CHECK_EQ(false, called.load());
        SIGHT_TEST_WAIT(checked);
        CHECK_EQ(true, checked.load());

        called = false;
        context.signal_shortcut_srv->slot("set_checked")->run(false);
        CHECK_EQ(false, called.load());
        SIGHT_TEST_WAIT(not checked);
        CHECK_EQ(false, checked.load());
    }

    //------------------------------------------------------------------------------

    void two_instances_test()
    {
        const auto* shortcut_key = "S";
        test_context context(m_child_uid, shortcut_key);

        // Register another service
        auto second_signal_shortcut_srv = service::add("sight::module::ui::qt::com::signal_shortcut");
        {
            // Build configuration
            service::config_t config;
            config.put("<xmlattr>.shortcut", shortcut_key);
            config.put("<xmlattr>.sid", m_child_uid);

            config.add_child("config", config);
            config.put("state.<xmlattr>.enabled", false);

            CHECK_NOTHROW(second_signal_shortcut_srv->configure(config));
            CHECK_NOTHROW(second_signal_shortcut_srv->start().get());
        }
        service_cleaner cleaner(second_signal_shortcut_srv);

        bool called1 = false;
        auto slot1   = sight::core::com::new_slot(
            [&]()
                {
                    called1 = true;
                });
        slot1->set_worker(m_worker);
        context.signal_shortcut_srv->signal("activated")->connect(slot1);

        bool called2 = false;
        auto slot2   = sight::core::com::new_slot(
            [&]()
                {
                    called2 = true;
                });
        slot2->set_worker(m_worker);
        second_signal_shortcut_srv->signal("activated")->connect(slot2);

        // This is essential to get the key shortcut processed
        auto* const active_window = qApp->activeWindow();
        REQUIRE(active_window != nullptr);
        active_window->activateWindow();
        active_window->setFocus(Qt::ActiveWindowFocusReason);
        bool active = QTest::qWaitForWindowActive(active_window);
        REQUIRE(active);

        QTest::keyClick(active_window, Qt::Key_S, Qt::NoModifier);
        SIGHT_TEST_WAIT(called1);
        CHECK_EQ(true, called1);
        SIGHT_TEST_FAIL_WAIT(not called2);
        CHECK_EQ(false, called2);

        context.signal_shortcut_srv->slot("disable")->run();
        second_signal_shortcut_srv->slot("enable")->run();

        called1 = false;
        called2 = false;
        QTest::keyClick(active_window, Qt::Key_S, Qt::NoModifier);
        SIGHT_TEST_WAIT(called2);
        CHECK_EQ(true, called2);
        SIGHT_TEST_FAIL_WAIT(not called1);
        CHECK_EQ(false, called1);

        second_signal_shortcut_srv->slot("disable")->run();
        context.signal_shortcut_srv->slot("enable")->run();

        called1 = false;
        called2 = false;
        QTest::keyClick(active_window, Qt::Key_S, Qt::NoModifier);
        SIGHT_TEST_WAIT(called1);
        CHECK_EQ(true, called1);
        SIGHT_TEST_FAIL_WAIT(not called2);
        CHECK_EQ(false, called2);
    }

private:

    class test_context
    {
    public:

        test_context(const std::string& _uid, const std::string& _shortcuts)
        {
            view = service::add("sight::module::ui::view");
            {
                const std::string config =
                    "<gui>"
                    "<layout type='sight::ui::layout::line'>"
                    "    <orientation value='horizontal' />"
                    "</layout>"
                    "</gui>"
                    "<registry>"
                    "</registry>";

                view->set_id(_uid);
                // Configure and start the container service
                view->set_config(config);
                view->configure();
                view->start().get();
            }

            // Register the service
            signal_shortcut_srv = service::add("sight::module::ui::qt::com::signal_shortcut");
            {
                // Build configuration
                service::config_t config;
                config.put("<xmlattr>.shortcut", _shortcuts);
                config.put("<xmlattr>.sid", _uid);

                config.add_child("config", config);

                CHECK_NOTHROW(signal_shortcut_srv->configure(config));
                CHECK_NOTHROW(signal_shortcut_srv->start().get());
            }
        }

        ~test_context()
        {
            if(signal_shortcut_srv->started())
            {
                signal_shortcut_srv->stop().wait();
            }

            service::remove(signal_shortcut_srv);

            view->stop().get();
            service::remove(view);
        }

        sight::service::base::sptr view;
        sight::service::base::sptr signal_shortcut_srv;
    };

    sight::service::base::sptr m_container;
    sight::core::thread::worker::sptr m_worker;
    std::string m_child_uid;
    std::shared_ptr<sight::core::runtime::module> m_module;
};

} // namespace

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::ut

TEST_SUITE("sight::module::ui::qt::com::signal_shortcut")
{
    TEST_CASE_FIXTURE(sight::module::ui::qt::ut::signal_shortcut_test, "single_shortcut")
    {
        single_shortcut_test();
    }

    TEST_CASE_FIXTURE(sight::module::ui::qt::ut::signal_shortcut_test, "multiple_shortcuts")
    {
        multiple_shortcuts_test();
    }

    TEST_CASE_FIXTURE(sight::module::ui::qt::ut::signal_shortcut_test, "enable_disable")
    {
        enable_disable_test();
    }

    TEST_CASE_FIXTURE(sight::module::ui::qt::ut::signal_shortcut_test, "check_uncheck")
    {
        check_uncheck_test();
    }

    TEST_CASE_FIXTURE(sight::module::ui::qt::ut::signal_shortcut_test, "two_instances")
    {
        two_instances_test();
    }
}
