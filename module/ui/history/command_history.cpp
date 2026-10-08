/************************************************************************
 *
 * Copyright (C) 2017-2026 IRCAD France
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

#include "command_history.hpp"

#include <core/com/signal.hpp>

namespace sight::module::ui::history
{

//-----------------------------------------------------------------------------

command_history::command_history()
{
    new_slot(slots::ENQUEUE, &command_history::enqueue, this);
    new_slot(slots::UNDO, &command_history::undo, this);
    new_slot(slots::REDO, &command_history::redo, this);
    new_slot(slots::CLEAR, &command_history::clear, this);

    m_can_undo_sig = new_signal<signals::can_do_t>(signals::CAN_UNDO);
    m_can_redo_sig = new_signal<signals::can_do_t>(signals::CAN_REDO);
}

//-----------------------------------------------------------------------------

command_history::~command_history()
= default;

//-----------------------------------------------------------------------------

void command_history::configuring()
{
    service::config_t config = this->get_config();

    auto max_commands = config.get_optional<std::size_t>("maxCommands");
    auto max_memory   = config.get_optional<std::size_t>("maxMemory");

    if(max_commands.is_initialized())
    {
        m_undo_redo_manager.set_command_count(max_commands.value());
    }

    if(max_memory.is_initialized())
    {
        m_undo_redo_manager.set_history_size(max_memory.value());
    }
}

//-----------------------------------------------------------------------------

void command_history::starting()
{
    this->emit_modified_sig();
}

//-----------------------------------------------------------------------------

void command_history::updating()
{
    this->emit_modified_sig();
}

//-----------------------------------------------------------------------------

void command_history::stopping()
{
    m_undo_redo_manager.clear();
}

//-----------------------------------------------------------------------------

void command_history::enqueue(sight::ui::history::command::sptr _command)
{
    m_undo_redo_manager.enqueue(_command);
    this->emit_modified_sig();
}

//-----------------------------------------------------------------------------

void command_history::undo()
{
    m_undo_redo_manager.undo();
    this->emit_modified_sig();
}

//-----------------------------------------------------------------------------

void command_history::redo()
{
    m_undo_redo_manager.redo();
    this->emit_modified_sig();
}

//-----------------------------------------------------------------------------

void command_history::clear()
{
    m_undo_redo_manager.clear();
    this->emit_modified_sig();
}

//-----------------------------------------------------------------------------

void command_history::emit_modified_sig() const
{
    m_can_undo_sig->async_emit(m_undo_redo_manager.can_undo());
    m_can_redo_sig->async_emit(m_undo_redo_manager.can_redo());
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::history
