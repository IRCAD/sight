/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "PoC07TimeLine/SProducer.hpp"
#include "PoC07TimeLine/MessageTL.hpp"

#include <fwCom/Signal.hxx>
#include <fwServices/macros.hpp>
#include <fwThread/Timer.hpp>
#include <fwTools/Object.hpp>

fwServicesRegisterMacro( ::fwServices::IService, ::PoC07TimeLine::SProducer, ::PoC07TimeLine::MessageTL );

namespace PoC07TimeLine
{

//------------------------------------------------------------------------------

SProducer::SProducer() throw()
{
    m_msgCount     = 0;
    m_timelineSize = 0;
}

//------------------------------------------------------------------------------

SProducer::~SProducer() throw()
{
}

//------------------------------------------------------------------------------

void SProducer::starting() throw( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    m_timer = m_associatedWorker->createTimer();
    m_timer->setFunction( ::boost::bind(&SProducer::updating, this) );
    m_timer->setDuration( ::boost::chrono::milliseconds( m_period ) );

    m_timer->start();

    // Init timeline pool
    if(m_timelineSize)
    {
        ::PoC07TimeLine::MessageTL::sptr timeline = this->getObject< ::PoC07TimeLine::MessageTL >();

        // This wouldn't hurt to initialize the timeline several times since it will be erased each time
        // but this would be a mess to know who is the last to initialize
        SLM_ASSERT("Timeline initialized twice", timeline->getMaxElementNum() == ~0u);

        timeline->setMaximumSize(m_timelineSize);

        // We only have one element per buffer in this simple example
        timeline->initPoolSize(1);
    }
}

//------------------------------------------------------------------------------

void SProducer::stopping() throw( ::fwTools::Failed )
{
    m_timer.reset();
}

//------------------------------------------------------------------------------

void SProducer::updating() throw( ::fwTools::Failed )
{
    ::PoC07TimeLine::MessageTL::sptr timeline = this->getObject< ::PoC07TimeLine::MessageTL >();

    const ::fwCore::HiResClock::HiResClockType timestamp = ::fwCore::HiResClock::getTimeInMilliSec();
    SPTR(::PoC07TimeLine::MessageTL::BufferType) buffer = timeline->createBuffer(timestamp);

    ::PoC07TimeLine::MsgData* data = buffer->addElement(0);

    const std::string message = m_message + " #" + ::boost::lexical_cast<std::string>( m_msgCount++ );

    data->uidSender = m_senderId;
    strncpy(data->szMsg, message.c_str(), ::PoC07TimeLine::MsgData::MAX_MSG_SIZE);

    timeline->pushObject(buffer);

    //Notify
    ::extData::TimeLine::ObjectPushedSignalType::sptr sig;
    sig = timeline->signal< ::extData::TimeLine::ObjectPushedSignalType >(::extData::TimeLine::s_OBJECT_PUSHED_SIG );
    sig->asyncEmit(timestamp);
}

//------------------------------------------------------------------------------

void SProducer::configuring() throw( ::fwTools::Failed )
{
    const ::fwRuntime::ConfigurationElement::csptr msgCfg = m_configuration->findConfigurationElement("message");
    SLM_ASSERT("Tag 'message' not found.", msgCfg);

    m_message = msgCfg->getValue();
    SLM_ASSERT("'message' is empty", !m_message.empty());

    const ::fwRuntime::ConfigurationElement::csptr idCfg = m_configuration->findConfigurationElement("id");
    SLM_ASSERT("Tag 'id' not found.", idCfg);

    const std::string id = idCfg->getValue();
    SLM_ASSERT("'id' is empty", !id.empty());

    m_senderId = ::boost::lexical_cast<unsigned int>(id);

    const ::fwRuntime::ConfigurationElement::csptr periodCfg = m_configuration->findConfigurationElement("period");
    SLM_ASSERT("Tag 'period' not found.", periodCfg);

    const std::string period = periodCfg->getValue();
    SLM_ASSERT("'period' is empty", !id.empty());

    m_period = ::boost::lexical_cast<unsigned int>(period);

    const ::fwRuntime::ConfigurationElement::csptr timelineCfg = m_configuration->findConfigurationElement(
        "timelineSize");
    if(timelineCfg)
    {
        const std::string timeline = timelineCfg->getValue();
        SLM_ASSERT("'timelineSize' is empty", !id.empty());

        m_timelineSize = ::boost::lexical_cast<unsigned int>(timeline);
    }
}

//------------------------------------------------------------------------------

void SProducer::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{
}

//------------------------------------------------------------------------------

void SProducer::swapping( ) throw( ::fwTools::Failed )
{

}

//------------------------------------------------------------------------------

} // namespace PoC07TimeLine
