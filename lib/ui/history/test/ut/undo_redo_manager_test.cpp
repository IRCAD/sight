/************************************************************************
 *
 * Copyright (C) 2021-2026 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include <ui/history/undo_redo_manager.hpp>

#include <doctest/doctest.h>

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace
{

struct command_info
{
    command_info(std::string _cmd_name, std::string _act_name) :
        command_name(std::move(_cmd_name)),
        action_name(std::move(_act_name))
    {
    }

    std::string command_name;
    std::string action_name;
};

using command_log = std::vector<command_info>;

class bogus_command : public sight::ui::history::command
{
public:

    bogus_command(std::string _description, command_log& _cmd_log, std::size_t _size = 0) :
        m_description(std::move(_description)),
        m_cmd_log(_cmd_log),
        m_size(_size)
    {
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] std::size_t size() const override
    {
        return m_size;
    }

    //------------------------------------------------------------------------------

    bool redo() override
    {
        m_cmd_log.emplace_back(m_description, "redo");

        return true;
    }

    //------------------------------------------------------------------------------

    bool undo() override
    {
        m_cmd_log.emplace_back(m_description, "undo");

        return true;
    }

    std::string m_description;

    command_log& m_cmd_log;

    std::size_t m_size;
};

} // namespace

TEST_SUITE("sight::ui::history")
{
//------------------------------------------------------------------------------

    TEST_CASE("manager_enqueue")
    {
        sight::ui::history::undo_redo_manager undo_redo_manager;
        command_log log;

        CHECK_EQ(std::size_t(0), undo_redo_manager.get_command_count());

        // Check that undo and redo fail on an empty history.
        CHECK_EQ(false, undo_redo_manager.undo());
        CHECK_EQ(false, undo_redo_manager.redo());

        bogus_command::sptr test_cmd0 = std::make_shared<bogus_command>(bogus_command("testCmd0", log));

        undo_redo_manager.enqueue(test_cmd0);

        // Ensure the element was added.
        CHECK_EQ(std::size_t(1), undo_redo_manager.get_command_count());

        // Add 99 commands to the command history.
        for(int i = 1 ; i < 100 ; ++i)
        {
            bogus_command::sptr test_cmd_x =
                std::make_shared<bogus_command>(bogus_command("testCmd" + std::to_string(i), log));

            undo_redo_manager.enqueue(test_cmd_x);

            // Ensure the element was added.
            CHECK_EQ(std::size_t(i + 1), undo_redo_manager.get_command_count());
        }

        // Undo 50 commands in the history.
        for(int i = 0 ; i < 50 ; ++i)
        {
            CHECK_EQ(true, undo_redo_manager.undo());

            // The history size should not change.
            CHECK_EQ(std::size_t(100), undo_redo_manager.get_command_count());

            command_info& last_log = log.back();

            // Check if the last undo command was logged.
            CHECK(last_log.command_name == "testCmd" + std::to_string(99 - i));
            CHECK(last_log.action_name == "undo");
        }

        // Enqueue a command at the current history position. (50)
        // All commands beyond this point are dropped.
        bogus_command::sptr enqueue_cmd = std::make_shared<bogus_command>(bogus_command("enqueueCmd", log));

        undo_redo_manager.enqueue(enqueue_cmd);

        CHECK_EQ(std::size_t(51), undo_redo_manager.get_command_count());

        // Redo the last command, this should fail.
        CHECK_EQ(false, undo_redo_manager.redo());

        // Undo the "enqueueCmd".
        CHECK_EQ(true, undo_redo_manager.undo());

        command_info& last_log = log.back();

        CHECK(last_log.command_name == "enqueueCmd");
        CHECK(last_log.action_name == "undo");

        // Redo the "enqueueCmd".
        CHECK_EQ(true, undo_redo_manager.redo());

        last_log = log.back();

        CHECK(last_log.command_name == "enqueueCmd");
        CHECK(last_log.action_name == "redo");
    }

//------------------------------------------------------------------------------

    TEST_CASE("manager_memory_size")
    {
        const std::size_t maxmemory = 10;
        const std::size_t cmdsize   = 2;

        sight::ui::history::undo_redo_manager undo_redo_manager(maxmemory);
        command_log log;

        for(int i = 0 ; i < 5 ; ++i)
        {
            bogus_command::sptr test_cmd_i =
                std::make_shared<bogus_command>(bogus_command("testCmd" + std::to_string(i), log, cmdsize));

            undo_redo_manager.enqueue(test_cmd_i);

            // Ensure all the commands where added.
            CHECK_EQ(std::size_t(i + 1), undo_redo_manager.get_command_count());
        }

        bogus_command::sptr test_cmd_i =
            std::make_shared<bogus_command>(bogus_command("testCmd5", log, cmdsize));

        undo_redo_manager.enqueue(test_cmd_i);

        CHECK_EQ(maxmemory, undo_redo_manager.get_history_size());
        CHECK_EQ(maxmemory / cmdsize, undo_redo_manager.get_command_count());

        // Undo all commands to find them in the log.
        for(int i = 5 ; i > 0 ; --i)
        {
            CHECK_EQ(true, undo_redo_manager.undo());

            command_info& last_log = log.back();

            CHECK(last_log.command_name == "testCmd" + std::to_string(i));
        }

        // Assert that "testCmd0" has been removed from the history.
        auto it = std::ranges::find_if(log, [](command_info& _info){return _info.command_name == "testCmd0";});

        CHECK(it == log.end());
    }

//------------------------------------------------------------------------------

    TEST_CASE("manager_command_count")
    {
        const std::size_t maxmemory = 10000;
        const std::size_t maxelt    = 5;
        const std::size_t cmdsize   = 2;

        sight::ui::history::undo_redo_manager undo_redo_manager(maxmemory, maxelt);
        command_log log;

        for(int i = 0 ; i < 5 ; ++i)
        {
            bogus_command::sptr test_cmd_i =
                std::make_shared<bogus_command>(bogus_command("testCmd" + std::to_string(i), log, cmdsize));

            undo_redo_manager.enqueue(test_cmd_i);

            // Ensure all the commands where added.
            CHECK_EQ(std::size_t(i + 1), undo_redo_manager.get_command_count());
        }

        bogus_command::sptr test_cmd_i =
            std::make_shared<bogus_command>(bogus_command("testCmd5", log, cmdsize));

        undo_redo_manager.enqueue(test_cmd_i);

        CHECK_EQ(maxelt, undo_redo_manager.get_command_count());

        // Undo all commands to find them in the log.
        for(int i = 5 ; i > 0 ; --i)
        {
            CHECK_EQ(true, undo_redo_manager.undo());

            command_info& last_log = log.back();

            CHECK(last_log.command_name == "testCmd" + std::to_string(i));
        }

        // Assert that "testCmd0" has been removed from the history.
        auto it = std::ranges::find_if(log, [](command_info& _info){return _info.command_name == "testCmd0";});

        CHECK(it == log.end());
    }

//------------------------------------------------------------------------------

    TEST_CASE("manager_clear_queue")
    {
        sight::ui::history::undo_redo_manager undo_redo_manager;
        command_log log;

        for(int i = 0 ; i < 5 ; ++i)
        {
            bogus_command::sptr test_cmd_i =
                std::make_shared<bogus_command>(bogus_command("testCmd" + std::to_string(i), log));

            undo_redo_manager.enqueue(test_cmd_i);

            // Ensure all the commands where added.
            CHECK_EQ(std::size_t(i + 1), undo_redo_manager.get_command_count());
        }

        undo_redo_manager.clear();

        CHECK_EQ(std::size_t(0), undo_redo_manager.get_command_count());

        CHECK_EQ(false, undo_redo_manager.undo());
    }
} // TEST_SUITE
