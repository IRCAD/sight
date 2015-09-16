/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>

#include <fwTools/fwID.hpp>

#include <fwRuntime/EConfigurationElement.hpp>
#include <fwRuntime/Convert.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include "fwServices/IService.hpp"
#include "fwServices/IEditionService.hpp"
#include "fwServices/registry/ObjectService.hpp"
#include "fwServices/Base.hpp"

namespace fwServices
{

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType IService::s_START_SLOT   = "start";
const ::fwCom::Slots::SlotKeyType IService::s_STOP_SLOT    = "stop";
const ::fwCom::Slots::SlotKeyType IService::s_UPDATE_SLOT  = "update";
const ::fwCom::Slots::SlotKeyType IService::s_RECEIVE_SLOT = "receive";
const ::fwCom::Slots::SlotKeyType IService::s_SWAP_SLOT    = "swap";

//-----------------------------------------------------------------------------

IService::IService() :
    m_configuration ( new ::fwRuntime::EConfigurationElement("EmptyConfigurationElement") ),
    m_globalState ( STOPPED ),
    m_updatingState ( NOTUPDATING ),
    m_configurationState ( UNCONFIGURED )
{
    // by default a weak_ptr have a use_count == 0

    m_slotStart   = ::fwCom::newSlot( &IService::start, this );
    m_slotStop    = ::fwCom::newSlot( &IService::stop, this );
    m_slotUpdate  = ::fwCom::newSlot( &IService::update, this );
    m_slotReceive = ::fwCom::newSlot( &IService::receive, this );
    m_slotSwap    = ::fwCom::newSlot( &IService::swap, this );

    ::fwCom::HasSlots::m_slots
        ( s_START_SLOT, m_slotStart   )
        ( s_STOP_SLOT, m_slotStop    )
        ( s_UPDATE_SLOT, m_slotUpdate  )
        ( s_RECEIVE_SLOT, m_slotReceive )
        ( s_SWAP_SLOT, m_slotSwap    )
    ;
#ifdef COM_LOG
    ::fwCom::HasSlots::m_slots.setID();
    ::fwCom::HasSignals::m_signals.setID();
#endif
}

//-----------------------------------------------------------------------------

IService::~IService()
{
}

//-----------------------------------------------------------------------------

void IService::info( std::ostream &_sstream )
{
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr IService::getObject()
{
    SLM_ASSERT(
        "Associated Object of " <<this->getID()<<" ["<<this->getClassname()<<"] is expired",
        !m_associatedObject.expired() );
    return m_associatedObject.lock();
}

//-----------------------------------------------------------------------------

void displayPt(::boost::property_tree::ptree &pt, std::string indent = "")
{
    OSLM_ERROR(indent << " data : '" << pt.data() << "'" );

    BOOST_FOREACH( ::boost::property_tree::ptree::value_type &v, pt)
    {
        OSLM_ERROR((indent + "  '") << v.first << "':" );
        displayPt(v.second, indent + "      ");

    }
}

void IService::setConfiguration(const ::fwRuntime::ConfigurationElement::sptr _cfgElement)
{
    SLM_ASSERT( "Invalid ConfigurationElement", _cfgElement );
    m_configuration      = _cfgElement;
    m_configurationState = UNCONFIGURED;
}

//-----------------------------------------------------------------------------

void IService::setConfiguration( const ConfigType &ptree )
{
    ::fwRuntime::ConfigurationElement::sptr ce;

    ce = ::fwRuntime::Convert::fromPropertyTree(ptree);

    SLM_ASSERT( "Invalid ConfigurationElement", ce );

    this->setConfiguration(ce);
}

//-----------------------------------------------------------------------------

::fwRuntime::ConfigurationElement::sptr IService::getConfiguration() const
{
    return m_configuration;
}

//-----------------------------------------------------------------------------

IService::ConfigType IService::getConfigTree() const
{
    return ::fwRuntime::Convert::toPropertyTree(this->getConfiguration());
}


////-----------------------------------------------------------------------------
//
//bool IService::checkConfiguration()
//{
//    ::std::stringstream stream ;
//    bool checkingResult = ::fwServices::validation::checkService( this->className() , this->m_configuration , stream ) ;
//    OSLM_TRACE("Configuration checking result for" << this->className() << " : " << stream.str() ) ;
//    return checkingResult ;
//}

//-----------------------------------------------------------------------------

void IService::configure()
{
    //SLM_ASSERT( "Configuration is not correct", this->checkConfiguration() );
    if( m_configurationState == UNCONFIGURED )
    {
        m_configurationState = CONFIGURING;
        if( m_globalState == STOPPED )
        {
            this->configuring();
        }
        else if( m_globalState == STARTED )
        {
            this->reconfiguring();
        }
        m_configurationState = CONFIGURED;
    }
}

//-----------------------------------------------------------------------------

void IService::reconfiguring() throw ( ::fwTools::Failed )
{
    OSLM_FATAL(
        "If this method (reconfiguring) is called, it must be overrided in the implementation ("<<this->getClassname()<<", "<< this->getID() <<
        ")" );
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::start() //throw( ::fwTools::Failed)
{
    if( !m_associatedWorker || ::fwThread::getCurrentThreadId() == m_associatedWorker->getThreadId() )
    {
        OSLM_FATAL_IF("Service "<<this->getID()<<" already started", m_globalState != STOPPED);

        PackagedTaskType task( ::boost::bind(&IService::starting, this) );
        UniqueFutureType ufuture = task.get_future();

        m_globalState = STARTING;
        task();
        m_globalState = STARTED;

        if ( ufuture.has_exception() )
        {
            ufuture.get();
        }
        return ::boost::move(ufuture);
    }
    else
    {
        return m_slotStart->asyncRun();
    }
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::stop() //throw( ::fwTools::Failed)
{
    if( !m_associatedWorker || ::fwThread::getCurrentThreadId() == m_associatedWorker->getThreadId() )
    {
        OSLM_FATAL_IF("Service "<<this->getID()<<" already stopped", m_globalState != STARTED);

        PackagedTaskType task( ::boost::bind(&IService::stopping, this) );
        UniqueFutureType ufuture = task.get_future();

        m_globalState = STOPPING;
        task();
        m_globalState = STOPPED;

        if ( ufuture.has_exception() )
        {
            ufuture.get();
        }
        return ::boost::move(ufuture);
    }
    else
    {
        return m_slotStop->asyncRun();
    }
}

//-----------------------------------------------------------------------------

void IService::receive( ::fwServices::ObjectMsg::csptr _msg )
{
    if( !m_associatedWorker || ::fwThread::getCurrentThreadId() == m_associatedWorker->getThreadId() )
    {
        OSLM_COM("Receive " << _msg->getLightID() << "::" << _msg->getEventIds()[0] << " ( "<< this->getLightID() <<
                 " )" );
        OSLM_FATAL_IF(
            "IService::receive : "<<this->getID()<<" is stopped and cannot manages messages.",
            m_globalState != STARTED);
        this->receiving( _msg );
    }
    else
    {
        m_slotReceive->asyncRun(_msg);
    }
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::update() //throw( ::fwTools::Failed)
{
    if( !m_associatedWorker || ::fwThread::getCurrentThreadId() == m_associatedWorker->getThreadId() )
    {
        OSLM_ASSERT(
            "INVOKING update WHILE STOPPED ("<<m_globalState<<") on this = " << this->className(),
            m_globalState == STARTED );
        OSLM_ASSERT(
            "INVOKING update WHILE NOT IDLE ("<<m_updatingState<<") on this = " << this->className(),
            m_updatingState == NOTUPDATING );

        PackagedTaskType task( ::boost::bind(&IService::updating, this) );
        UniqueFutureType ufuture = task.get_future();

        m_updatingState = UPDATING;
        task();
        m_updatingState = NOTUPDATING;

        if ( ufuture.has_exception() )
        {
            ufuture.get();
        }
        return ::boost::move(ufuture);
    }
    else
    {
        return m_slotUpdate->asyncRun();
    }
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::swap( ::fwData::Object::sptr _obj ) //throw(::fwTools::Failed)
{
    if( !m_associatedWorker || ::fwThread::getCurrentThreadId() == m_associatedWorker->getThreadId() )
    {
        OSLM_ASSERT("Swapping on "<< this->getID() << " with same Object " << _obj->getID(),
                    m_associatedObject.lock() != _obj );
        OSLM_FATAL_IF(
            "Service "<< this->getID() << " is not STARTED, no swapping with Object " << _obj->getID(),
            m_globalState != STARTED);

        PackagedTaskType task( ::boost::bind(&IService::swapping, this) );
        UniqueFutureType ufuture = task.get_future();

        m_globalState = SWAPPING;
        ::fwServices::OSR::swapService( _obj, this->getSptr() );
        task();
        m_globalState = STARTED;

        if ( ufuture.has_exception() )
        {
            ufuture.get();
        }
        return ::boost::move(ufuture);
    }
    else
    {
        return m_slotSwap->asyncRun( _obj );
    }
}

//-----------------------------------------------------------------------------

void IService::receiving( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed )
{
    SLM_FATAL("TODO : This method must be implemented");
}

//-----------------------------------------------------------------------------

IService::GlobalStatus IService::getStatus() const throw()
{
    return m_globalState;
}

//-----------------------------------------------------------------------------

bool IService::isStarted() const throw()
{
    return (m_globalState == STARTED);
}

//-----------------------------------------------------------------------------

bool IService::isStopped() const throw()
{
    return (m_globalState == STOPPED);
}

//-----------------------------------------------------------------------------

IService::ConfigurationStatus IService::getConfigurationStatus() const throw()
{
    return m_configurationState;
}

//-----------------------------------------------------------------------------

IService::UpdatingStatus IService::getUpdatingStatus() const throw()
{
    return m_updatingState;
}

//-----------------------------------------------------------------------------

void IService::setWorker( ::fwThread::Worker::sptr worker )
{
    m_associatedWorker = worker;
    ::fwCom::HasSlots::m_slots.setWorker( m_associatedWorker );
}

//-----------------------------------------------------------------------------

::fwThread::Worker::sptr IService::getWorker() const
{
    return m_associatedWorker;
}

//-----------------------------------------------------------------------------

IService::KeyConnectionsType IService::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Object::s_OBJECT_MODIFIED_SIG, s_RECEIVE_SLOT ) );
    return connections;
}

//-----------------------------------------------------------------------------

#ifdef COM_LOG
void IService::setID( ::fwTools::fwID::IDType newID )
{
    if( !this->hasID() ||
        this->getID( ::fwTools::fwID::MUST_EXIST ) != newID )
    {
        this->::fwTools::fwID::setID( newID );
    }

    std::string lightID = this->getLightID( ::fwTools::fwID::MUST_EXIST );

    ::fwCom::HasSlots::m_slots.setID( lightID + "::" );
    ::fwCom::HasSignals::m_signals.setID( lightID + "::" );
}
#endif

//-----------------------------------------------------------------------------

/**
 * @brief Streaming a service
 * @see IService::operator<<(std::ostream & _ostream, IService& _service)
 * @note Invoke IService::info( std::ostream )
 */
std::ostream & operator<<(std::ostream & _ostream, IService& _service)
{
    _service.info( _ostream );
    return _ostream;
}


}

