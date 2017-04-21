/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwCommand/UndoRedoManager.hpp"

namespace fwCommand
{

//-----------------------------------------------------------------------------

UndoRedoManager::UndoRedoManager(size_t maxMemory, size_t maxCommands) :
    m_maxMemory(maxMemory),
    m_maxCommands(maxCommands == 0 ? 1 : maxCommands),
    m_usedMemory(0),
    m_commandIndex(-1)
{
    SLM_ASSERT("The number of commands must be greater than 0", maxCommands > 0);
}

//-----------------------------------------------------------------------------

void UndoRedoManager::enqueue(ICommand::sptr cmd)
{
    if(m_maxMemory == 0)
    {
        SLM_WARN("Cannot add a command because maxMemory is 0.");
        return;
    }

    if(cmd->getSize() > m_maxMemory)
    {
        SLM_WARN("The current command is bigger than the maximum history size");
        return;
    }

    // Remove all commands following the current history point.
    if(!m_commandQueue.empty())
    {
        for(std::int64_t i = m_commandQueue.size() - 1; i > m_commandIndex; --i)
        {
            m_usedMemory -= m_commandQueue[i]->getSize();
            m_commandQueue.pop_back();
        }
    }

    // Remove the oldest command if we reached the maximum number of commands.
    if(m_maxCommands == m_commandQueue.size())
    {
        popFront();
    }

    // Remove the oldest commands if we reached the maximum history size.
    while(m_usedMemory + cmd->getSize() > m_maxMemory)
    {
        popFront();
    }

    m_commandQueue.push_back(cmd);
    m_usedMemory  += cmd->getSize();
    m_commandIndex = static_cast<std::int64_t>(m_commandQueue.size() - 1);
}

//-----------------------------------------------------------------------------

bool UndoRedoManager::redo()
{
    bool success = false;

    if(m_commandIndex != m_commandQueue.size() - 1)
    {
        m_commandIndex++;
        success = m_commandQueue[m_commandIndex]->redo();
    }

    return success;
}

//-----------------------------------------------------------------------------

bool UndoRedoManager::undo()
{
    bool success = false;

    if(m_commandIndex > -1)
    {
        success = m_commandQueue[m_commandIndex]->undo();
        m_commandIndex--;
    }

    return success;
}

//-----------------------------------------------------------------------------

void UndoRedoManager::clear()
{
    m_commandQueue.clear();
    m_commandIndex = -1;
}

//-----------------------------------------------------------------------------

size_t UndoRedoManager::getCommandCount() const
{
    return m_commandQueue.size();
}

//-----------------------------------------------------------------------------

size_t UndoRedoManager::getHistorySize() const
{
    return m_usedMemory;
}

//-----------------------------------------------------------------------------

void UndoRedoManager::popFront()
{
    CommandHistoryType::iterator it = m_commandQueue.begin();

    m_usedMemory -= (*it)->getSize();
    m_commandQueue.pop_front();
}

//-----------------------------------------------------------------------------

} // namespace fwCommand
