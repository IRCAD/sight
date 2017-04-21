/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "UndoRedoManagerTest.hpp"

#include <fwCommand/UndoRedoManager.hpp>

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwCommand::ut::UndoRedoManagerTest );

namespace fwCommand
{
namespace ut
{

struct CommandInfo
{
    CommandInfo(const std::string& cmdName, const std::string& actName) :
        commandName(cmdName),
        actionName(actName)
    {

    }

    std::string commandName;
    std::string actionName;
};

typedef std::vector<CommandInfo> CommandLog;

class BogusCommand : public ICommand
{
public:

    BogusCommand(const std::string& description, CommandLog& cmdLog, size_t size = 0) :
        m_description(description),
        m_cmdLog(cmdLog),
        m_size(size)
    {

    }

    //------------------------------------------------------------------------------

    virtual const size_t getSize() const
    {
        return m_size;
    }

    //------------------------------------------------------------------------------

    virtual bool redo()
    {
        m_cmdLog.push_back(CommandInfo(m_description, "redo"));

        return true;
    }

    //------------------------------------------------------------------------------

    virtual bool undo()
    {
        m_cmdLog.push_back(CommandInfo(m_description, "undo"));

        return true;
    }

    std::string m_description;

    CommandLog& m_cmdLog;

    size_t m_size;

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
    ::fwCommand::UndoRedoManager undoRedoManager;
    CommandLog log;

    CPPUNIT_ASSERT_EQUAL(size_t(0), undoRedoManager.getCommandCount());

    // Check that undo and redo fail on an empty history.
    CPPUNIT_ASSERT_EQUAL(false, undoRedoManager.undo());
    CPPUNIT_ASSERT_EQUAL(false, undoRedoManager.redo());

    BogusCommand::sptr testCmd0 = std::make_shared<BogusCommand>(BogusCommand("testCmd0", log));

    undoRedoManager.enqueue(testCmd0);

    // Ensure the element was added.
    CPPUNIT_ASSERT_EQUAL(size_t(1), undoRedoManager.getCommandCount());

    // Add 99 commands to the command history.
    for(int i = 1; i < 100; ++i)
    {
        BogusCommand::sptr testCmdX = std::make_shared<BogusCommand>(BogusCommand("testCmd" + std::to_string(i), log));

        undoRedoManager.enqueue(testCmdX);

        // Ensure the element was added.
        CPPUNIT_ASSERT_EQUAL(size_t(i + 1), undoRedoManager.getCommandCount());
    }

    // Undo 50 commands in the history.
    for(int i = 0; i < 50; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(true, undoRedoManager.undo());

        // The history size should not change.
        CPPUNIT_ASSERT_EQUAL(size_t(100), undoRedoManager.getCommandCount());

        CommandInfo& lastLog = log.back();

        // Check if the last undo command was logged.
        CPPUNIT_ASSERT(lastLog.commandName == "testCmd" + std::to_string(99 - i));
        CPPUNIT_ASSERT(lastLog.actionName == "undo");
    }

    // Enqueue a command at the current history position. (50)
    // All commands beyond this point are dropped.
    BogusCommand::sptr enqueueCmd = std::make_shared<BogusCommand>(BogusCommand("enqueueCmd", log));

    undoRedoManager.enqueue(enqueueCmd);

    CPPUNIT_ASSERT_EQUAL(size_t(51), undoRedoManager.getCommandCount());

    // Redo the last command, this should fail.
    CPPUNIT_ASSERT_EQUAL(false, undoRedoManager.redo());

    // Undo the "enqueueCmd".
    CPPUNIT_ASSERT_EQUAL(true, undoRedoManager.undo());

    CommandInfo& lastLog = log.back();

    CPPUNIT_ASSERT(lastLog.commandName == "enqueueCmd");
    CPPUNIT_ASSERT(lastLog.actionName == "undo");

    // Redo the "enqueueCmd".
    CPPUNIT_ASSERT_EQUAL(true, undoRedoManager.redo());

    lastLog = log.back();

    CPPUNIT_ASSERT(lastLog.commandName == "enqueueCmd");
    CPPUNIT_ASSERT(lastLog.actionName == "redo");
}

//------------------------------------------------------------------------------

void UndoRedoManagerTest::managerMemorySizeTest()
{
    const size_t MAXMEMORY = 10;
    const size_t CMDSIZE   = 2;

    ::fwCommand::UndoRedoManager undoRedoManager(MAXMEMORY);
    CommandLog log;

    for(int i = 0; i < 5; ++i)
    {
        BogusCommand::sptr testCmdI =
            std::make_shared<BogusCommand>(BogusCommand("testCmd" + std::to_string(i), log, CMDSIZE));

        undoRedoManager.enqueue(testCmdI);

        // Ensure all the commands where added.
        CPPUNIT_ASSERT_EQUAL(size_t(i + 1), undoRedoManager.getCommandCount());
    }

    BogusCommand::sptr testCmdI =
        std::make_shared<BogusCommand>(BogusCommand("testCmd5", log, CMDSIZE));

    undoRedoManager.enqueue(testCmdI);

    CPPUNIT_ASSERT_EQUAL(MAXMEMORY, undoRedoManager.getHistorySize());
    CPPUNIT_ASSERT_EQUAL(MAXMEMORY / CMDSIZE, undoRedoManager.getCommandCount());

    // Undo all commands to find them in the log.
    for(int i = 5; i > 0; --i)
    {
        CPPUNIT_ASSERT_EQUAL(true, undoRedoManager.undo());

        CommandInfo& lastLog = log.back();

        CPPUNIT_ASSERT(lastLog.commandName == "testCmd" + std::to_string(i));
    }

    // Assert that "testCmd0" has been removed from the history.
    auto it = std::find_if(log.begin(), log.end(), [](CommandInfo& info){ return info.commandName == "testCmd0"; });

    CPPUNIT_ASSERT(it == log.end());
}

//------------------------------------------------------------------------------

void UndoRedoManagerTest::managerCommandCountTest()
{
    const size_t MAXMEMORY = 10000;
    const size_t MAXELT    = 5;
    const size_t CMDSIZE   = 2;

    ::fwCommand::UndoRedoManager undoRedoManager(MAXMEMORY, MAXELT);
    CommandLog log;

    for(int i = 0; i < 5; ++i)
    {
        BogusCommand::sptr testCmdI =
            std::make_shared<BogusCommand>(BogusCommand("testCmd" + std::to_string(i), log, CMDSIZE));

        undoRedoManager.enqueue(testCmdI);

        // Ensure all the commands where added.
        CPPUNIT_ASSERT_EQUAL(size_t(i + 1), undoRedoManager.getCommandCount());
    }

    BogusCommand::sptr testCmdI =
        std::make_shared<BogusCommand>(BogusCommand("testCmd5", log, CMDSIZE));

    undoRedoManager.enqueue(testCmdI);

    CPPUNIT_ASSERT_EQUAL(MAXELT, undoRedoManager.getCommandCount());

    // Undo all commands to find them in the log.
    for(int i = 5; i > 0; --i)
    {
        CPPUNIT_ASSERT_EQUAL(true, undoRedoManager.undo());

        CommandInfo& lastLog = log.back();

        CPPUNIT_ASSERT(lastLog.commandName == "testCmd" + std::to_string(i));
    }

    // Assert that "testCmd0" has been removed from the history.
    auto it = std::find_if(log.begin(), log.end(), [](CommandInfo& info){ return info.commandName == "testCmd0"; });

    CPPUNIT_ASSERT(it == log.end());
}

//------------------------------------------------------------------------------

} //namespace ut
} //namespace registrationOp
