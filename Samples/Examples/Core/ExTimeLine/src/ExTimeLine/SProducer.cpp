/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ExTimeLine/SProducer.hpp"

#include "ExTimeLine/MessageTL.hpp"

#include <fwCom/Signal.hxx>

#include <fwServices/macros.hpp>

#include <fwThread/Timer.hpp>

#include <functional>

fwServicesRegisterMacro( ::fwServices::IService, ::ExTimeLine::SProducer );

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
    ::fwServices::IService::ConfigType config = this->getConfigTree();

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
        ::ExTimeLine::MessageTL::sptr timeline = this->getInOut< ::ExTimeLine::MessageTL >("timeline");

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
    ::ExTimeLine::MessageTL::sptr timeline = this->getInOut< ::ExTimeLine::MessageTL >("timeline");

    const ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();
    SPTR(::ExTimeLine::MessageTL::BufferType) buffer = timeline->createBuffer(timestamp);

    ::ExTimeLine::MsgData* data = buffer->addElement(0);

    const std::string message = m_message + " #" + std::to_string( m_msgCount++ );

    data->uidSender = m_senderId;
    strncpy(data->szMsg, message.c_str(), ::ExTimeLine::MsgData::MAX_MSG_SIZE);

    timeline->pushObject(buffer);

    //Notify
    ::arData::TimeLine::ObjectPushedSignalType::sptr sig;
    sig = timeline->signal< ::arData::TimeLine::ObjectPushedSignalType >(::arData::TimeLine::s_OBJECT_PUSHED_SIG );
    sig->asyncEmit(timestamp);
}

//------------------------------------------------------------------------------

} // namespace ExTimeLine
