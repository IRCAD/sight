/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
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

#include "SConsumer.hpp"

#include <core/com/Slots.hxx>
#include <core/thread/Timer.hpp>

#include <service/macros.hpp>

#include <functional>

namespace ExTimeLine
{

//------------------------------------------------------------------------------

const sight::core::com::Slots::SlotKeyType SConsumer::s_CONSUME_SLOT = "consume";

//------------------------------------------------------------------------------

SConsumer::SConsumer() noexcept
{
    newSlot(s_CONSUME_SLOT, &SConsumer::consume, this);
}

//------------------------------------------------------------------------------

SConsumer::~SConsumer() noexcept =
    default;

//------------------------------------------------------------------------------

void SConsumer::configuring()
{
    sight::service::IService::ConfigType config = this->getConfiguration();

    m_receiverId = config.get<unsigned int>("id");
    m_period     = config.get<unsigned int>("period", 0);
}

//------------------------------------------------------------------------------

void SConsumer::starting()
{
    if(m_period != 0U)
    {
        m_timer = this->worker()->createTimer();
        m_timer->setFunction([this](auto&& ...){updating();});
        m_timer->setDuration(std::chrono::milliseconds(m_period));
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
    const auto timeline = m_timeline.lock();

    const auto timestamp = sight::core::HiResClock::getTimeInMilliSec();
    const CSPTR(::ExTimeLine::MessageTL::BufferType) buffer = timeline->getClosestBuffer(timestamp);

    if(buffer)
    {
        const ::ExTimeLine::MsgData& element = buffer->getElement(0);

        std::cout << "Message received (timer): CONSUMER: " << m_receiverId << " SENDER: " << element.uidSender
        << " MESSAGE: \"" << element.szMsg.data() << "\"" << std::endl;
    }
}

//------------------------------------------------------------------------------

void SConsumer::consume(sight::core::HiResClock::HiResClockType timestamp)
{
    const auto timeline = m_timeline.lock();

    const CSPTR(::ExTimeLine::MessageTL::BufferType) buffer = timeline->getClosestBuffer(timestamp);

    if(buffer)
    {
        const ::ExTimeLine::MsgData& element = buffer->getElement(0);

        std::cout << "Message received (slot) : CONSUMER: " << m_receiverId << " SENDER: " << element.uidSender
        << " MESSAGE: \"" << element.szMsg.data() << "\"" << std::endl;
    }
}

//------------------------------------------------------------------------------

} // namespace ExTimeLine
