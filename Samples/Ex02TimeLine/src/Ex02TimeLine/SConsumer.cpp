/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "Ex02TimeLine/SConsumer.hpp"
#include "Ex02TimeLine/MessageTL.hpp"

#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>
#include <fwData/String.hpp>
#include <fwServices/macros.hpp>
#include <fwThread/Timer.hpp>
#include <fwTools/Object.hpp>

#include <functional>

fwServicesRegisterMacro( ::fwServices::IService, ::Ex02TimeLine::SConsumer, ::Ex02TimeLine::MessageTL );


namespace Ex02TimeLine
{

//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SConsumer::s_CONSUME_SLOT = "consume";

//------------------------------------------------------------------------------

SConsumer::SConsumer() throw()
{
    m_slotConsume = ::fwCom::newSlot(&SConsumer::consume, this);

    ::fwCom::HasSlots::m_slots(s_CONSUME_SLOT, m_slotConsume);
    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );

    m_period = 0;
}

//------------------------------------------------------------------------------

SConsumer::~SConsumer() throw()
{
}

//------------------------------------------------------------------------------

void SConsumer::starting() throw( ::fwTools::Failed )
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

void SConsumer::stopping() throw( ::fwTools::Failed )
{
    m_timer.reset();
}

//------------------------------------------------------------------------------

void SConsumer::updating() throw( ::fwTools::Failed )
{
    const ::Ex02TimeLine::MessageTL::csptr timeline = this->getObject< ::Ex02TimeLine::MessageTL >();

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

void SConsumer::swapping( ) throw( ::fwTools::Failed )
{

}

//------------------------------------------------------------------------------

void SConsumer::configuring() throw( ::fwTools::Failed )
{
    ::fwRuntime::ConfigurationElement::csptr idCfg = m_configuration->findConfigurationElement("id");
    SLM_ASSERT("Tag 'id' not found.", idCfg);

    const std::string id = idCfg->getValue();
    SLM_ASSERT("'id' is empty", !id.empty());

    m_receiverId = ::boost::lexical_cast<unsigned int>(id);

    const ::fwRuntime::ConfigurationElement::sptr periodCfg = m_configuration->findConfigurationElement("period");
    if(periodCfg)
    {
        const std::string period = periodCfg->getValue();
        m_period = ::boost::lexical_cast<unsigned int>(period);
    }

}

//------------------------------------------------------------------------------

void SConsumer::consume(fwCore::HiResClock::HiResClockType timestamp)
{
    ::Ex02TimeLine::MessageTL::sptr timeline = this->getObject< ::Ex02TimeLine::MessageTL >();

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
