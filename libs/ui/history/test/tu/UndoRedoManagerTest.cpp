/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "UndoRedoManagerTest.hpp"

#include <ui/history/UndoRedoManager.hpp>

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::ui::history::ut::UndoRedoManagerTest);

namespace sight::ui::history::ut
{

struct CommandInfo
{
    CommandInfo(std::string _cmd_name, std::string _act_name) :
        commandName(std::move(_cmd_name)),
        actionName(std::move(_act_name))
    {
    }

    std::string commandName;
    std::string actionName;
};

using CommandLog = std::vector<CommandInfo>;

class BogusCommand : public command
{
public:

    BogusCommand(std::string _description, CommandLog& _cmd_log, std::size_t _size = 0) :
        m_description(std::move(_description)),
        m_cmdLog(_cmd_log),
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
        m_cmdLog.emplace_back(m_description, "redo");

        return true;
    }

    //------------------------------------------------------------------------------

    bool undo() override
    {
        m_cmdLog.emplace_back(m_description, "undo");

        return true;
    }

    std::string m_description;

    CommandLog& m_cmdLog;

    std::size_t m_size;
};

//------------------------------------------------------------------------------

void UndoRedoManagerTest::setUp()
{
}

//------------------------------------------------------------------------------

void UndoRedoManagerTest::tearDown()
{
}

//------------------------------------------------------------------------------

void UndoRedoManagerTest::managerEnqueueTest()
{
    ui::history::UndoRedoManager undo_redo_manager;
    CommandLog log;

    CPPUNIT_ASSERT_EQUAL(std::size_t(0), undo_redo_manager.getCommandCount());

    // Check that undo and redo fail on an empty history.
    CPPUNIT_ASSERT_EQUAL(false, undo_redo_manager.undo());
    CPPUNIT_ASSERT_EQUAL(false, undo_redo_manager.redo());

    BogusCommand::sptr test_cmd0 = std::make_shared<BogusCommand>(BogusCommand("testCmd0", log));

    undo_redo_manager.enqueue(test_cmd0);

    // Ensure the element was added.
    CPPUNIT_ASSERT_EQUAL(std::size_t(1), undo_redo_manager.getCommandCount());

    // Add 99 commands to the command history.
    for(int i = 1 ; i < 100 ; ++i)
    {
        BogusCommand::sptr test_cmd_x =
            std::make_shared<BogusCommand>(BogusCommand("testCmd" + std::to_string(i), log));

        undo_redo_manager.enqueue(test_cmd_x);

        // Ensure the element was added.
        CPPUNIT_ASSERT_EQUAL(std::size_t(i + 1), undo_redo_manager.getCommandCount());
    }

    // Undo 50 commands in the history.
    for(int i = 0 ; i < 50 ; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(true, undo_redo_manager.undo());

        // The history size should not change.
        CPPUNIT_ASSERT_EQUAL(std::size_t(100), undo_redo_manager.getCommandCount());

        CommandInfo& last_log = log.back();

        // Check if the last undo command was logged.
        CPPUNIT_ASSERT(last_log.commandName == "testCmd" + std::to_string(99 - i));
        CPPUNIT_ASSERT(last_log.actionName == "undo");
    }

    // Enqueue a command at the current history position. (50)
    // All commands beyond this point are dropped.
    BogusCommand::sptr enqueue_cmd = std::make_shared<BogusCommand>(BogusCommand("enqueueCmd", log));

    undo_redo_manager.enqueue(enqueue_cmd);

    CPPUNIT_ASSERT_EQUAL(std::size_t(51), undo_redo_manager.getCommandCount());

    // Redo the last command, this should fail.
    CPPUNIT_ASSERT_EQUAL(false, undo_redo_manager.redo());

    // Undo the "enqueueCmd".
    CPPUNIT_ASSERT_EQUAL(true, undo_redo_manager.undo());

    CommandInfo& last_log = log.back();

    CPPUNIT_ASSERT(last_log.commandName == "enqueueCmd");
    CPPUNIT_ASSERT(last_log.actionName == "undo");

    // Redo the "enqueueCmd".
    CPPUNIT_ASSERT_EQUAL(true, undo_redo_manager.redo());

    last_log = log.back();

    CPPUNIT_ASSERT(last_log.commandName == "enqueueCmd");
    CPPUNIT_ASSERT(last_log.actionName == "redo");
}

//------------------------------------------------------------------------------

