/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>
#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <numeric>

namespace sight::module::ui::history
{

static const core::com::signals::key_t CANUNDO_SIGNAL = "canUndo";
static const core::com::signals::key_t CANREDO_SIGNAL = "canRedo";

static const core::com::slots::key_t ENQUEUE_SLOT = "enqueue";
static const core::com::slots::key_t UNDO_SLOT    = "undo";
static const core::com::slots::key_t REDO_SLOT    = "redo";
static const core::com::slots::key_t CLEAR_SLOT   = "clear";

//-----------------------------------------------------------------------------

command_history::command_history()
{
    new_slot(ENQUEUE_SLOT, &command_history::enqueue, this);
    new_slot(UNDO_SLOT, &command_history::undo, this);
    new_slot(REDO_SLOT, &command_history::redo, this);
    new_slot(CLEAR_SLOT, &command_history::clear, this);

    m_canUndoSig = new_signal<can_do_signal_t>(CANUNDO_SIGNAL);
    m_canRedoSig = new_signal<can_do_signal_t>(CANREDO_SIGNAL);
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
        m_undoRedoManager.setCommandCount(max_commands.value());
    }

    if(max_memory.is_initialized())
    {
        m_undoRedoManager.setHistorySize(max_memory.value());
    }
}

//-----------------------------------------------------------------------------

void command_history::starting()
{
    this->emitModifiedSig();
}

//-----------------------------------------------------------------------------

void command_history::updating()
{
    this->emitModifiedSig();
}

//-----------------------------------------------------------------------------

void command_history::stopping()
{
    m_undoRedoManager.clear();
}

//-----------------------------------------------------------------------------

void command_history::enqueue(sight::ui::history::command::sptr _command)
{
    m_undoRedoManager.enqueue(_command);
    this->emitModifiedSig();
}

//-----------------------------------------------------------------------------

void command_history::undo()
{
    m_undoRedoManager.undo();
    this->emitModifiedSig();
}

//-----------------------------------------------------------------------------

void command_history::redo()
{
    m_undoRedoManager.redo();
    this->emitModifiedSig();
}

//-----------------------------------------------------------------------------

void command_history::clear()
{
    m_undoRedoManager.clear();
    this->emitModifiedSig();
}

//-----------------------------------------------------------------------------

void command_history::emitModifiedSig() const
{
    m_canUndoSig->async_emit(m_undoRedoManager.canUndo());
    m_canRedoSig->async_emit(m_undoRedoManager.canRedo());
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::history
