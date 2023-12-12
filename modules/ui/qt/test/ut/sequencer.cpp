/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "loader.hpp"

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

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::qt::ut::sequencer);

namespace sight::module::ui::qt::ut
{

//------------------------------------------------------------------------------

void sequencer::setUp()
{
    // Set up context before running a test.
    CPPUNIT_ASSERT(qApp != nullptr);

    static bool done = false;
    if(!done)
    {
        CPPUNIT_ASSERT_NO_THROW(core::runtime::load_module("ut_sequencer"));
        done = true;
    }

    CPPUNIT_ASSERT_NO_THROW(sight::activity::extension::activity::get_default()->parse_plugin_infos());

    // Build container
    std::tie(m_container, m_child_uuid) = make_container();
}

//------------------------------------------------------------------------------

void sequencer::tearDown()
{
    // Destroy container
    destroy_container(m_container);

    // Clean up after the test run.
    CPPUNIT_ASSERT_NO_THROW(sight::activity::extension::activity::get_default()->clear_registry());
}

//------------------------------------------------------------------------------

void sequencer::reset_requirements_test()
{
    // Register the service
    sight::service::base::sptr sequencer(service::add("sight::module::ui::qt::activity::sequencer", m_child_uuid));

    // Will stop the service and unregister it when destroyed
    service_cleaner cleaner(sequencer);

    // Set inout
    auto activity_set = std::make_shared<data::activity_set>();
    sequencer->set_inout(activity_set, "activitySet", true);

    // Build sequencer configuration
    service::config_t sequencer_config;

    for(int i = 0 ; i < 3 ; ++i)
    {
        auto& activity = sequencer_config.add("activity", "");
        activity.put("<xmlattr>.id", "id_" + std::to_string(i));
        activity.put("<xmlattr>.name", "name_" + std::to_string(i));
    }

    // Configure the service
    CPPUNIT_ASSERT_NO_THROW(sequencer->configure(sequencer_config));

    CPPUNIT_ASSERT_NO_THROW(sequencer->start().get());

    // This should go to the first activity
    CPPUNIT_ASSERT_NO_THROW(sequencer->update().get());

    // Add some requirements from "outside"
    activity_set->at(0)->insert_or_assign("outside_1", std::make_shared<data::integer>(1));
    activity_set->at(0)->insert_or_assign("outside_2", std::make_shared<data::integer>(2));

    // Go to the last activity, so all requirements are created
    CPPUNIT_ASSERT_NO_THROW(sequencer->slot("next")->run());
    CPPUNIT_ASSERT_NO_THROW(sequencer->slot("next")->run());

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
    CPPUNIT_ASSERT_NO_THROW(sequencer->update().get());
    check_activity(false);

    // Reset the requirements
    CPPUNIT_ASSERT_NO_THROW(sequencer->slot("reset_requirements")->run());

    // Check that the requirements are reset
    check_activity(true);

    // cleanup
    CPPUNIT_ASSERT_NO_THROW(sequencer->stop().get());
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qt::ut
