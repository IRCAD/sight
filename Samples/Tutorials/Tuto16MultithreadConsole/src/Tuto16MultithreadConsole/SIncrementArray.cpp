/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "Tuto16MultithreadConsole/SIncrementArray.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Array.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <fwServices/macros.hpp>

#include <fwThread/Timer.hpp>

#include <functional>

fwServicesRegisterMacro( ::fwServices::IController, ::Tuto16MultithreadConsole::SIncrementArray, ::fwData::Array );

namespace Tuto16MultithreadConsole
{

static const ::fwCom::Slots::SlotKeyType s_START_TIMER_SLOT = "startTimer";

//------------------------------------------------------------------------------

SIncrementArray::SIncrementArray() throw() :
    m_periodInMillisec(500)
{
    newSlot(s_START_TIMER_SLOT, &SIncrementArray::startTimer, this);
}

//------------------------------------------------------------------------------

SIncrementArray::~SIncrementArray() throw()
{
}

//------------------------------------------------------------------------------

void SIncrementArray::starting() throw( ::fwTools::Failed )
{
    m_timer = m_associatedWorker->createTimer();
    m_timer->setFunction( std::bind(&SIncrementArray::updating, this) );
    m_timer->setDuration( ::boost::chrono::milliseconds(m_periodInMillisec) );
}

//------------------------------------------------------------------------------

void SIncrementArray::stopping() throw( ::fwTools::Failed )
{
    if (m_timer->isRunning())
    {
        m_timer->stop();
    }
    m_timer.reset();
}

//------------------------------------------------------------------------------

void SIncrementArray::updating() throw( ::fwTools::Failed )
{
    ::fwData::Array::sptr array = this->getInOut< ::fwData::Array >("array");
    ::fwData::mt::ObjectWriteLock writeLock(array);

    SLM_ASSERT("No array.", array);
    SLM_ASSERT("Array : bad number of dimensions.", array->getNumberOfDimensions() == 1 );

    const size_t arraySize = array->getSize()[0];

    ::fwDataTools::helper::Array arrayHelper(array);

    unsigned int* buffer = static_cast< unsigned int* >( arrayHelper.getBuffer() );

    // Increment the array values
    for (size_t i = 0; i < arraySize; i++)
    {
        ++buffer[i];
    }

    // Notify that the array is modified
    ::fwData::Object::ModifiedSignalType::sptr sig
        = array->signal< ::fwData::Object::ModifiedSignalType>( ::fwData::Object::s_MODIFIED_SIG );
    {
        ::fwCom::Connection::Blocker blockUpdate(sig->getConnection(m_slotUpdate));
        ::fwCom::Connection::Blocker blockTimer(sig->getConnection(this->slot(s_START_TIMER_SLOT)));
        sig->asyncEmit();
    }

}

//------------------------------------------------------------------------------

void SIncrementArray::configuring() throw( ::fwTools::Failed )
{

}

//------------------------------------------------------------------------------

void SIncrementArray::startTimer()
{
    m_timer->start();
}

//------------------------------------------------------------------------------

} // namespace Tuto16MultithreadConsole
