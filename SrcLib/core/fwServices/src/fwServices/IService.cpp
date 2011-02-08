/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwTools/fwID.hpp>

#include <fwRuntime/EConfigurationElement.hpp>

#include "fwServices/IService.hpp"
#include "fwServices/IEditionService.hpp"
#include "fwServices/ComChannelService.hpp"
#include "fwServices/ObjectServiceRegistry.hpp"
#include "fwServices/helper.hpp"
#include "fwServices/Factory.hpp"
#include "fwServices/validation/Validator.hpp"


namespace fwServices
{

//-----------------------------------------------------------------------------

IService::IService() :
    m_globalState ( STOPPED ),
    m_updatingState ( NOTUPDATING ),
    m_notificationState ( IDLE ),
    m_configurationState ( UNCONFIGURED ),
    m_isHandlingAllEvents ( true ),
    m_configuration ( new ::fwRuntime::EConfigurationElement("EmptyConfigurationElement") )
{
    // by default a weak_ptr have a use_count == 0
    m_msgDeque.clear();
}

//-----------------------------------------------------------------------------

IService::~IService()
{}

//-----------------------------------------------------------------------------

void IService::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

::fwTools::Object::sptr IService::getObject()
{
    SLM_ASSERT("Empty shared_ptr for Associated Object", m_associatedObject.use_count() ); // initialized
    SLM_ASSERT("Associated Object is expired", m_associatedObject.expired() == false ); // not expired
    return m_associatedObject.lock();
}

//-----------------------------------------------------------------------------

void IService::setConfiguration(const ::fwRuntime::ConfigurationElement::sptr _cfgElement)
{
    SLM_ASSERT( "Invalid ConfigurationElement", _cfgElement ) ;
    m_configuration = _cfgElement ;
    m_configurationState = UNCONFIGURED ;
}

//-----------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::sptr IService::getConfiguration()
{
    return m_configuration ;
}

//-----------------------------------------------------------------------------

bool IService::checkConfiguration()
{
    ::std::stringstream stream ;
    bool checkingResult = ::fwServices::validation::checkService( this->className() , this->m_configuration , stream ) ;
    OSLM_TRACE("Configuration checking result for" << this->className() << " : " << stream.str() ) ;
    return checkingResult ;
}

//-----------------------------------------------------------------------------

void IService::configure()
{
    SLM_ASSERT( "Configuration is not correct", this->checkConfiguration() );
    if( m_configurationState == UNCONFIGURED )
    {
        m_configurationState = CONFIGURING ;
        if( m_globalState == STOPPED )
        {
            this->configuring();
        }
        else if( m_globalState == STARTED )
        {
            this->reconfiguring();
        }
        m_configurationState = CONFIGURED ;
    }
}

//-----------------------------------------------------------------------------

void IService::reconfiguring() throw ( ::fwTools::Failed )
{
    OSLM_FATAL("If this method (reconfiguring) is called, it must be overrided in the implementation ("<<this->getClassname()<<", "<< this->getID() <<")" );
}

//-----------------------------------------------------------------------------

void IService::start() throw( ::fwTools::Failed)
{
    if( m_globalState == STOPPED )
    {
        m_globalState = STARTING ;
        this->starting() ;
        m_globalState = STARTED ;
    }

    OSLM_WARN_IF( "INVOKING START WHILE ALREADY STARTED (on this = " << this->className() << ")", m_globalState != STOPPED);
}

//-----------------------------------------------------------------------------

void IService::stop() throw( ::fwTools::Failed)
{
    if( m_globalState == STARTED )
    {
        m_globalState = STOPPING ;
        this->stopping() ;
        m_globalState = STOPPED ;
    }

    OSLM_WARN_IF( "INVOKING STOP WHILE ALREADY STOPPED (on this = " << this->className() << ")", m_globalState != STARTED);
}

//-----------------------------------------------------------------------------

void IService::update( ::fwServices::ObjectMsg::csptr _msg )
{
    OSLM_ASSERT("INVOKING update(msg) WHILE ALREADY STOPPED ("<<m_globalState<<") on this = " << this->className(), m_globalState == STARTED );

    if(m_notificationState == SENDING_MSG /* state during the service notifies to listeners a message */
            || m_notificationState == RECEIVING_WITH_SENDING_MSG /*  state during the service notifies to listeners a message when it is already receiving a message */
            || m_notificationState == RECEIVING_MSG ) /* state during the service receives a message and analyzed it */
    {
        m_msgDeque.push_back(_msg);
        OSLM_TRACE("m_msgDeque size: " << m_msgDeque.size());
    }
    else /* IDLE: state when service do nothing */
    {
        OSLM_TRACE("Service" << this->className() <<" is on IDLE state ==> treatment of message: " << _msg->getGeneralInfo());
        m_notificationState = RECEIVING_MSG ;
        this->updating( _msg ) ;
        m_notificationState = IDLE ;
        this->processingPendingMessages();
    }

    //  OSLM_ASSERT("INVOKING update(msg) WHILE NOT IDLED ("<<m_notificationState<<") on this = " << this->className(), m_notificationState == IDLE );
    //  OSLM_ASSERT("simple loop detection on "<< this->getSptr()->getClassname(), this->getSptr() !=  _msg->getSource().lock());
}

//-----------------------------------------------------------------------------

void IService::handlingEventOff()
{
    SLM_ASSERT( "Handling event vector must be empty", m_handledEvents.size() == 0 );
    m_isHandlingAllEvents = false;
    m_handledEvents.push_back( "TOTO" ); //HACK FIXME
}

//-----------------------------------------------------------------------------

void IService::addNewHandledEvent( std::string _eventId )
{
    m_isHandlingAllEvents = false;
    m_handledEvents.push_back( _eventId );
}

//-----------------------------------------------------------------------------

std::vector< std::string > IService::getHandledEvents()
{
    SLM_ASSERT( "ACH : This service handles all messages, why you use getHandledEvents() ? test isHandlingAllEvents() before.", !m_isHandlingAllEvents );
    return m_handledEvents;
}

//-----------------------------------------------------------------------------

bool IService::isHandlingAllEvents()
{
    return m_isHandlingAllEvents;
}

//-----------------------------------------------------------------------------

void IService::update() throw( ::fwTools::Failed)
{
    OSLM_ASSERT("INVOKING update WHILE ALREADY STOPPED ("<<m_globalState<<") on this = " << this->className(), m_globalState == STARTED );
    OSLM_ASSERT("INVOKING update WHILE NOT IDLED ("<<m_updatingState<<") on this = " << this->className(), m_updatingState == NOTUPDATING );
    OSLM_ASSERT("INVOKING update WHILE NOT IDLED ("<<m_notificationState<<") on this = " << this->className(), !this->isSending() );

    m_updatingState = UPDATING ;
    this->updating( ) ;
    m_updatingState = NOTUPDATING ;

}

//-----------------------------------------------------------------------------

void IService::swap( ::fwTools::Object::sptr _obj ) throw(::fwTools::Failed)
{
    OSLM_ASSERT("Swapping on "<< this->getID() << " with same Object " << _obj->getID(), m_associatedObject.lock() != _obj );

    if( m_globalState == STARTED ) // FIXME ???
    {
        m_globalState = SWAPPING ;

        ::fwServices::IEditionService::sptr oldEditor = ::fwServices::get< ::fwServices::IEditionService >( m_associatedObject.lock())  ;
        ::fwServices::IEditionService::sptr newEditor = ::fwServices::get< ::fwServices::IEditionService >( _obj ) ;
        typedef std::vector< ::fwServices::ComChannelService::sptr > OContainerType;
        OContainerType obs = ::fwServices::OSR::getServices< ::fwServices::ComChannelService >() ;
        BOOST_FOREACH(::fwServices::ComChannelService::sptr comChannel, obs)
        {
            /// Check if _service is the subject (IEditionService) or the destination service
            if( comChannel->getDest() == this->getSptr() && comChannel->getSrc() == oldEditor )
            {
                comChannel->stop() ;
                comChannel->setSrc(newEditor);
                ::fwServices::OSR::swapService(m_associatedObject.lock(), _obj , comChannel );
                comChannel->start();
            }
        }

        ::fwServices::OSR::swapService(m_associatedObject.lock(), _obj , this->getSptr() );

        this->swapping();

        m_globalState = STARTED ;
    }

    OSLM_WARN_IF( "Service "<< this->getID() << " is not STARTED, no swapping with Object " << _obj->getID(), m_globalState != STARTED);
}

//-----------------------------------------------------------------------------

IService::GlobalStatus IService::getStatus() const throw()
{
    return m_globalState ;
}

//-----------------------------------------------------------------------------

bool IService::isStarted() const throw()
{
    return (m_globalState == STARTED) ;
}

//-----------------------------------------------------------------------------

bool IService::isStopped() const throw()
{
    return (m_globalState == STOPPED) ;
}

//-----------------------------------------------------------------------------

bool IService::isSending() const throw()
{
    return (m_notificationState == SENDING_MSG) || (m_notificationState == RECEIVING_WITH_SENDING_MSG) ;
}

//-----------------------------------------------------------------------------

IService::ConfigurationStatus IService::getConfigurationStatus() const throw()
{
    return m_configurationState ;
}

//-----------------------------------------------------------------------------

IService::UpdatingStatus IService::getUpdatingStatus() const throw()
{
    return m_updatingState ;
}

//-----------------------------------------------------------------------------

IService::NotificationStatus IService::getNotificationStatus() const throw()
{
    return m_notificationState ;
}

//-----------------------------------------------------------------------------

void IService::sendingModeOn()
{
    OSLM_ASSERT("INVOKING sendingModeOn WHILE NOT IDLED or RECEIVED ("<<m_notificationState<<") on this = " << this->className(),
            m_notificationState == IDLE || m_notificationState == RECEIVING_MSG);

    if(m_notificationState == RECEIVING_MSG)
    {
        m_notificationState = RECEIVING_WITH_SENDING_MSG;
    }
    else
    {
        m_notificationState = SENDING_MSG;
    }
}

//-----------------------------------------------------------------------------

void IService::sendingModeOff()
{
    OSLM_ASSERT("INVOKING sendingModeOff WHILE NOT SENT  or RECEIVED ("<<m_notificationState<<") on this = " << this->className(),
            m_notificationState == SENDING_MSG || m_notificationState == RECEIVING_WITH_SENDING_MSG);

    if(m_notificationState == RECEIVING_WITH_SENDING_MSG)
    {
        m_notificationState = RECEIVING_MSG;
    }
    else
    {
        m_notificationState = IDLE;
        this->processingPendingMessages();
    }
}

//-----------------------------------------------------------------------------

 void IService::processingPendingMessages()
 {
        OSLM_TRACE(" Processing " << m_msgDeque.size() << " pending message(s).");
        // Processing of pending messages.
        if(m_msgDeque.size() > 50)
        {
            OSLM_FATAL("The size of the queue is very hight " << m_msgDeque.size());
        }
        while (m_msgDeque.size() != 0)
        {
            ::fwServices::ObjectMsg::csptr msg = m_msgDeque.front();
            m_msgDeque.pop_front();
            this->update(msg);
        }
 }

 //-----------------------------------------------------------------------------

/**
 * @brief Streaming a service
 * @see IService::operator<<(std::ostream & _ostream, IService& _service)
 * @note Invoke IService::info( std::ostream )
 */
std::ostream & operator<<(std::ostream & _ostream, IService& _service)
{
    _service.info( _ostream ) ;
    return _ostream ;
}


}

