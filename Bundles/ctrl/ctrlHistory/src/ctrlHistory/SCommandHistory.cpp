/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlHistory/SCommandHistory.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataCamp/exception/ObjectNotFound.hpp>
#include <fwDataCamp/getObject.hpp>

#include <numeric>

namespace ctrlHistory
{

fwServicesRegisterMacro(::fwServices::IService, ::ctrlHistory::SCommandHistory, ::fwData::Object);

static const ::fwCom::Signals::SignalKeyType s_CANUNDO_SIGNAL = "canUndo";
static const ::fwCom::Signals::SignalKeyType s_CANREDO_SIGNAL = "canRedo";

static const ::fwCom::Slots::SlotKeyType s_ENQUEUE_SLOT = "enqueue";
static const ::fwCom::Slots::SlotKeyType s_UNDO_SLOT    = "undo";
static const ::fwCom::Slots::SlotKeyType s_REDO_SLOT    = "redo";
static const ::fwCom::Slots::SlotKeyType s_CLEAR_SLOT   = "clear";

//-----------------------------------------------------------------------------

SCommandHistory::SCommandHistory()
{
    newSlot(s_ENQUEUE_SLOT, &SCommandHistory::enqueue, this);
    newSlot(s_UNDO_SLOT, &SCommandHistory::undo, this);
    newSlot(s_REDO_SLOT, &SCommandHistory::redo, this);
    newSlot(s_CLEAR_SLOT, &SCommandHistory::clear, this);

    m_canUndoSig = newSignal< CanDoSignalType >( s_CANUNDO_SIGNAL );
    m_canRedoSig = newSignal< CanDoSignalType >( s_CANREDO_SIGNAL );
}

//-----------------------------------------------------------------------------

SCommandHistory::~SCommandHistory()
{
}

//-----------------------------------------------------------------------------

void SCommandHistory::configuring()
{
    ::fwServices::IService::ConfigType config = this->getConfigTree();

    auto maxCommands = config.get_optional< size_t >("maxCommands");
    auto maxMemory   = config.get_optional< size_t >("maxMemory");

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

void SCommandHistory::enqueue(fwCommand::ICommand::sptr command)
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

} // namespace ctrlHistory
