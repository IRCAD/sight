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

#include <QAbstractButton>
#include <QApplication>
#include <QPointer>

namespace sight::module::ui::qt::ut
{

namespace
{

class signal_button_test
{
public:

    signal_button_test()
    {
        // Necessary to start with a fresh qApp, if other tests were launched
        sight::core::runtime::unload_module("sight::module::ui::qt");

        m_module = sight::core::runtime::load_module("sight::module::ui::qt");
        m_module->start();

        m_worker = sight::core::thread::worker::make();

        std::tie(m_container, m_child_uid) = make_container();
    }

    ~signal_button_test()
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

    //------------------------------------------------------------------------------

    void click_test()
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
        CHECK_EQ(true, called);
    }

private:

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

                CHECK_NOTHROW(signal_button_srv->configure(config));
                CHECK_NOTHROW(signal_button_srv->start().get());
            }
        }

        ~signal_button_test_context()
        {
            if(signal_button_srv->started())
            {
                signal_button_srv->stop().get();
            }

            service::remove(signal_button_srv);
        }

        sight::service::base::sptr signal_button_srv;
    };

    sight::service::base::sptr m_container;
    sight::core::thread::worker::sptr m_worker;
    std::string m_child_uid;
    std::shared_ptr<sight::core::runtime::module> m_module;
};

} // namespace

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::ut

TEST_SUITE("sight::module::ui::qt::com::signal_button")
{
    TEST_CASE_FIXTURE(sight::module::ui::qt::ut::signal_button_test, "click")
    {
        click_test();
    }
}
