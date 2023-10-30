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

undo_redo_manager::undo_redo_manager(std::size_t _max_memory, std::size_t _max_commands) :
    m_max_memory(_max_memory),
    m_max_commands(_max_commands == 0 ? 1 : _max_commands)
{
    SIGHT_ASSERT("The number of commands must be greater than 0", _max_commands > 0);
}

//-----------------------------------------------------------------------------

bool undo_redo_manager::enqueue(command::sptr _cmd)
{
    if(m_max_memory == 0)
    {
        SIGHT_WARN("Cannot add a command because maxMemory is 0.");
        return false;
    }

    if(_cmd->size() > m_max_memory)
    {
        SIGHT_WARN("The current command is bigger than the maximum history size");
        return false;
    }

    // Remove all commands following the current history point.
    if(!m_command_queue.empty())
    {
        for(std::size_t i = m_command_queue.size() - 1 ; i > std::size_t(m_command_index) ; --i)
        {
            m_used_memory -= m_command_queue[i]->size();
            m_command_queue.pop_back();
        }
    }

    // Remove the oldest command if we reached the maximum number of commands.
    if(m_max_commands == m_command_queue.size())
    {
        pop_front();
    }

    // Remove the oldest commands if we reached the maximum history size.
    while(m_used_memory + _cmd->size() > m_max_memory)
    {
        pop_front();
    }

    m_command_queue.push_back(_cmd);
    m_used_memory  += _cmd->size();
    m_command_index = static_cast<std::int64_t>(m_command_queue.size() - 1);

    return true;
}

//-----------------------------------------------------------------------------

bool undo_redo_manager::redo()
{
    bool success = false;

    if(std::size_t(m_command_index) != m_command_queue.size() - 1)
    {
        m_command_index++;
        success = m_command_queue[std::size_t(m_command_index)]->redo();
    }

    return success;
}

//-----------------------------------------------------------------------------

bool undo_redo_manager::undo()
{
    bool success = false;

    if(m_command_index > -1)
    {
        success = m_command_queue[std::size_t(m_command_index)]->undo();
        m_command_index--;
    }

    return success;
}

//-----------------------------------------------------------------------------

bool undo_redo_manager::can_undo() const
{
    return m_command_index > -1;
}

//-----------------------------------------------------------------------------

bool undo_redo_manager::can_redo() const
{
    return (std::size_t(m_command_index) != this->get_command_count() - 1) && (this->get_command_count() > 0);
}

//-----------------------------------------------------------------------------

void undo_redo_manager::clear()
{
    m_command_queue.clear();
    m_command_index = -1;
    m_used_memory   = 0;
}

//-----------------------------------------------------------------------------

std::size_t undo_redo_manager::get_command_count() const
{
    return m_command_queue.size();
}

//-----------------------------------------------------------------------------

void undo_redo_manager::set_command_count(std::size_t _cmd_count)
{
    this->clear();
    m_max_commands = _cmd_count;
}

//-----------------------------------------------------------------------------

std::size_t undo_redo_manager::get_history_size() const
{
    return m_used_memory;
}

//-----------------------------------------------------------------------------

void undo_redo_manager::set_history_size(std::size_t _hist_size)
{
    this->clear();
    m_max_memory = _hist_size;
}

//-----------------------------------------------------------------------------

void undo_redo_manager::pop_front()
{
    auto it = m_command_queue.begin();

    m_used_memory -= (*it)->size();
    m_command_queue.pop_front();
}

//-----------------------------------------------------------------------------

} // namespace sight::ui::history
