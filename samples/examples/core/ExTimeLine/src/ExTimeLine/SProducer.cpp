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

#include "ExTimeLine/SProducer.hpp"

#include "exTimeLineData/MessageTL.hpp"

#include <core/com/Signal.hxx>
#include <core/thread/Timer.hpp>

#include <services/macros.hpp>

#include <functional>

fwServicesRegisterMacro( ::sight::services::IService, ::ExTimeLine::SProducer )

namespace ExTimeLine
{

//------------------------------------------------------------------------------

SProducer::SProducer() noexcept :
    m_senderId(0),
    m_period(0),
    m_msgCount(0),
    m_timelineSize(0)
{
}

//------------------------------------------------------------------------------

SProducer::~SProducer() noexcept
{
}

//------------------------------------------------------------------------------

void SProducer::configuring()
{
    services::IService::ConfigType config = this->getConfigTree();

    m_message      = config.get<std::string>("message");
    m_senderId     = config.get<unsigned int>("id");
    m_period       = config.get<unsigned int>("period", 0);
    m_timelineSize = config.get<unsigned int>("timelineSize", 0);
}

//------------------------------------------------------------------------------

void SProducer::starting()
{
    m_timer = m_associatedWorker->createTimer();
    m_timer->setFunction( std::bind(&SProducer::updating, this) );
    m_timer->setDuration( std::chrono::milliseconds( m_period ) );

    m_timer->start();

    // Init timeline pool
    if(m_timelineSize)
    {
        const auto timeline = this->getLockedInOut< ::exTimeLineData::MessageTL >("timeline");

        // This wouldn't hurt to initialize the timeline several times since it will be erased each time
        // but this would be a mess to know who is the last to initialize
        SLM_ASSERT("Timeline initialized twice", timeline->getMaxElementNum() == ~0u);

        timeline->setMaximumSize(m_timelineSize);

        // We only have one element per buffer in this simple example
        timeline->initPoolSize(1);
    }
}

//------------------------------------------------------------------------------

void SProducer::stopping()
{
    m_timer.reset();
}

//------------------------------------------------------------------------------

void SProducer::updating()
{
    ::exTimeLineData::MessageTL::sptr timeline = this->getInOut< ::exTimeLineData::MessageTL >("timeline");

    const core::HiResClock::HiResClockType timestamp = core::HiResClock::getTimeInMilliSec();
    SPTR(::exTimeLineData::MessageTL::BufferType) buffer = timeline->createBuffer(timestamp);

    ::exTimeLineData::MsgData* data = buffer->addElement(0);

    const std::string message = m_message + " #" + std::to_string( m_msgCount++ );

    data->uidSender = m_senderId;
    strncpy(data->szMsg, message.c_str(), ::exTimeLineData::MsgData::MAX_MSG_SIZE);

    timeline->pushObject(buffer);

    //Notify
    data::TimeLine::ObjectPushedSignalType::sptr sig;
    sig = timeline->signal< data::TimeLine::ObjectPushedSignalType >(data::TimeLine::s_OBJECT_PUSHED_SIG );
    sig->asyncEmit(timestamp);
}

//------------------------------------------------------------------------------

} // namespace ExTimeLine
