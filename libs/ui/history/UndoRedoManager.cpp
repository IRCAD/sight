/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "ui/history/UndoRedoManager.hpp"

#include <core/spy_log.hpp>

namespace sight::ui::history
{

//-----------------------------------------------------------------------------

UndoRedoManager::UndoRedoManager(std::size_t _max_memory, std::size_t _max_commands) :
    m_maxMemory(_max_memory),
    m_maxCommands(_max_commands == 0 ? 1 : _max_commands)
{
    SIGHT_ASSERT("The number of commands must be greater than 0", _max_commands > 0);
}

//-----------------------------------------------------------------------------

bool UndoRedoManager::enqueue(command::sptr _cmd)
{
    if(m_maxMemory == 0)
    {
        SIGHT_WARN("Cannot add a command because maxMemory is 0.");
        return false;
    }

    if(_cmd->size() > m_maxMemory)
    {
        SIGHT_WARN("The current command is bigger than the maximum history size");
        return false;
    }

    // Remove all commands following the current history point.
    if(!m_commandQueue.empty())
    {
        for(std::size_t i = m_commandQueue.size() - 1 ; i > std::size_t(m_commandIndex) ; --i)
        {
            m_usedMemory -= m_commandQueue[i]->size();
            m_commandQueue.pop_back();
        }
    }

    // Remove the oldest command if we reached the maximum number of commands.
    if(m_maxCommands == m_commandQueue.size())
    {
        popFront();
    }

    // Remove the oldest commands if we reached the maximum history size.
    while(m_usedMemory + _cmd->size() > m_maxMemory)
    {
        popFront();
    }

    m_commandQueue.push_back(_cmd);
    m_usedMemory  += _cmd->size();
    m_commandIndex = static_cast<std::int64_t>(m_commandQueue.size() - 1);

    return true;
}

//-----------------------------------------------------------------------------

bool UndoRedoManager::redo()
{
    bool success = false;

    if(std::size_t(m_commandIndex) != m_commandQueue.size() - 1)
    {
        m_commandIndex++;
        success = m_commandQueue[std::size_t(m_commandIndex)]->redo();
    }

    return success;
}

//-----------------------------------------------------------------------------

bool UndoRedoManager::undo()
{
    bool success = false;

    if(m_commandIndex > -1)
    {
        success = m_commandQueue[std::size_t(m_commandIndex)]->undo();
        m_commandIndex--;
    }

    return success;
}

//-----------------------------------------------------------------------------

bool UndoRedoManager::canUndo() const
{
    return m_commandIndex > -1;
}

//-----------------------------------------------------------------------------

bool UndoRedoManager::canRedo() const
{
    return (std::size_t(m_commandIndex) != this->getCommandCount() - 1) && (this->getCommandCount() > 0);
}

//-----------------------------------------------------------------------------

void UndoRedoManager::clear()
{
    m_commandQueue.clear();
    m_commandIndex = -1;
    m_usedMemory   = 0;
}

//-----------------------------------------------------------------------------

std::size_t UndoRedoManager::getCommandCount() const
{
    return m_commandQueue.size();
}

//-----------------------------------------------------------------------------

void UndoRedoManager::setCommandCount(std::size_t _cmd_count)
{
    this->clear();
    m_maxCommands = _cmd_count;
}

//-----------------------------------------------------------------------------

std::size_t UndoRedoManager::getHistorySize() const
{
    return m_usedMemory;
}

//-----------------------------------------------------------------------------

void UndoRedoManager::setHistorySize(std::size_t _hist_size)
{
    this->clear();
    m_maxMemory = _hist_size;
}

//-----------------------------------------------------------------------------

void UndoRedoManager::popFront()
{
    auto it = m_commandQueue.begin();

    m_usedMemory -= (*it)->size();
    m_commandQueue.pop_front();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::history
