/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <algorithm>
#include <iterator>
#include <limits>
#include <stdexcept>

#include <fwTools/ClassRegistrar.hpp>

#include "fwCommand/UndoRedoManager.hpp"
#include "fwCommand/Empty.hpp"

namespace fwCommand
{

//-----------------------------------------------------------------------------

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwCommand::UndoRedoManager, ::fwCommand::UndoRedoManager );

//-----------------------------------------------------------------------------

UndoRedoManager::sptr UndoRedoManager::getDefault()
{
    SLM_TRACE_FUNC();
    static UndoRedoManager::sptr m_instance = UndoRedoManager::New();
    return m_instance;
}

//-----------------------------------------------------------------------------

UndoRedoManager::UndoRedoManager()
{}

//-----------------------------------------------------------------------------

UndoRedoManager::~UndoRedoManager() throw()
{}

//-----------------------------------------------------------------------------

void UndoRedoManager::queue( ICommand::sptr pCmd, const bool execute )
{
    m_currentManager->queue(pCmd,execute);
    m_sig("QUEUE_EVENT");
}

//-----------------------------------------------------------------------------

void UndoRedoManager::forward()
{
    m_currentManager->forward();
    m_sig("FORWARD_EVENT");
}

//-----------------------------------------------------------------------------

void UndoRedoManager::backward()
{
    m_currentManager->backward();
    m_sig("BACKWARD_EVENT");
}

//-----------------------------------------------------------------------------

void UndoRedoManager::clear()
{
    m_currentManager->clear();
    m_sig("CLEAR_EVENT");
}

//-----------------------------------------------------------------------------

const ::boost::uint32_t UndoRedoManager::getUndoSize()
{
    return m_currentManager ? m_currentManager->getUndoSize() : 0;
}

//-----------------------------------------------------------------------------

const ::boost::uint32_t UndoRedoManager::getRedoSize()
{
    return m_currentManager ? m_currentManager->getRedoSize() : 0;
}

//-----------------------------------------------------------------------------

ICommand::sptr UndoRedoManager::getFirstUndoableCommand()
{
    return m_currentManager->getFirstUndoableCommand();
}

//-----------------------------------------------------------------------------

ICommand::sptr UndoRedoManager::getFirstRedoableCommand()
{
    return m_currentManager->getFirstRedoableCommand();
}

//-----------------------------------------------------------------------------

void UndoRedoManager::setManager( Manager::sptr currentManager )
{
    m_currentManager = currentManager;
}

//-----------------------------------------------------------------------------

void UndoRedoManager::removeManager()
{
    m_currentManager.reset();
}

//-----------------------------------------------------------------------------

const ::boost::uint32_t UndoRedoManager::getMaxUndoLevel()
{
    return m_currentManager->getMaxUndoLevel();
}

//-----------------------------------------------------------------------------

const ::boost::uint32_t UndoRedoManager::getMaxUndoMemory()
{
    return m_currentManager->getMaxUndoMemory();
}

//-----------------------------------------------------------------------------

const ::boost::uint32_t UndoRedoManager::getMaxCommandMemory()
{
    return m_currentManager->getMaxCommandMemory();
}

//-----------------------------------------------------------------------------

UndoRedoManager::ConnectionType UndoRedoManager::connect(SignalType::slot_function_type subscriber)
{
    return m_sig.connect(subscriber);
}

//-----------------------------------------------------------------------------

void UndoRedoManager::disconnect(UndoRedoManager::ConnectionType subscriber)
{
    subscriber.disconnect();
}

//-----------------------------------------------------------------------------

} // namespace fwCommand
