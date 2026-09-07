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

#include <core/notification/error.hpp>
#include <core/notification/information.hpp>
#include <core/notification/instruction.hpp>
#include <core/notification/message.hpp>
#include <core/notification/warning.hpp>
#include <core/runtime/path.hpp>

#include <doctest/doctest.h>

#include <filesystem>
#include <memory>
#include <string>

TEST_SUITE("sight::core::notification")
{
    TEST_CASE("message_metadata_and_state")
    {
        const auto icon = sight::core::runtime::get_resource_file_path(
            "sight::module::ui::icons/information.svg"
        );

        sight::core::notification::message message(
            {.title = "Title", .text = "Body", .icon = icon, .channel = "message_channel"});

        CHECK_EQ(message.title(), std::string("Title"));
        CHECK_EQ(message.text(), std::string("Body"));
        REQUIRE(message.icon().has_value());
        CHECK_EQ(*message.icon(), icon);
        CHECK_EQ(message.channel(), std::string("message_channel"));
        CHECK_EQ(message.state(), sight::core::notification::base::waiting);
        CHECK_FALSE(message.is_finished());

        message.finish();

        CHECK_EQ(message.state(), sight::core::notification::base::finished);
        CHECK(message.is_finished());
    }

//------------------------------------------------------------------------------

    TEST_CASE("message_uses_title_as_default_channel")
    {
        sight::core::notification::message message("Visible title", "Body");

        CHECK_EQ(message.channel(), std::string("Visible title"));
        CHECK_EQ(message.title(), std::string("Visible title"));
    }

//------------------------------------------------------------------------------

    TEST_CASE("derived_messages_keep_message_contract")
    {
        const auto info = std::make_shared<sight::core::notification::information>(
            sight::core::notification::message::params {
            .title = "Info",
            .text  = "Information body",
            .icon  = std::filesystem::path("sight::module::ui::icons/information.svg")
        });
        const auto instruction = std::make_shared<sight::core::notification::instruction>(
            "Instruction",
            "Instruction body"
        );
        const auto warning = std::make_shared<sight::core::notification::warning>("Warning", "Warning body");
        const auto error   = std::make_shared<sight::core::notification::error>("Error", "Error body");

        CHECK(std::dynamic_pointer_cast<sight::core::notification::message>(info) != nullptr);
        CHECK(std::dynamic_pointer_cast<sight::core::notification::message>(instruction) != nullptr);
        CHECK(std::dynamic_pointer_cast<sight::core::notification::message>(warning) != nullptr);
        CHECK(std::dynamic_pointer_cast<sight::core::notification::message>(error) != nullptr);

        CHECK_EQ(info->title(), std::string("Info"));
        CHECK_EQ(instruction->text(), std::string("Instruction body"));
        CHECK_EQ(warning->state(), sight::core::notification::base::waiting);
        CHECK_EQ(error->title(), std::string("Error"));
        CHECK_EQ(error->channel(), std::string("Error"));
        CHECK(info->icon().has_value());
        CHECK(instruction->icon().has_value());
        CHECK(warning->icon().has_value());
        CHECK(error->icon().has_value());

        sight::core::notification::message no_icon(
            {.icon = std::filesystem::path()});
        REQUIRE(no_icon.icon().has_value());
        CHECK(no_icon.icon()->empty());
    }

//------------------------------------------------------------------------------

    TEST_CASE("message_set_text_and_title_fire_change_hooks")
    {
        sight::core::notification::message message("Title", "Body");

        int change_count = 0;
        message.add_change_hook([&change_count]{++change_count;});

        message.set_text("New body");
        CHECK_EQ(message.text(), std::string("New body"));
        CHECK_EQ(change_count, 1);

        message.set_title("New title");
        CHECK_EQ(message.title(), std::string("New title"));
        CHECK_EQ(change_count, 2);
    }

//------------------------------------------------------------------------------

    TEST_CASE("message_change_hook_can_read_back_content")
    {
        sight::core::notification::message message("Title", "Body");

        std::string observed_text;
        message.add_change_hook(
            [&message, &observed_text]
        {
            // Must not deadlock: the hook is fired after set_text() released the write lock.
            observed_text = message.text();
        });

        message.set_text("Updated");
        CHECK_EQ(observed_text, std::string("Updated"));
    }

//------------------------------------------------------------------------------
} // TEST_SUITE
