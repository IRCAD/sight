/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "sequencer.hpp"

#include <activity/extension/activity.hpp>

#include <core/runtime/path.hpp>
#include <core/runtime/runtime.hpp>

#include <data/activity_set.hpp>
#include <data/integer.hpp>
#include <data/string.hpp>

#include <service/op.hpp>

#include <ui/__/registry.hpp>
#include <ui/qt/container/widget.hpp>

#include <QApplication>
#include <QMainWindow>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::qt::ut::sequencer);

namespace sight::module::ui::qt::ut
{

struct service_launcher
{
    explicit service_launcher(
        const std::string& _name,
        const service::config_t& _config = {},
        const std::string& _uid          = {}) :
        m_frame(service::add("sight::module::ui::frame")),
        m_service(service::add(_name, _uid.empty() ? _name : _uid))
    {
        CPPUNIT_ASSERT(m_frame);
        CPPUNIT_ASSERT(m_service);

        service::config_t frame_config;
        frame_config.put("gui.frame.name", "guiQtUnitTest");
        frame_config.put("gui.frame.minSize.<xmlattr>.width", "640");
        frame_config.put("gui.frame.minSize.<xmlattr>.height", "480");
        frame_config.put("registry.view.<xmlattr>.sid", _uid.empty() ? _name : _uid);

        // Configure the services
        CPPUNIT_ASSERT_NO_THROW(m_frame->set_config(frame_config));
        CPPUNIT_ASSERT_NO_THROW(m_frame->configure());

        CPPUNIT_ASSERT_NO_THROW(m_service->set_config(_config));
        CPPUNIT_ASSERT_NO_THROW(m_service->configure());
    }

    ~service_launcher()
    {
        if(m_service->started())
        {
            CPPUNIT_ASSERT_NO_THROW(m_service->stop().wait());
        }

        CPPUNIT_ASSERT_NO_THROW(service::unregister_service(m_service));

        if(m_frame->started())
        {
            CPPUNIT_ASSERT_NO_THROW(m_frame->stop().wait());
        }

        CPPUNIT_ASSERT_NO_THROW(service::unregister_service(m_frame));
    }

    //------------------------------------------------------------------------------

    void start()
    {
        // Start the services
        CPPUNIT_ASSERT_NO_THROW(m_frame->start().wait());
        CPPUNIT_ASSERT_NO_THROW(m_service->start().wait());
    }

    const service::base::sptr m_frame;
    const service::base::sptr m_service;
};

//------------------------------------------------------------------------------

void sequencer::setUp()
{
    // Set up context before running a test.
    CPPUNIT_ASSERT(qApp != nullptr);

    static bool done = false;
    if(!done)
    {
        CPPUNIT_ASSERT_NO_THROW(
            core::runtime::add_modules(
                core::runtime::get_resource_file_path(
                    "module_ui_qt_ut"
                )
            )
        );
        CPPUNIT_ASSERT_NO_THROW(core::runtime::load_module("ut_sequencer"));

        done = true;
    }

    CPPUNIT_ASSERT_NO_THROW(sight::activity::extension::activity::get_default()->parse_plugin_infos());
}

//------------------------------------------------------------------------------

void sequencer::tearDown()
{
    // Clean up after the test run.
    CPPUNIT_ASSERT_NO_THROW(sight::activity::extension::activity::get_default()->clear_registry());
}

//------------------------------------------------------------------------------

void sequencer::basic_test()
{
    // Build sequencer configuration
    service::config_t sequencer_config;

    for(int i = 0 ; i < 3 ; ++i)
    {
        auto& activity = sequencer_config.add("activity", "");
        activity.put("<xmlattr>.id", "id_" + std::to_string(i));
        activity.put("<xmlattr>.name", "name_" + std::to_string(i));
    }

    // Register the service
    service_launcher sequencer("sight::module::ui::qt::activity::sequencer", sequencer_config);

    // Set inout
    auto activity_set = std::make_shared<data::activity_set>();
    sequencer.m_service->set_inout(activity_set, "activitySet", true);

    // Start the service
    sequencer.start();

    // This should go to the first activity
    sequencer.m_service->update().wait();
}

//------------------------------------------------------------------------------

void sequencer::reset_requirements_test()
{
    // Build sequencer configuration
    service::config_t sequencer_config;

    for(int i = 0 ; i < 3 ; ++i)
    {
        auto& activity = sequencer_config.add("activity", "");
        activity.put("<xmlattr>.id", "id_" + std::to_string(i));
        activity.put("<xmlattr>.name", "name_" + std::to_string(i));
    }

    // Register the service
    service_launcher sequencer("sight::module::ui::qt::activity::sequencer", sequencer_config);

    // Set inout
    auto activity_set = std::make_shared<data::activity_set>();
    sequencer.m_service->set_inout(activity_set, "activitySet", true);

    // Start the service
    sequencer.start();

    // This should go to the first activity
    sequencer.m_service->update().wait();

    // Add some requirements from "outside"
    activity_set->at(0)->insert_or_assign("outside_1", std::make_shared<data::integer>(1));
    activity_set->at(0)->insert_or_assign("outside_2", std::make_shared<data::integer>(2));

    // Go to the last activity, so all requirements are created
    sequencer.m_service->slot("next")->run();
    sequencer.m_service->slot("next")->run();

    // 3 activities should be in the set
    CPPUNIT_ASSERT_EQUAL(std::size_t(3), activity_set->size());

    // Lambda helper to check the activity set
    const auto& check_activity =
        [&activity_set](bool _should_be_empty, bool _modify = false)
        {
            for(const auto& activity : *activity_set)
            {
                // At least one requirement should be present in the current activity
                CPPUNIT_ASSERT_GREATEREQUAL(std::size_t(1), activity->size());

                for(const auto& [key, value] : *activity)
                {
                    if(key.starts_with("inside_"))
                    {
                        if(auto string = std::dynamic_pointer_cast<data::string>(value); string)
                        {
                            // The initial value should be empty string
                            CPPUNIT_ASSERT_EQUAL(_should_be_empty ? std::string() : key, string->value());

                            if(_modify)
                            {
                                // Set a new value
                                string->set_value(key);
                            }
                        }
                    }
                    else if(key.starts_with("outside_"))
                    {
                        if(auto integer = std::dynamic_pointer_cast<data::integer>(value); integer)
                        {
                            if(key.ends_with("1"))
                            {
                                CPPUNIT_ASSERT_EQUAL(std::int64_t(1), integer->value());
                            }
                            else if(key.ends_with("2"))
                            {
                                CPPUNIT_ASSERT_EQUAL(std::int64_t(2), integer->value());
                            }
                        }
                    }
                }
            }
        };

    // Modify them to simulate user interaction
    check_activity(true, true);

    // Just to be sure, check that nothing change
    sequencer.m_service->update().wait();
    check_activity(false);

    // Reset the requirements
    sequencer.m_service->slot("reset_requirements")->run();

    // Check that the requirements are reset
    check_activity(true);
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::ut
