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
}

//-----------------------------------------------------------------------------

SCommandHistory::~SCommandHistory()
{
}

//-----------------------------------------------------------------------------

void SCommandHistory::configuring() throw( ::fwTools::Failed )
{
    ::fwServices::IService::ConfigType config = this->getConfigTree();

    auto maxCommands = config.get_optional< size_t >("service.maxCommands");
    auto maxMemory   = config.get_optional< size_t >("service.maxMemory");

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

void SCommandHistory::starting() throw( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void SCommandHistory::updating() throw( ::fwTools::Failed )
{
}

//-----------------------------------------------------------------------------

void SCommandHistory::stopping() throw( ::fwTools::Failed )
{
    m_undoRedoManager.clear();
}

//-----------------------------------------------------------------------------

void SCommandHistory::enqueue(fwCommand::ICommand::sptr command)
{
    m_undoRedoManager.enqueue(command);
}

//-----------------------------------------------------------------------------

void SCommandHistory::undo()
{
    m_undoRedoManager.undo();
}

//-----------------------------------------------------------------------------

void SCommandHistory::redo()
{
    m_undoRedoManager.redo();
}

//-----------------------------------------------------------------------------

void SCommandHistory::clear()
{
    m_undoRedoManager.clear();
}

//-----------------------------------------------------------------------------

} // namespace ctrlHistory
