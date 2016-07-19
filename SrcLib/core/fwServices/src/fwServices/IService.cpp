/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwServices/IService.hpp"
#include "fwServices/registry/ActiveWorkers.hpp"
#include "fwServices/registry/ObjectService.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwRuntime/Convert.hpp>
#include <fwRuntime/EConfigurationElement.hpp>

#include <fwThread/Worker.hpp>
#include <fwTools/fwID.hpp>

#include <boost/property_tree/ptree.hpp>

namespace fwServices
{

//-----------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType IService::s_STARTED_SIG = "started";
const ::fwCom::Slots::SlotKeyType IService::s_UPDATED_SIG = "updated";
const ::fwCom::Slots::SlotKeyType IService::s_STOPPED_SIG = "stopped";

const ::fwCom::Slots::SlotKeyType IService::s_START_SLOT   = "start";
const ::fwCom::Slots::SlotKeyType IService::s_STOP_SLOT    = "stop";
const ::fwCom::Slots::SlotKeyType IService::s_UPDATE_SLOT  = "update";
const ::fwCom::Slots::SlotKeyType IService::s_SWAP_SLOT    = "swap";
const ::fwCom::Slots::SlotKeyType IService::s_SWAPKEY_SLOT = "swapKey";

const std::string IService::s_DEFAULT_OBJECT = "defaultObject";

int IService::s_version = 1;

//-----------------------------------------------------------------------------

IService::IService() :
    m_configuration ( new ::fwRuntime::EConfigurationElement("EmptyConfigurationElement") ),
    m_globalState ( STOPPED ),
    m_updatingState ( NOTUPDATING ),
    m_configurationState ( UNCONFIGURED )
{
    newSignal<StartedSignalType>( s_STARTED_SIG );
    newSignal<UpdatedSignalType>( s_UPDATED_SIG );
    newSignal<StoppedSignalType>( s_STOPPED_SIG );

    m_slotStart   = newSlot( s_START_SLOT, &IService::start, this );
    m_slotStop    = newSlot( s_STOP_SLOT, &IService::stop, this );
    m_slotUpdate  = newSlot( s_UPDATE_SLOT, &IService::update, this );
    m_slotSwap    = newSlot( s_SWAP_SLOT, &IService::swap, this );
    m_slotSwapKey = newSlot( s_SWAPKEY_SLOT, &IService::swapKey, this );
}

//-----------------------------------------------------------------------------

IService::~IService()
{
}

//-----------------------------------------------------------------------------

void IService::info( std::ostream& _sstream )
{
}

//-----------------------------------------------------------------------------

void IService::setOutput(const IService::KeyType& key, const fwData::Object::sptr& object, size_t index)
{
    std::string outKey = key;

    if(m_keyGroupSize.find(key) != m_keyGroupSize.end())
    {
        outKey = KEY_GROUP_NAME(key, index);
    }
    if(::fwServices::OSR::isRegistered(outKey, ::fwServices::IService::AccessType::OUTPUT, this->getSptr()))
    {
        ::fwServices::OSR::unregisterServiceOutput(outKey, this->getSptr());
    }

    if(object != nullptr)
    {
        ::fwServices::OSR::registerServiceOutput(object, outKey, this->getSptr());
    }
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr IService::getObject()
{
    // Handle compatibility with new behavior
    if(m_associatedObject.expired())
    {
        // If we have an appXml2 but with an old service definition, we consider that the old primary object is the
        // first one in the map
        if(!m_inputsMap.empty())
        {
            return ::fwData::Object::constCast(m_inputsMap.begin()->second.lock());
        }
        else if(!m_inOutsMap.empty())
        {
            return m_inOutsMap.begin()->second.lock();
        }
        else if(!m_outputsMap.empty())
        {
            return m_outputsMap.begin()->second.lock();
        }
        else
        {
            OSLM_ASSERT("Associated Object of " <<this->getID()<<" ["<<this->getClassname()<<"] is expired", false );
        }
    }
    return m_associatedObject.lock();
}

//-----------------------------------------------------------------------------

IService::IdType IService::getObjectId(const IService::KeyType& _key) const
{
    auto it = m_idsMap.find(_key);
    SLM_ASSERT("Object key '" + _key + "' not found in service " + this->getID() + ".", it != m_idsMap.end());
    return it->second;
}

//-----------------------------------------------------------------------------

void IService::setObjectId(const IService::KeyType& _key, const IService::IdType& _id)
{
    m_idsMap[_key] = _id;
}

//-----------------------------------------------------------------------------

void displayPt(::boost::property_tree::ptree& pt, std::string indent = "")
{
    OSLM_ERROR(indent << " data : '" << pt.data() << "'" );

    for( ::boost::property_tree::ptree::value_type &v :  pt)
    {
        OSLM_ERROR((indent + "  '") << v.first << "':" );
        displayPt(v.second, indent + "      ");

    }
}

//-----------------------------------------------------------------------------

void IService::setConfiguration(const ::fwRuntime::ConfigurationElement::sptr _cfgElement)
{
    SLM_ASSERT( "Invalid ConfigurationElement", _cfgElement );
    m_configuration      = _cfgElement;
    m_configurationState = UNCONFIGURED;
}

//-----------------------------------------------------------------------------

void IService::setConfiguration( const ConfigType& ptree )
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

IService::SharedFutureType IService::start()
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

        auto sig = this->signal<StartedSignalType>(s_STARTED_SIG);
        sig->asyncEmit();

        return ::boost::move(ufuture);
    }
    else
    {
        return m_slotStart->asyncRun();
    }
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::stop()
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

