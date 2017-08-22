/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "Ex02TimeLine/SProducer.hpp"

#include "Ex02TimeLine/MessageTL.hpp"

#include <fwCom/Signal.hxx>

#include <fwServices/macros.hpp>

#include <fwThread/Timer.hpp>

#include <functional>

fwServicesRegisterMacro( ::fwServices::IService, ::Ex02TimeLine::SProducer );

namespace Ex02TimeLine
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
    ::fwServices::IService::ConfigType config = this->getConfigTree().get_child("service");

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
    m_timer->setDuration( ::boost::chrono::milliseconds( m_period ) );

    m_timer->start();

    // Init timeline pool
    if(m_timelineSize)
    {
        ::Ex02TimeLine::MessageTL::sptr timeline = this->getInOut< ::Ex02TimeLine::MessageTL >("timeline");

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
    ::Ex02TimeLine::MessageTL::sptr timeline = this->getInOut< ::Ex02TimeLine::MessageTL >("timeline");

    const ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();
    SPTR(::Ex02TimeLine::MessageTL::BufferType) buffer = timeline->createBuffer(timestamp);

    ::Ex02TimeLine::MsgData* data = buffer->addElement(0);

    const std::string message = m_message + " #" + ::boost::lexical_cast<std::string>( m_msgCount++ );

    data->uidSender = m_senderId;
    strncpy(data->szMsg, message.c_str(), ::Ex02TimeLine::MsgData::MAX_MSG_SIZE);

    timeline->pushObject(buffer);

    //Notify
    ::arData::TimeLine::ObjectPushedSignalType::sptr sig;
    sig = timeline->signal< ::arData::TimeLine::ObjectPushedSignalType >(::arData::TimeLine::s_OBJECT_PUSHED_SIG );
    sig->asyncEmit(timestamp);
}

//------------------------------------------------------------------------------

} // namespace Ex02TimeLine
