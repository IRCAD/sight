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

#include "signal_button_test.hpp"

#include "loader.hpp"

#include <core/runtime/profile.hpp>
#include <core/runtime/runtime.hpp>

#include <ui/__/service.hpp>
#include <ui/qt/container/widget.hpp>

#include <utest/wait.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <QAbstractButton>
#include <QApplication>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::qt::ut::signal_button_test);

namespace sight::module::ui::qt::ut
{

//------------------------------------------------------------------------------

void signal_button_test::setUp()
{
    // Necessary to start with a fresh qApp, if other tests were launched
    sight::core::runtime::unload_module("sight::module::ui::qt");

    m_module = sight::core::runtime::load_module("sight::module::ui::qt");
    m_module->start();

    m_worker = sight::core::thread::worker::make();

    std::tie(m_container, m_child_uid) = make_container();
}

//------------------------------------------------------------------------------

void signal_button_test::tearDown()
{
    // Destroy container.
    destroy_container(m_container);
    m_container.reset();

    m_worker->stop();
    m_worker.reset();

    if(m_module->is_started())
    {
        m_module->stop();
    }
}

class signal_button_test_context
{
public:

    explicit signal_button_test_context(const std::string& _uid)
    {
        // Register the service
        signal_button_srv = service::add("sight::module::ui::qt::com::signal_button", _uid);
        {
            // Build configuration
            service::config_t config;
            config.put("text", "test");
            config.put("checked", "false");
            config.put("joystick", "left");
            config.put("checkable", "false");

            config.add_child("config", config);

            CPPUNIT_ASSERT_NO_THROW(signal_button_srv->configure(config));
            CPPUNIT_ASSERT_NO_THROW(signal_button_srv->start().get());
        }
    }

    ~signal_button_test_context()
    {
        if(signal_button_srv->started())
        {
            signal_button_srv->stop().wait();
        }

        service::remove(signal_button_srv);
    }

    sight::service::base::sptr signal_button_srv;
};

//------------------------------------------------------------------------------

void signal_button_test::click_test()
{
    signal_button_test_context context(m_child_uid);

    bool called = false;

    auto slot = sight::core::com::new_slot(
        [&]()
        {
            called = true;
        });

    slot->set_worker(m_worker);
    context.signal_button_srv->signal("clicked")->connect(slot);

    QString button_name = QString::fromStdString(m_child_uid) + "/signal_button";

    for(auto* widget : qApp->allWidgets())
    {
        if(auto* button = widget->findChild<QAbstractButton*>(button_name); button != nullptr)
        {
            QPointer<QAbstractButton> button_ptr(button);

            button_ptr->clicked();
        }
    }

    SIGHT_TEST_WAIT(called);
    CPPUNIT_ASSERT_EQUAL(true, called);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::ut