void UndoRedoManagerTest::managerMemorySizeTest()
{
    const std::size_t maxmemory = 10;
    const std::size_t cmdsize   = 2;

    ui::history::UndoRedoManager undo_redo_manager(maxmemory);
    CommandLog log;

    for(int i = 0 ; i < 5 ; ++i)
    {
        BogusCommand::sptr test_cmd_i =
            std::make_shared<BogusCommand>(BogusCommand("testCmd" + std::to_string(i), log, cmdsize));

        undo_redo_manager.enqueue(test_cmd_i);

        // Ensure all the commands where added.
        CPPUNIT_ASSERT_EQUAL(std::size_t(i + 1), undo_redo_manager.getCommandCount());
    }

    BogusCommand::sptr test_cmd_i =
        std::make_shared<BogusCommand>(BogusCommand("testCmd5", log, cmdsize));

    undo_redo_manager.enqueue(test_cmd_i);

    CPPUNIT_ASSERT_EQUAL(maxmemory, undo_redo_manager.getHistorySize());
    CPPUNIT_ASSERT_EQUAL(maxmemory / cmdsize, undo_redo_manager.getCommandCount());

    // Undo all commands to find them in the log.
    for(int i = 5 ; i > 0 ; --i)
    {
        CPPUNIT_ASSERT_EQUAL(true, undo_redo_manager.undo());

        CommandInfo& last_log = log.back();

        CPPUNIT_ASSERT(last_log.commandName == "testCmd" + std::to_string(i));
    }

    // Assert that "testCmd0" has been removed from the history.
    auto it = std::find_if(log.begin(), log.end(), [](CommandInfo& _info){return _info.commandName == "testCmd0";});

    CPPUNIT_ASSERT(it == log.end());
}

//------------------------------------------------------------------------------

void UndoRedoManagerTest::managerCommandCountTest()
{
    const std::size_t maxmemory = 10000;
    const std::size_t maxelt    = 5;
    const std::size_t cmdsize   = 2;

    ui::history::UndoRedoManager undo_redo_manager(maxmemory, maxelt);
    CommandLog log;

    for(int i = 0 ; i < 5 ; ++i)
    {
        BogusCommand::sptr test_cmd_i =
            std::make_shared<BogusCommand>(BogusCommand("testCmd" + std::to_string(i), log, cmdsize));

        undo_redo_manager.enqueue(test_cmd_i);

        // Ensure all the commands where added.
        CPPUNIT_ASSERT_EQUAL(std::size_t(i + 1), undo_redo_manager.getCommandCount());
    }

    BogusCommand::sptr test_cmd_i =
        std::make_shared<BogusCommand>(BogusCommand("testCmd5", log, cmdsize));

    undo_redo_manager.enqueue(test_cmd_i);

    CPPUNIT_ASSERT_EQUAL(maxelt, undo_redo_manager.getCommandCount());

    // Undo all commands to find them in the log.
    for(int i = 5 ; i > 0 ; --i)
    {
        CPPUNIT_ASSERT_EQUAL(true, undo_redo_manager.undo());

        CommandInfo& last_log = log.back();

        CPPUNIT_ASSERT(last_log.commandName == "testCmd" + std::to_string(i));
    }

    // Assert that "testCmd0" has been removed from the history.
    auto it = std::find_if(log.begin(), log.end(), [](CommandInfo& _info){return _info.commandName == "testCmd0";});

    CPPUNIT_ASSERT(it == log.end());
}

//------------------------------------------------------------------------------

void UndoRedoManagerTest::managerClearQueueTest()
{
    ui::history::UndoRedoManager undo_redo_manager;
    CommandLog log;

    for(int i = 0 ; i < 5 ; ++i)
    {
        BogusCommand::sptr test_cmd_i =
            std::make_shared<BogusCommand>(BogusCommand("testCmd" + std::to_string(i), log));

        undo_redo_manager.enqueue(test_cmd_i);

        // Ensure all the commands where added.
        CPPUNIT_ASSERT_EQUAL(std::size_t(i + 1), undo_redo_manager.getCommandCount());
    }

    undo_redo_manager.clear();

    CPPUNIT_ASSERT_EQUAL(std::size_t(0), undo_redo_manager.getCommandCount());

    CPPUNIT_ASSERT_EQUAL(false, undo_redo_manager.undo());
}

//------------------------------------------------------------------------------

} // namespace sight::ui::history::ut
