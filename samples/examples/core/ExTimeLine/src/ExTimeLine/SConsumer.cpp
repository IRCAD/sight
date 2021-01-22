/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "ExTimeLine/SConsumer.hpp"

#include <fwCom/Slots.hxx>

#include <fwServices/macros.hpp>

#include <fwThread/Timer.hpp>

#include <exTimeLineData/MessageTL.hpp>

#include <functional>

fwServicesRegisterMacro( ::fwServices::IService, ::ExTimeLine::SConsumer )

namespace ExTimeLine
{

//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SConsumer::s_CONSUME_SLOT = "consume";

//------------------------------------------------------------------------------

SConsumer::SConsumer() noexcept :
    m_period(0)
{
    newSlot(s_CONSUME_SLOT, &SConsumer::consume, this);
}

//------------------------------------------------------------------------------

SConsumer::~SConsumer() noexcept
{
}

//------------------------------------------------------------------------------

void SConsumer::configuring()
{
    ::fwServices::IService::ConfigType config = this->getConfigTree();

    m_receiverId = config.get<unsigned int>("id");
    m_period     = config.get<unsigned int>("period", 0);
}

//------------------------------------------------------------------------------

void SConsumer::starting()
{

    if(m_period)
    {
        m_timer = m_associatedWorker->createTimer();
        m_timer->setFunction( std::bind(&SConsumer::updating, this) );
        m_timer->setDuration( std::chrono::milliseconds( m_period ) );
        m_timer->start();
    }
}

//------------------------------------------------------------------------------

void SConsumer::stopping()
{
    m_timer.reset();
}

//------------------------------------------------------------------------------

void SConsumer::updating()
{
    const auto timeline = this->getLockedInput< ::exTimeLineData::MessageTL >("timeline");

    const core::HiResClock::HiResClockType timestamp = core::HiResClock::getTimeInMilliSec();
    const CSPTR(::exTimeLineData::MessageTL::BufferType) buffer = timeline->getClosestBuffer(timestamp);

    if(buffer)
    {
        const ::exTimeLineData::MsgData& element = buffer->getElement(0);

        std::cout << "Message received (timer): CONSUMER: " << m_receiverId << " SENDER: " << element.uidSender <<
            " MESSAGE: \"" << element.szMsg << "\"" << std::endl;
    }
}

//------------------------------------------------------------------------------

void SConsumer::consume(core::HiResClock::HiResClockType timestamp)
{
    const auto timeline = this->getLockedInput< ::exTimeLineData::MessageTL >("timeline");

    const CSPTR(::exTimeLineData::MessageTL::BufferType) buffer = timeline->getClosestBuffer(timestamp);

    if(buffer)
    {
        const ::exTimeLineData::MsgData& element = buffer->getElement(0);

        std::cout << "Message received (slot) : CONSUMER: " << m_receiverId << " SENDER: " << element.uidSender <<
            " MESSAGE: \"" << element.szMsg << "\"" << std::endl;
    }
}

//------------------------------------------------------------------------------

} // namespace ExTimeLine
