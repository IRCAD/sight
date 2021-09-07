/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#include <data/mt/ObjectWriteLock.hpp>
#include <data/reflection/exception/ObjectNotFound.hpp>
#include <data/reflection/getObject.hpp>

#include <numeric>

namespace sight::module::ui::history
{

static const core::com::Signals::SignalKeyType s_CANUNDO_SIGNAL = "canUndo";
static const core::com::Signals::SignalKeyType s_CANREDO_SIGNAL = "canRedo";

static const core::com::Slots::SlotKeyType s_ENQUEUE_SLOT = "enqueue";
static const core::com::Slots::SlotKeyType s_UNDO_SLOT    = "undo";
static const core::com::Slots::SlotKeyType s_REDO_SLOT    = "redo";
static const core::com::Slots::SlotKeyType s_CLEAR_SLOT   = "clear";

//-----------------------------------------------------------------------------

SCommandHistory::SCommandHistory()
{
    newSlot(s_ENQUEUE_SLOT, &SCommandHistory::enqueue, this);
    newSlot(s_UNDO_SLOT, &SCommandHistory::undo, this);
    newSlot(s_REDO_SLOT, &SCommandHistory::redo, this);
    newSlot(s_CLEAR_SLOT, &SCommandHistory::clear, this);

    m_canUndoSig = newSignal<CanDoSignalType>(s_CANUNDO_SIGNAL);
    m_canRedoSig = newSignal<CanDoSignalType>(s_CANREDO_SIGNAL);
}

//-----------------------------------------------------------------------------

SCommandHistory::~SCommandHistory()
{
}

//-----------------------------------------------------------------------------

void SCommandHistory::configuring()
{
    service::IService::ConfigType config = this->getConfigTree();

    auto maxCommands = config.get_optional<size_t>("maxCommands");
    auto maxMemory   = config.get_optional<size_t>("maxMemory");

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
    this->emitModifSig();
}

//-----------------------------------------------------------------------------

void SCommandHistory::updating()
{
    this->emitModifSig();
}

//-----------------------------------------------------------------------------

void SCommandHistory::stopping()
{
    m_undoRedoManager.clear();
}

//-----------------------------------------------------------------------------

void SCommandHistory::enqueue(sight::ui::history::ICommand::sptr command)
{
    m_undoRedoManager.enqueue(command);
    this->emitModifSig();
}

//-----------------------------------------------------------------------------

void SCommandHistory::undo()
{
    m_undoRedoManager.undo();
    this->emitModifSig();
}

//-----------------------------------------------------------------------------

void SCommandHistory::redo()
{
    m_undoRedoManager.redo();
    this->emitModifSig();
}

//-----------------------------------------------------------------------------

void SCommandHistory::clear()
{
    m_undoRedoManager.clear();
    this->emitModifSig();
}

//-----------------------------------------------------------------------------

void SCommandHistory::emitModifSig() const
{
    m_canUndoSig->asyncEmit(m_undoRedoManager.canUndo());
    m_canRedoSig->asyncEmit(m_undoRedoManager.canRedo());
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui::history
