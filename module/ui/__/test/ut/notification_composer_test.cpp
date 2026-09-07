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

#include <core/com/slot.hpp>
#include <core/notification/has_notifications.hpp>
#include <core/notification/warning.hpp>
#include <core/runtime/types.hpp>
#include <core/thread/worker.hpp>

#include <service/base.hpp>
#include <service/op.hpp>

#include <utest/wait.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <doctest/doctest.h>

#include <cstdint>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace
{

/// Two sensors contributing to the same warning, the case this service exists for.
constexpr auto CONFIG =
    R"(<service>
           <warning>
               <notification id="left_sensor_out_of_range">
                   <message>Left sensor out of range</message>
               </notification>
               <notification id="right_sensor_out_of_range">
                   <message>Right sensor out of range</message>
               </notification>
               <message>Imprecise: ${left_sensor_out_of_range} ${right_sensor_out_of_range}</message>
           </warning>
           <instruction id="tracking_started">
               <message>Move the sensors into range</message>
           </instruction>
       </service>)";

struct notification_composer_fixture
{
    notification_composer_fixture()
    {
        m_composer = sight::service::add("sight::module::ui::notification_composer");
        REQUIRE_MESSAGE(m_composer, "Failed to create service 'sight::module::ui::notification_composer'");

        // Collect everything the service emits, the way a notification zone would.
        m_slot = sight::core::com::new_slot(
            [this](sight::core::notification::base::sptr _notification)
            {
                m_emitted.push_back(std::move(_notification));
            });
        m_worker = sight::core::thread::worker::make();
        m_slot->set_worker(m_worker);
        m_composer->signal(sight::core::notification::has_notifications::signals::NOTIFICATION_CREATED)
        ->connect(m_slot);
    }

    ~notification_composer_fixture()
    {
        if(!m_composer->stopped())
        {
            CHECK_NOTHROW(m_composer->stop().get());
        }

        m_worker->stop();
        sight::service::remove(m_composer);
    }

    notification_composer_fixture(const notification_composer_fixture&)            = delete;
    notification_composer_fixture& operator=(const notification_composer_fixture&) = delete;
    notification_composer_fixture(notification_composer_fixture&&)                 = delete;
    notification_composer_fixture& operator=(notification_composer_fixture&&)      = delete;

    //------------------------------------------------------------------------------

    void start(const std::string& _config = CONFIG) const
    {
        std::stringstream xml;
        xml << _config;

        sight::core::runtime::config_t config;
        boost::property_tree::read_xml(xml, config);

        m_composer->set_config(config.get_child("service"));
        CHECK_NOTHROW(m_composer->configure());
        CHECK_NOTHROW(m_composer->start().get());
    }

    //------------------------------------------------------------------------------

    void run(const std::string& _slot) const
    {
        m_composer->slot(_slot)->run();
    }

    //------------------------------------------------------------------------------

    /// The merged notification, i.e. the last emitted one, once _count have been emitted.
    [[nodiscard]] sight::core::notification::message::sptr last(std::size_t _count)
    {
        SIGHT_TEST_WAIT(m_emitted.size() == _count);
        REQUIRE_EQ(m_emitted.size(), _count);

        return std::dynamic_pointer_cast<sight::core::notification::message>(m_emitted.back());
    }

    /// The text of the _index-th emitted notification, in emission order.
    [[nodiscard]] std::string text_of(std::size_t _index) const
    {
        const auto message = std::dynamic_pointer_cast<sight::core::notification::message>(m_emitted.at(_index));

        return message ? message->text() : std::string();
    }

    /// A notification carrying an id, as has_notifications emits it once the key is resolved.
    [[nodiscard]] static sight::core::notification::warning::sptr received(
        const std::string& _text,
        const std::string& _id,
        bool _cancelable = false
)
    {
        auto notification = std::make_shared<sight::core::notification::warning>(
            sight::core::notification::message::params {.text = _text, .cancelable = _cancelable
            });
        notification->set_id(_id);

        return notification;
    }

