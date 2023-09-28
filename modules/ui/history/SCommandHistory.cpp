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

#include "SCommandHistory.hpp"

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

SCommandHistory::SCommandHistory()
{
    new_slot(ENQUEUE_SLOT, &SCommandHistory::enqueue, this);
    new_slot(UNDO_SLOT, &SCommandHistory::undo, this);
    new_slot(REDO_SLOT, &SCommandHistory::redo, this);
    new_slot(CLEAR_SLOT, &SCommandHistory::clear, this);

    m_canUndoSig = new_signal<CanDoSignalType>(CANUNDO_SIGNAL);
    m_canRedoSig = new_signal<CanDoSignalType>(CANREDO_SIGNAL);
}

//-----------------------------------------------------------------------------

SCommandHistory::~SCommandHistory()
= default;

//-----------------------------------------------------------------------------

void SCommandHistory::configuring()
{
    service::config_t config = this->getConfiguration();

    auto maxCommands = config.get_optional<std::size_t>("maxCommands");
    auto maxMemory   = config.get_optional<std::size_t>("maxMemory");

    if(maxCommands.is_initialized())
    {
        m_undoRedoManager.setCommandCount(maxCommands.value());
    }

    if(maxMemory.is_initialized())
    {
        m_undoRedoManager.setHistorySize(maxMemory.value());
    }
}

//-----------------------------------------------------------------------------

void SCommandHistory::starting()
{
    this->emitModifiedSig();
}

//-----------------------------------------------------------------------------

void SCommandHistory::updating()
{
    this->emitModifiedSig();
}

//-----------------------------------------------------------------------------

void SCommandHistory::stopping()
{
    m_undoRedoManager.clear();
}

//-----------------------------------------------------------------------------

void SCommandHistory::enqueue(sight::ui::history::command::sptr command)
{
    m_undoRedoManager.enqueue(command);
    this->emitModifiedSig();
}

//-----------------------------------------------------------------------------

void SCommandHistory::undo()
{
    m_undoRedoManager.undo();
    this->emitModifiedSig();
}

//-----------------------------------------------------------------------------

void SCommandHistory::redo()
{
    m_undoRedoManager.redo();
    this->emitModifiedSig();
}

//-----------------------------------------------------------------------------

void SCommandHistory::clear()
{
    m_undoRedoManager.clear();
    this->emitModifiedSig();
}

//-----------------------------------------------------------------------------

void SCommandHistory::emitModifiedSig() const
{
    m_canUndoSig->async_emit(m_undoRedoManager.canUndo());
    m_canRedoSig->async_emit(m_undoRedoManager.canRedo());
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::history
