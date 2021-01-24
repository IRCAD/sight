/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "Tuto05MultithreadConsoleCpp/SIncrementArray.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>
#include <core/thread/Timer.hpp>

#include <data/Array.hpp>
#include <data/mt/ObjectWriteLock.hpp>

#include <fwServices/macros.hpp>

#include <functional>

fwServicesRegisterMacro( ::fwServices::IController, ::Tuto05MultithreadConsoleCpp::SIncrementArray, data::Array )

namespace Tuto05MultithreadConsoleCpp
{

static const core::com::Slots::SlotKeyType s_START_TIMER_SLOT = "startTimer";

static const std::string s_ARRAY_INOUT = "array";

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

void SIncrementArray::configuring()
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

void SIncrementArray::updating()
{
    const auto array = this->getInOut< data::Array >(s_ARRAY_INOUT);
    SLM_ASSERT("Bad number of dimensions", array->getNumberOfDimensions() == 1 );

    const auto dumpLock = array->lock();

    auto itr       = array->begin< unsigned int >();
    const auto end = array->end< unsigned int >();

    // Increment the array values.
    for (; itr != end; ++itr)
    {
        ++(*itr);
    }

    // Notify that the array is modified.
    const auto sig = array->signal< data::Object::ModifiedSignalType>( data::Object::s_MODIFIED_SIG );
    {
        core::com::Connection::Blocker blockUpdate(sig->getConnection(m_slotUpdate));
        core::com::Connection::Blocker blockTimer(sig->getConnection(this->slot(s_START_TIMER_SLOT)));
        sig->asyncEmit();
    }

}

//------------------------------------------------------------------------------

void SIncrementArray::stopping()
{
    if(m_timer->isRunning())
    {
        m_timer->stop();
    }
    m_timer.reset();
}

//------------------------------------------------------------------------------

void SIncrementArray::startTimer()
{
    m_timer->start();
}

//------------------------------------------------------------------------------

} // namespace Tuto05MultithreadConsoleCpp.