        auto sig = this->signal<StoppedSignalType>(s_STOPPED_SIG);
        sig->asyncEmit();

        return ::boost::move(ufuture);
    }
    else
    {
        return m_slotStop->asyncRun();
    }
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::update()
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

        auto sig = this->signal<StartedSignalType>(s_UPDATED_SIG);
        sig->asyncEmit();

        return ::boost::move(ufuture);
    }
    else
    {
        return m_slotUpdate->asyncRun();
    }
}

//-----------------------------------------------------------------------------

IService::SharedFutureType IService::swap( ::fwData::Object::sptr _obj )
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

IService::SharedFutureType IService::swapKey(const IService::KeyType& _key, fwData::Object::sptr _obj)
{
    if( !m_associatedWorker || ::fwThread::getCurrentThreadId() == m_associatedWorker->getThreadId() )
    {
        OSLM_FATAL_IF(
            "Service "<< this->getID() << " is not STARTED, no swapping with Object " <<
            (_obj ? _obj->getID() : "nullptr"),
            m_globalState != STARTED);

        PackagedTaskType task( ::boost::bind(&IService::swapping, this, _key) );
        UniqueFutureType ufuture = task.get_future();

        m_globalState = SWAPPING;
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
        return m_slotSwapKey->asyncRun( _key, _obj );
    }
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

IService::KeyConnectionsMap IService::getAutoConnections() const
{
    KeyConnectionsMap connections;
    return connections;
}

//-----------------------------------------------------------------------------

IService::KeyConnectionsType IService::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    return connections;
}

//-----------------------------------------------------------------------------

/**
 * @brief Streaming a service
 * @see IService::operator<<(std::ostream & _ostream, IService& _service)
 * @note Invoke IService::info( std::ostream )
 */
std::ostream& operator<<(std::ostream& _ostream, IService& _service)
{
    _service.info( _ostream );
    return _ostream;
}

//-----------------------------------------------------------------------------

bool IService::isVersion2()
{
    return s_version == 2;
}

//-----------------------------------------------------------------------------

void IService::setVersion(int version)
{
    s_version = version;
}

//-----------------------------------------------------------------------------

}
