/************************************************************************
 *
 * Copyright (C) 2023-2025 IRCAD France
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

#include "signal_shortcut_test.hpp"

#include "loader.hpp"

#include <core/runtime/profile.hpp>
#include <core/runtime/runtime.hpp>

#include <ui/__/service.hpp>
#include <ui/qt/container/widget.hpp>

#include <utest/wait.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <QLineEdit>
#include <QTest>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::qt::ut::signal_shortcut_test);

namespace sight::module::ui::qt::ut
{

//------------------------------------------------------------------------------

void signal_shortcut_test::setUp()
{
    // Necessary to start with a fresh qApp, if other tests were launched
    sight::core::runtime::unload_module("sight::module::ui::qt");

    m_module = sight::core::runtime::load_module("sight::module::ui::qt");
    m_module->start();

    m_worker = sight::core::thread::worker::make();

    std::tie(m_container, m_child_uid) = make_container();
}

//------------------------------------------------------------------------------

void signal_shortcut_test::tearDown()
{
    // Destroy container.
    destroy_container(m_container);
    m_container.reset();

    m_worker->stop();
    m_worker.reset();

    m_module->stop();
}

class test_context
{
public:

    test_context(const std::string& _uid, const std::string& _shortcuts)
    {
        view = service::add("sight::module::ui::view");
        {
            std::stringstream config_string;
            config_string
            << "<gui>"
               R"(<layout type="sight::ui::layout::line">)"
               R"(    <orientation value="horizontal" />)"
               R"(</layout>)"
               R"(</gui>)"
               R"(<registry>)"
               R"(</registry>)";

            sight::service::base::config_t config;
            boost::property_tree::read_xml(config_string, config);

            view->set_id(_uid);
            // Configure and start the container service
            view->configure(config);
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

            CPPUNIT_ASSERT_NO_THROW(signal_shortcut_srv->configure(config));
            CPPUNIT_ASSERT_NO_THROW(signal_shortcut_srv->start().get());
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

//------------------------------------------------------------------------------

void signal_shortcut_test::single_shortcut_test()
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
    bool exposed = QTest::qWaitForWindowExposed(qApp->activeWindow());
    CPPUNIT_ASSERT_EQUAL(true, exposed);

    QTest::keyClick(qApp->activeWindow(), Qt::Key_S, Qt::NoModifier);
    SIGHT_TEST_WAIT(called);
    CPPUNIT_ASSERT_EQUAL(true, called);
}

//------------------------------------------------------------------------------

void signal_shortcut_test::multiple_shortcuts_test()
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
    bool exposed = QTest::qWaitForWindowExposed(qApp->activeWindow());
    CPPUNIT_ASSERT_EQUAL(true, exposed);

    QTest::keyClick(qApp->activeWindow(), Qt::Key_S, Qt::NoModifier);
    SIGHT_TEST_WAIT(called);
    CPPUNIT_ASSERT_EQUAL(true, called);

    called = false;
    QTest::keyClick(qApp->activeWindow(), Qt::Key_F2, Qt::ControlModifier);
    SIGHT_TEST_WAIT(called);
    CPPUNIT_ASSERT_EQUAL(true, called);
}

//------------------------------------------------------------------------------

void signal_shortcut_test::enable_disable_test()
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
    bool exposed = QTest::qWaitForWindowExposed(qApp->activeWindow());
    CPPUNIT_ASSERT_EQUAL(true, exposed);

    QTest::keyClick(qApp->activeWindow(), Qt::Key_S, Qt::ShiftModifier);
    SIGHT_TEST_WAIT(called);
    CPPUNIT_ASSERT_EQUAL(true, called);

    context.signal_shortcut_srv->slot("disable")->run();
    called = false;
    QTest::keyClick(qApp->activeWindow(), Qt::Key_S, Qt::ShiftModifier);
    SIGHT_TEST_FAIL_WAIT(not called);
    CPPUNIT_ASSERT_EQUAL(false, called);

    context.signal_shortcut_srv->slot("enable")->run();
    called = false;
    QTest::keyClick(qApp->activeWindow(), Qt::Key_L, Qt::NoModifier);
    SIGHT_TEST_WAIT(called);
    CPPUNIT_ASSERT_EQUAL(true, called);
}

//------------------------------------------------------------------------------

void signal_shortcut_test::check_uncheck_test()
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
    bool exposed = QTest::qWaitForWindowExposed(qApp->activeWindow());
    CPPUNIT_ASSERT_EQUAL(true, exposed);

    QTest::keyClick(qApp->activeWindow(), Qt::Key_F2, Qt::ControlModifier);
    SIGHT_TEST_WAIT(called);
    CPPUNIT_ASSERT_EQUAL(true, called.load());
    SIGHT_TEST_WAIT(checked);
    CPPUNIT_ASSERT_EQUAL(true, checked.load());

    called = false;
    QTest::keyClick(qApp->activeWindow(), Qt::Key_S, Qt::ShiftModifier);
    SIGHT_TEST_WAIT(called);
    CPPUNIT_ASSERT_EQUAL(true, called.load());
    SIGHT_TEST_WAIT(not checked);
    CPPUNIT_ASSERT_EQUAL(false, checked.load());

    called = false;
    context.signal_shortcut_srv->slot("check")->run();
    CPPUNIT_ASSERT_EQUAL(false, called.load());
    SIGHT_TEST_WAIT(checked);
    CPPUNIT_ASSERT_EQUAL(true, checked.load());

    called = false;
    QTest::keyClick(qApp->activeWindow(), Qt::Key_L, Qt::NoModifier);
    SIGHT_TEST_WAIT(called);
    CPPUNIT_ASSERT_EQUAL(true, called.load());
    SIGHT_TEST_WAIT(not checked);
    CPPUNIT_ASSERT_EQUAL(false, checked.load());

    QTest::keyClick(qApp->activeWindow(), Qt::Key_F2, Qt::ControlModifier);
    SIGHT_TEST_WAIT(called);
    CPPUNIT_ASSERT_EQUAL(true, called.load());
    SIGHT_TEST_WAIT(checked);
    CPPUNIT_ASSERT_EQUAL(true, checked.load());

    called = false;
    context.signal_shortcut_srv->slot("uncheck")->run();
    CPPUNIT_ASSERT_EQUAL(false, called.load());
    SIGHT_TEST_WAIT(not checked);
    CPPUNIT_ASSERT_EQUAL(false, checked.load());

    called = false;
    context.signal_shortcut_srv->slot("set_checked")->run(true);
    CPPUNIT_ASSERT_EQUAL(false, called.load());
    SIGHT_TEST_WAIT(checked);
    CPPUNIT_ASSERT_EQUAL(true, checked.load());

    called = false;
    context.signal_shortcut_srv->slot("set_checked")->run(false);
    CPPUNIT_ASSERT_EQUAL(false, called.load());
    SIGHT_TEST_WAIT(not checked);
    CPPUNIT_ASSERT_EQUAL(false, checked.load());
}

//------------------------------------------------------------------------------

void signal_shortcut_test::two_instances_test()
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
        config.put("properties.<xmlattr>.enabled", false);

        CPPUNIT_ASSERT_NO_THROW(second_signal_shortcut_srv->configure(config));
        CPPUNIT_ASSERT_NO_THROW(second_signal_shortcut_srv->start().get());
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
    bool exposed = QTest::qWaitForWindowExposed(qApp->activeWindow());
    CPPUNIT_ASSERT_EQUAL(true, exposed);

    QTest::keyClick(qApp->activeWindow(), Qt::Key_S, Qt::NoModifier);
    SIGHT_TEST_WAIT(called1);
    CPPUNIT_ASSERT_EQUAL(true, called1);
    SIGHT_TEST_FAIL_WAIT(not called2);
    CPPUNIT_ASSERT_EQUAL(false, called2);

    context.signal_shortcut_srv->slot("disable")->run();
    second_signal_shortcut_srv->slot("enable")->run();

    called1 = false;
    called2 = false;
    QTest::keyClick(qApp->activeWindow(), Qt::Key_S, Qt::NoModifier);
    SIGHT_TEST_WAIT(called2);
    CPPUNIT_ASSERT_EQUAL(true, called2);
    SIGHT_TEST_FAIL_WAIT(not called1);
    CPPUNIT_ASSERT_EQUAL(false, called1);

    second_signal_shortcut_srv->slot("disable")->run();
    context.signal_shortcut_srv->slot("enable")->run();

    called1 = false;
    called2 = false;
    QTest::keyClick(qApp->activeWindow(), Qt::Key_S, Qt::NoModifier);
    SIGHT_TEST_WAIT(called1);
    CPPUNIT_ASSERT_EQUAL(true, called1);
    SIGHT_TEST_FAIL_WAIT(not called2);
    CPPUNIT_ASSERT_EQUAL(false, called2);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::ut
