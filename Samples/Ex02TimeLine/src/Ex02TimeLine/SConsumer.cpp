/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "Ex02TimeLine/SConsumer.hpp"

#include "Ex02TimeLine/MessageTL.hpp"

#include <fwCom/Slots.hxx>

#include <fwServices/macros.hpp>

#include <fwThread/Timer.hpp>

#include <functional>

fwServicesRegisterMacro( ::fwServices::IService, ::Ex02TimeLine::SConsumer );

namespace Ex02TimeLine
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
    SLM_TRACE_FUNC();

    if(m_period)
    {
        m_timer = m_associatedWorker->createTimer();
        m_timer->setFunction( std::bind(&SConsumer::updating, this) );
        m_timer->setDuration( ::boost::chrono::milliseconds( m_period ) );
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
    ::Ex02TimeLine::MessageTL::csptr timeline = this->getInput< ::Ex02TimeLine::MessageTL >("timeline");

    const ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();
    const CSPTR(::Ex02TimeLine::MessageTL::BufferType) buffer = timeline->getClosestBuffer(timestamp);

    if(buffer)
    {
        const ::Ex02TimeLine::MsgData& element = buffer->getElement(0);

        std::cout << "Message received (timer): CONSUMER: " << m_receiverId << " SENDER: " << element.uidSender <<
            " MESSAGE: \"" << element.szMsg << "\"" << std::endl;
    }
}

//------------------------------------------------------------------------------

void SConsumer::consume(fwCore::HiResClock::HiResClockType timestamp)
{
    ::Ex02TimeLine::MessageTL::csptr timeline = this->getInput< ::Ex02TimeLine::MessageTL >("timeline");

    const CSPTR(::Ex02TimeLine::MessageTL::BufferType) buffer = timeline->getClosestBuffer(timestamp);

    if(buffer)
    {
        const ::Ex02TimeLine::MsgData& element = buffer->getElement(0);

        std::cout << "Message received (slot) : CONSUMER: " << m_receiverId << " SENDER: " << element.uidSender <<
            " MESSAGE: \"" << element.szMsg << "\"" << std::endl;
    }
}

//------------------------------------------------------------------------------

} // namespace Ex02TimeLine
