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

#include <activity/extension/activity.hpp>

#include <core/runtime/runtime.hpp>
#include <core/thread/worker.hpp>

#include <data/activity_set.hpp>
#include <data/integer.hpp>
#include <data/string.hpp>

#include <service/base.hpp>
#include <service/op.hpp>

#include <utest/wait.hpp>

#include <QApplication>

namespace
{

class sequencer
{
public:

    sequencer()
    {
        // A prior gui_fixture test may have stopped qApp/the default worker in tear_down().
        sight::core::runtime::load_module("sight::module::ui::qt");

        // Set up context before running a test.
        CHECK(qApp != nullptr);

        static bool done = false;
        if(!done)
        {
            CHECK_NOTHROW(sight::core::runtime::load_module("ut_sequencer"));
            CHECK_NOTHROW(sight::core::runtime::load_module("sight::module::app"));
            CHECK_NOTHROW(sight::core::runtime::load_module("sight::module::activity"));
            done = true;
        }

        // Build container
        std::tie(m_container, m_child_uuid) = sight::module::ui::qt::ut::make_container();

        m_worker = sight::core::thread::worker::make();
    }

    ~sequencer()
    {
        // Destroy container
        sight::module::ui::qt::ut::destroy_container(m_container);

        m_worker->stop();
        m_worker.reset();
    }

    //------------------------------------------------------------------------------

    void reset_requirements_test()
    {
        // Register the service
        sight::service::base::sptr sequencer(
            sight::service::add("sight::module::ui::qt::activity::sequencer", m_child_uuid));

        // Will stop the service and unregister it when destroyed
        sight::module::ui::qt::ut::service_cleaner cleaner(sequencer);

        // Set inout
        auto activity_set = std::make_shared<sight::data::activity_set>();
        sequencer->set_inout(activity_set, "activitySet", true);

        // Build sequencer configuration
        sight::service::config_t sequencer_config;

        for(int i = 0 ; i < 3 ; ++i)
        {
            auto& activity = sequencer_config.add("activity", "");
            activity.put("<xmlattr>.id", "id_" + std::to_string(i));
            activity.put("<xmlattr>.name", "name_" + std::to_string(i));
        }

        // Configure the service
        CHECK_NOTHROW(sequencer->configure(sequencer_config));

        CHECK_NOTHROW(sequencer->start().get());

        // This should go to the first activity
        CHECK_NOTHROW(sequencer->update().get());

        // Add some requirements from "outside"
        activity_set->at(0)->insert_or_assign("outside_1", std::make_shared<sight::data::integer>(1));
        activity_set->at(0)->insert_or_assign("outside_2", std::make_shared<sight::data::integer>(2));

        // Go to the last activity, so all requirements are created
        CHECK_NOTHROW(sequencer->slot("next")->run());
        CHECK_NOTHROW(sequencer->slot("next")->run());

        // 3 activities should be in the set
        CHECK_EQ(std::size_t(3), activity_set->size());

        // Lambda helper to check the activity set
        const auto& check_activity =
            [&activity_set](bool _should_be_empty, bool _modify = false)
            {
                for(const auto& activity : *activity_set)
                {
                    // At least one requirement should be present in the current activity
                    CHECK_GE(activity->size(), std::size_t(1));

                    for(const auto& [key, value] : *activity)
                    {
                        if(key.starts_with("inside_"))
                        {
                            if(auto string = std::dynamic_pointer_cast<sight::data::string>(value); string)
                            {
                                // The initial value should be empty string
                                CHECK_EQ(_should_be_empty ? std::string() : key, string->value());

                                if(_modify)
                                {
                                    // Set a new value
                                    string->set_value(key);
                                }
                            }
                        }
                        else if(key.starts_with("outside_"))
                        {
                            if(auto integer = std::dynamic_pointer_cast<sight::data::integer>(value); integer)
                            {
                                if(key.ends_with("1"))
                                {
                                    CHECK_EQ(std::int64_t(1), integer->value());
                                }
                                else if(key.ends_with("2"))
                                {
                                    CHECK_EQ(std::int64_t(2), integer->value());
                                }
                            }
                        }
                    }
                }
            };

        // Modify them to simulate user interaction
        check_activity(true, true);

        // Just to be sure, check that nothing change
        CHECK_NOTHROW(sequencer->update().get());
        check_activity(false);

        // Reset the requirements
        CHECK_NOTHROW(sequencer->slot("reset_requirements")->run());

        // Check that the requirements are reset
        check_activity(true);
    }

    //------------------------------------------------------------------------------

    void go_to_slot_test()
    {
        // Register the service
        sight::service::base::sptr sequencer(sight::service::add(
                                                 "sight::module::ui::qt::activity::sequencer",
                                                 m_child_uuid
        ));
        sight::module::ui::qt::ut::service_cleaner cleaner(sequencer);

        // Set inout
        auto activity_set = std::make_shared<sight::data::activity_set>();
        sequencer->set_inout(
            activity_set,
            "activitySet",
            true
        );

        std::string current_activity_id;
        {
            auto activity_slot = sight::core::com::new_slot(
                [&current_activity_id](sight::data::activity::sptr _activity)
                {
                    current_activity_id = _activity->get_activity_config_id();
                });
            activity_slot->set_worker(m_worker);
            sequencer->signal<sight::core::com::signal<void(sight::data::activity::sptr)> >(
                "activity_created"
            )->connect(
                activity_slot
            );

            // Build sequencer configuration
            sight::service::config_t sequencer_config;
            for(int i = 0 ; i < 3 ; ++i)
            {
                auto& activity = sequencer_config.add("activity", "");
                activity.put("<xmlattr>.id", "id_" + std::to_string(i));
                activity.put("<xmlattr>.name", "name_" + std::to_string(i));
            }

            // Configure and start the service
            CHECK_NOTHROW(sequencer->configure(sequencer_config));
            CHECK_NOTHROW(sequencer->start().get());
            CHECK_NOTHROW(sequencer->update().get());

            activity_set->at(0)->insert_or_assign("outside_1", std::make_shared<sight::data::integer>(1));
            activity_set->at(0)->insert_or_assign("outside_2", std::make_shared<sight::data::integer>(2));

            CHECK_NOTHROW(sequencer->slot("go_to")->run(std::string("id_1")));
            SIGHT_TEST_WAIT(current_activity_id == "id_1");
            CHECK_EQ(std::string("id_1"), current_activity_id);

            CHECK_NOTHROW(sequencer->slot("go_to")->run(std::string("id_2")));
            SIGHT_TEST_WAIT(current_activity_id == "id_2");
            CHECK_EQ(std::string("id_2"), current_activity_id);

            CHECK_NOTHROW(sequencer->slot("previous")->run());
            SIGHT_TEST_WAIT(current_activity_id == "id_1");
            CHECK_EQ(std::string("id_1"), current_activity_id);

            CHECK_NOTHROW(sequencer->slot("go_to")->run(std::string("invalid_id")));
            SIGHT_TEST_WAIT(current_activity_id == "id_1");
            CHECK_EQ(std::string("id_1"), current_activity_id);
        }
    }

private:

    sight::service::base::sptr m_container;
    std::string m_child_uuid;
    sight::core::thread::worker::sptr m_worker;
};

} // namespace

TEST_SUITE("sight::module::ui::qt::activity::sequencer")
{
    TEST_CASE_FIXTURE(sequencer, "reset_requirements")
    {
        reset_requirements_test();
    }

    TEST_CASE_FIXTURE(sequencer, "go_to_slot")
    {
        go_to_slot_test();
    }
}