    sight::service::base::sptr m_composer;
    sight::core::com::slot_base::sptr m_slot;
    sight::core::thread::worker::sptr m_worker;
    std::vector<sight::core::notification::base::sptr> m_emitted;
};

} // namespace

TEST_SUITE("sight::module::ui::notification_composer")
{
    TEST_CASE_FIXTURE(notification_composer_fixture, "merged_notification_is_created_on_the_first_contribution")
    {
        start();

        // Nothing is displayed as long as no contribution arrived.
        CHECK(m_emitted.empty());

        run("show_left_sensor_out_of_range");

        const auto merged = last(1);
        REQUIRE(merged);
        CHECK_EQ(merged->text(), std::string("Imprecise: Left sensor out of range "));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(notification_composer_fixture, "second_contribution_updates_in_place")
    {
        start();

        run("show_left_sensor_out_of_range");
        const auto merged = last(1);
        REQUIRE(merged);

        run("show_right_sensor_out_of_range");

        // Still a single notification, emitted once: it was updated, not destroyed and recreated. Its
        // consumer follows the update through set_text(), so it is not emitted a second time either.
        SIGHT_TEST_WAIT(merged->text().find("Right sensor") != std::string::npos);
        CHECK_EQ(m_emitted.size(), std::size_t(1));
        CHECK_EQ(merged->text(), std::string("Imprecise: Left sensor out of range Right sensor out of range"));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(notification_composer_fixture, "canceling_one_contribution_keeps_the_others")
    {
        start();

        run("show_left_sensor_out_of_range");
        run("show_right_sensor_out_of_range");
        const auto merged = last(1);
        REQUIRE(merged);

        // What a shared clear slot could not do: the right sensor stays out of range.
        run("cancel_left_sensor_out_of_range");

        SIGHT_TEST_WAIT(merged->text().find("Left sensor") == std::string::npos);
        CHECK_EQ(merged->text(), std::string("Imprecise:  Right sensor out of range"));
        CHECK_FALSE(merged->is_finished());
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(notification_composer_fixture, "canceling_the_last_contribution_cancels_the_merged")
    {
        start();

        run("show_left_sensor_out_of_range");
        const auto merged = last(1);
        REQUIRE(merged);

        run("cancel_left_sensor_out_of_range");

        SIGHT_TEST_WAIT(merged->is_finished());
        CHECK(merged->is_finished());
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(notification_composer_fixture, "canceling_the_merged_cancels_its_contributions")
    {
        start();

        run("show_left_sensor_out_of_range");
        run("show_right_sensor_out_of_range");
        const auto merged = last(1);
        REQUIRE(merged);

        // Acknowledging the merged notification in the notification zone.
        merged->cancel();

        SIGHT_TEST_WAIT(merged->is_finished());
        CHECK(merged->is_finished());

        // A later contribution starts a new merged notification, without the cleared ones.
        run("show_right_sensor_out_of_range");

        const auto next = last(2);
        REQUIRE(next);
        CHECK_NE(next, merged);
        CHECK_EQ(next->text(), std::string("Imprecise:  Right sensor out of range"));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(notification_composer_fixture, "acknowledging_the_merged_clears_its_contributions")
    {
        start();

        run("show_left_sensor_out_of_range");
        const auto merged = last(1);
        REQUIRE(merged);

        // Acknowledging an error in the notification zone finishes it rather than canceling it.
        merged->finish();
        CHECK(merged->is_finished());

        // Showing the same contribution again starts a new merged notification, without any removal first.
        run("show_left_sensor_out_of_range");

        const auto next = last(2);
        REQUIRE(next);
        CHECK_NE(next, merged);
        CHECK_FALSE(next->is_finished());
        CHECK_EQ(next->text(), std::string("Imprecise: Left sensor out of range "));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(notification_composer_fixture, "a_received_notification_contributes_through_its_id")
    {
        start();

        auto notification = received("Sensor drift", "left_sensor_out_of_range", /*_cancelable=*/ true);

        m_composer->slot("add_notification")->run(
            std::static_pointer_cast<sight::core::notification::base>(notification)
        );

        const auto merged = last(1);
        REQUIRE(merged);
        CHECK_EQ(merged->text(), std::string("Imprecise: Sensor drift "));

        // The emitter updating its own text is reflected on the merged notification.
        notification->set_text("Sensor drift (12mm)");
        SIGHT_TEST_WAIT(merged->text().find("12mm") != std::string::npos);
        CHECK_EQ(merged->text(), std::string("Imprecise: Sensor drift (12mm) "));

        // And so is its cancellation.
        notification->cancel();
        SIGHT_TEST_WAIT(merged->is_finished());
        CHECK(merged->is_finished());
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(notification_composer_fixture, "a_finished_notification_drops_its_contribution")
    {
        start();

        // A notification that is not cancelable is dropped when its emitter finishes it instead.
        auto notification = received("Sensor drift", "left_sensor_out_of_range");

        m_composer->slot("add_notification")->run(
            std::static_pointer_cast<sight::core::notification::base>(notification)
        );

        const auto merged = last(1);
        REQUIRE(merged);

        notification->finish();

        SIGHT_TEST_WAIT(merged->is_finished());
        CHECK(merged->is_finished());
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(notification_composer_fixture, "a_notification_without_a_known_id_is_ignored")
    {
        start();

        for(const auto& id : {std::string {}, std::string("unknown")})
        {
            m_composer->slot("add_notification")->run(
                std::static_pointer_cast<sight::core::notification::base>(received("Ignored", id))
            );
        }

        SIGHT_TEST_WAIT(!m_emitted.empty());
        CHECK(m_emitted.empty());
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(notification_composer_fixture, "a_notification_shown_on_start_needs_no_trigger")
    {
        // What the 14 configurations migrated from workflow_guide rely on for their single instruction.
        start(
            R"(<service>
                   <instruction id="entry_point" show_on_start="true">
                       <message>Select the entry point.</message>
                   </instruction>
                   <instruction id="silent">
                       <message>Not shown until asked</message>
                   </instruction>
               </service>)"
        );

        const auto instruction = last(1);
        REQUIRE(instruction);
        CHECK_EQ(instruction->text(), std::string("Select the entry point."));

        // Its cancel slot closes it, and its show slot brings it back.
        run("cancel_entry_point");
        SIGHT_TEST_WAIT(instruction->is_finished());
        CHECK(instruction->is_finished());

        run("show_entry_point");
        CHECK(last(2));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(notification_composer_fixture, "the_notifications_shown_on_start_follow_the_declaration_order")
    {
        // Ids sorting the other way around than they are declared: what the application shows must follow the
        // XML, not the order the notifications happen to be stored in.
        start(
            R"(<service>
                   <instruction id="zulu" show_on_start="true">
                       <message>First declared</message>
                   </instruction>
                   <instruction id="alpha" show_on_start="true">
                       <message>Second declared</message>
                   </instruction>
                   <instruction id="mike">
                       <message>Third declared</message>
                   </instruction>
               </service>)"
        );

        REQUIRE(last(2));
        CHECK_EQ(text_of(0), std::string("First declared"));
        CHECK_EQ(text_of(1), std::string("Second declared"));

        // The cursor is left on the last notification shown, so the sequence carries on with the one declared
        // after it rather than with the one that merely sorts after it.
        run("next");
        CHECK_EQ(last(3)->text(), std::string("Third declared"));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(notification_composer_fixture, "the_regular_notifications_form_a_sequence")
    {
        // The three steps of a workflow, as taranis and imaging declare them.
        start(
            R"(<service>
                   <instruction id="first" show_on_start="true">
                       <message>First</message>
                   </instruction>
                   <instruction id="second">
                       <message>Second</message>
                   </instruction>
                   <instruction id="third">
                       <message>Third</message>
                   </instruction>
               </service>)"
        );

        REQUIRE(last(1));
        CHECK_EQ(last(1)->text(), std::string("First"));

        run("next");
        CHECK_EQ(last(2)->text(), std::string("Second"));

        // Showing one by name moves the cursor, so the next step follows from there rather than from the
        // beginning. This is what the taranis workflows rely on.
        run("show_third");
        CHECK_EQ(last(3)->text(), std::string("Third"));

        run("restart");
        CHECK_EQ(last(4)->text(), std::string("First"));

        // show_current repeats the step without advancing, the way workflow_guide's update did.
        run("show_current");
        CHECK_EQ(last(5)->text(), std::string("First"));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(notification_composer_fixture, "next_past_the_last_step_shows_nothing")
    {
        start(
            R"(<service>
                   <instruction id="only" show_on_start="true">
                       <message>Only</message>
                   </instruction>
               </service>)"
        );

        REQUIRE(last(1));

        run("next");
        run("show_current");

        // Nothing more was emitted: the sequence is over.
        SIGHT_TEST_WAIT(m_emitted.size() > 1);
        CHECK_EQ(m_emitted.size(), std::size_t(1));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(notification_composer_fixture, "a_regular_notification_is_emitted_on_its_slot")
    {
        start();

        run("show_tracking_started");

        const auto instruction = last(1);
        REQUIRE(instruction);
        CHECK_EQ(instruction->text(), std::string("Move the sensors into range"));
        CHECK(std::dynamic_pointer_cast<sight::core::notification::instruction>(instruction));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(notification_composer_fixture, "cancel_drops_every_contribution")
    {
        start();

        run("show_left_sensor_out_of_range");
        run("show_right_sensor_out_of_range");
        const auto merged = last(1);
        REQUIRE(merged);

        run("cancel");

        SIGHT_TEST_WAIT(merged->is_finished());
        CHECK(merged->is_finished());
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(notification_composer_fixture, "a_merged_notification_has_no_duration_of_its_own")
    {
        start();

        run("show_left_sensor_out_of_range");

        const auto merged = last(1);
        REQUIRE(merged);

        // Nothing configured: its consumer applies its own duration for that type, restarted by each
        // contribution.
        CHECK(!merged->duration().has_value());
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(notification_composer_fixture, "a_regular_notification_has_no_duration_of_its_own")
    {
        start();

        run("show_tracking_started");

        const auto instruction = last(1);
        REQUIRE(instruction);

        // Nothing configured: its consumer applies its own duration for that type.
        CHECK(!instruction->duration().has_value());
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(notification_composer_fixture, "a_configured_duration_overrides_the_defaults")
    {
        constexpr auto timed_config =
            R"(<service>
                   <warning duration="1500">
                       <notification id="left_sensor_out_of_range">
                           <message>Left sensor out of range</message>
                       </notification>
                       <message>Imprecise: ${left_sensor_out_of_range}</message>
                   </warning>
                   <instruction id="tracking_started" duration="2500">
                       <message>Move the sensors into range</message>
                   </instruction>
               </service>)";

        start(timed_config);

        run("show_left_sensor_out_of_range");

        const auto merged = last(1);
        REQUIRE(merged);
        REQUIRE(merged->duration().has_value());
        CHECK_EQ(merged->duration()->count(), std::int64_t(1500));

        run("show_tracking_started");

        const auto instruction = last(2);
        REQUIRE(instruction);
        REQUIRE(instruction->duration().has_value());
        CHECK_EQ(instruction->duration()->count(), std::int64_t(2500));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(notification_composer_fixture, "a_configured_icon_is_passed_to_a_merged_notification")
    {
        start(
            R"(<service>
                   <warning icon="sight::module::ui::icons/warning.svg">
                       <notification id="sensor"><message>Sensor</message></notification>
                       <message>Warning: ${sensor}</message>
                   </warning>
               </service>)"
        );

        run("show_sensor");
        const auto merged = last(1);
        REQUIRE(merged);
        REQUIRE(merged->icon().has_value());
        CHECK_EQ(merged->icon()->filename(), std::filesystem::path("warning.svg"));
    }
}
