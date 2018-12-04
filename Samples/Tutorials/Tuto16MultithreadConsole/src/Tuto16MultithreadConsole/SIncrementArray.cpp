/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
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

SIncrementArray::SIncrementArray() noexcept :
    m_periodInMillisec(500)
{
    newSlot(s_START_TIMER_SLOT, &SIncrementArray::startTimer, this);
}

//------------------------------------------------------------------------------

SIncrementArray::~SIncrementArray() noexcept
{
}

//------------------------------------------------------------------------------

void SIncrementArray::starting()
{
    m_timer = m_associatedWorker->createTimer();
    m_timer->setFunction( std::bind(&SIncrementArray::updating, this) );
    m_timer->setDuration( std::chrono::milliseconds(m_periodInMillisec) );
}

//------------------------------------------------------------------------------

void SIncrementArray::stopping()
{
    if (m_timer->isRunning())
    {
        m_timer->stop();
    }
    m_timer.reset();
}

//------------------------------------------------------------------------------

void SIncrementArray::updating()
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

void SIncrementArray::configuring()
{

}

//------------------------------------------------------------------------------

void SIncrementArray::startTimer()
{
    m_timer->start();
}

//------------------------------------------------------------------------------

} // namespace Tuto16MultithreadConsole
