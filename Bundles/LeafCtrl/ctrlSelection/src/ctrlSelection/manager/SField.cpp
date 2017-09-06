/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlSelection/manager/SField.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

#include <fwTools/fwID.hpp>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>


namespace ctrlSelection
{

namespace manager
{

static const ::fwCom::Slots::SlotKeyType s_ADD_FIELDS_SLOT    = "addFields";
static const ::fwCom::Slots::SlotKeyType s_CHANGE_FIELDS_SLOT = "changeFields";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_FIELDS_SLOT = "removeFields";

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::ctrlSelection::IManagerSrv, ::ctrlSelection::manager::SField, ::fwData::Object );

//-----------------------------------------------------------------------------

SField::SField() noexcept : m_dummyStopMode(false)
{
    newSlot(s_ADD_FIELDS_SLOT, &SField::addFields, this);
    newSlot(s_CHANGE_FIELDS_SLOT, &SField::changeFields, this);
    newSlot(s_REMOVE_FIELDS_SLOT, &SField::removeFields, this);
}

//-----------------------------------------------------------------------------

SField::~SField() noexcept
{
}

//-----------------------------------------------------------------------------

void SField::reconfiguring()
{
}

//-----------------------------------------------------------------------------

void SField::updating()
{
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

void SField::info( std::ostream& _sstream )
{
}

//-----------------------------------------------------------------------------

void SField::stopping()
{
    SLM_TRACE_FUNC();

    for(SubServicesMapType::value_type elt :  m_fieldsSubServices)
    {
        SubServicesVecType subServices = elt.second;
        BOOST_REVERSE_FOREACH( SPTR(SubService) subSrv, subServices )
        {
            OSLM_ASSERT("SubService on "<< elt.first <<" expired !", subSrv->getService() );

            if( subSrv->m_hasAutoConnection )
            {
                subSrv->m_connections.disconnect();
            }
            subSrv->getService()->stop().wait();
            ::fwServices::OSR::unregisterService(subSrv->getService());
            subSrv->m_service.reset();
        }
    }
    m_fieldsSubServices.clear();

    while( !m_objectConnections.empty())
    {
        this->removeConnections(m_objectConnections.begin()->first);
    }
    SLM_ASSERT("Connections must be empty", m_objectConnections.empty());

    while( !m_proxyCtns.empty())
    {
        this->disconnectProxies(m_proxyCtns.begin()->first);
    }
    SLM_ASSERT("Proxy connections must be empty", m_proxyCtns.empty());
}

//-----------------------------------------------------------------------------

void SField::configuring()
{
    SLM_TRACE_FUNC();
    std::vector < ConfigurationType > vectMode = m_configuration->find("mode");
    if(!vectMode.empty())
    {
        ConfigurationType modeConfiguration = vectMode.at(0);
        SLM_ASSERT("Missing 'type' attribute", modeConfiguration->hasAttribute("type"));
        std::string mode = modeConfiguration->getAttributeValue("type");
        SLM_ASSERT("Wrong mode type", (mode == "dummy" ) || (mode == "stop" ) || mode=="startAndUpdate");
        m_dummyStopMode = (mode == "dummy" );
        m_mode          = mode;
    }

    std::vector < ConfigurationType > vectConfig = m_configuration->find("config");
    SLM_ASSERT("Missing <config> tag!", !vectConfig.empty());
    m_managerConfiguration = vectConfig.at(0);
}

//-----------------------------------------------------------------------------

void SField::starting()
{
    SLM_TRACE_FUNC();

    ::fwData::Object::sptr object = this->getObject();
    const ::fwData::Object::FieldMapType& fieldsMap = object->getFields();
    ::fwRuntime::ConfigurationElementContainer::Iterator iter;
    for (iter = m_managerConfiguration->begin(); iter != m_managerConfiguration->end(); ++iter)
    {
        if ((*iter)->getName() == "field")
        {
            const std::string fieldName = (*iter)->getAttributeValue("id");

            if (fieldsMap.find(fieldName) != fieldsMap.end())
            {
                this->addField(fieldName, fieldsMap.at(fieldName));
            }
            else if (m_dummyStopMode)
            {
                // Initialize on dummy fields
                this->initOnDummyObject( fieldName );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SField::addFields( ::fwData::Object::FieldsContainerType fields )
{
    for( auto addedObjectId :  fields)
    {
        if(m_fieldsSubServices.find(addedObjectId.first) != m_fieldsSubServices.end())
        {
            // Services are on dummyObject
            this->swapField(addedObjectId.first, addedObjectId.second);
        }
        else
        {
            this->addField(addedObjectId.first, addedObjectId.second);
        }
    }
}

//-----------------------------------------------------------------------------

::fwServices::IService::sptr SField::add( ::fwData::Object::sptr obj, ::fwRuntime::ConfigurationElement::sptr _elt )
{
    OSLM_ASSERT("ConfigurationElement node name must be \"service\" not "<<_elt->getName(),
                _elt->getName() == "service" );
    SLM_ASSERT("Attribute \"type\" is missing", _elt->hasAttribute("type") );
    SLM_ASSERT("Attribute \"impl\" is missing", _elt->hasAttribute("impl") );

    ::fwServices::IService::sptr service;

    std::string serviceType        = _elt->getExistingAttributeValue("type");
    std::string implementationType = _elt->getExistingAttributeValue("impl");

    // Add service with possible id
    if( _elt->hasAttribute("uid")  )
    {
        service = ::fwServices::add( obj, serviceType, implementationType, _elt->getExistingAttributeValue("uid") );
    }
    else
    {
        service = ::fwServices::add( obj, serviceType, implementationType );
    }

    // Search for configuration : inline or offline
    ::fwRuntime::ConfigurationElement::sptr cfg = _elt;
    if( _elt->hasAttribute("config"))
    {
        cfg = ::fwRuntime::ConfigurationElement::constCast(
            ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig( _elt->
                                                                                   getExistingAttributeValue("config"),
                                                                                   implementationType ) );
    }

    // Set configuration
    service->setConfiguration( cfg );

    // Configure
    service->configure();

    // Return
    return service;
}

//-----------------------------------------------------------------------------

void SField::addField( const FieldNameType& fieldName, ::fwData::Object::sptr field )
{
    if(!m_managerConfiguration->find("field", "id", fieldName).empty())
    {
        ConfigurationType conf      = m_managerConfiguration->find("field", "id", fieldName).at(0);
        const std::string fieldType = conf->getAttributeValue("type");

        OSLM_ASSERT("FieldType "<<fieldType<<" does not match ObjectType in Object "<<field->getClassname(),
                    fieldType == field->getClassname());
        SubServicesVecType subVecSrv;
        std::vector< ConfigurationType > services = conf->find("service");
        for( ConfigurationType cfg :  services)
        {
            ::fwServices::IService::sptr srv = this->add( field, cfg );
            OSLM_ASSERT("Instantiation Service failed on field "<<fieldName, srv);
            srv->configure();
            SPTR(SubService) subSrv = SPTR(SubService)( new SubService());
            subSrv->m_config        = cfg;
            subSrv->m_service       = srv;

            // Standard communication management
            if ( cfg->getAttributeValue("autoConnect") == "yes" )
            {
                subSrv->m_hasAutoConnection = true;
                subSrv->m_connections.connect( field, srv, srv->getObjSrvConnections() );
            }


            std::string workerKey = cfg->getAttributeValue("worker");

            if (!workerKey.empty())
            {
                ::fwServices::registry::ActiveWorkers::sptr activeWorkers =
                    ::fwServices::registry::ActiveWorkers::getDefault();
                ::fwThread::Worker::sptr worker;
                worker = activeWorkers->getWorker(workerKey);
                if (!worker)
                {
                    worker = ::fwThread::Worker::New();
                    activeWorkers->addWorker(workerKey, worker);
                }
                srv->setWorker(worker);
            }

            subVecSrv.push_back(subSrv);
            subSrv->getService()->start();
            if (m_mode =="startAndUpdate")
            {
                subSrv->getService()->update();
            }
        }
        m_fieldsSubServices[fieldName] = subVecSrv;

        this->manageConnections(fieldName, field, conf);
        this->manageProxies(fieldName, field, conf);
    }
    else
    {
        OSLM_DEBUG("Field "<<fieldName<<" not managed.");
    }
}
//-----------------------------------------------------------------------------

void SField::changeFields( ::fwData::Object::FieldsContainerType newfields,
                           ::fwData::Object::FieldsContainerType oldFields )
{
    for( auto swappedObjectId :  newfields)
    {
        this->swapField(swappedObjectId.first, swappedObjectId.second);
    }
}

//-----------------------------------------------------------------------------

void SField::swapField(const FieldNameType& fieldName, ::fwData::Object::sptr field)
{
    std::vector< ConfigurationType > fields = m_managerConfiguration->find("field", "id", fieldName);
    for( ConfigurationType cfg :  fields)
    {
        SubServicesVecType subServices = m_fieldsSubServices[fieldName];
        for( SPTR(SubService) subSrv :  subServices )
        {
            OSLM_ASSERT("SubService on " << fieldName <<" expired !", subSrv->getService() );
            OSLM_ASSERT( subSrv->getService()->getID() <<  " is not started ", subSrv->getService()->isStarted());

            OSLM_TRACE("Swapping subService " << subSrv->getService()->getID() << " on "<< fieldName );
            if(subSrv->getService()->getObject() != field)
            {
                subSrv->getService()->swap(field);
                subSrv->m_dummy.reset();

                if (subSrv->m_hasAutoConnection)
                {
                    subSrv->m_connections.disconnect();
                    subSrv->m_connections.connect( field, subSrv->getService(),
                                                   subSrv->getService()->getObjSrvConnections() );
                }
            }
            else
            {
                OSLM_WARN( subSrv->getService()->getID()
                           << "'s field already is '"
                           << subSrv->getService()->getObject()->getID()
                           << "', no need to swap");
            }
        }
        this->manageConnections(fieldName, field, cfg);
        this->manageProxies(fieldName, field, cfg);
    }
}

//-----------------------------------------------------------------------------

void SField::removeFields( ::fwData::Object::FieldsContainerType fields )
{
    for( auto swappedObjectId :  fields)
    {
        this->removeField(swappedObjectId.first);
    }
}

//-----------------------------------------------------------------------------

void SField::removeField( const FieldNameType& fieldName )
{
    if(!m_managerConfiguration->find("field", "id", fieldName).empty())
    {
        ConfigurationType conf      = m_managerConfiguration->find("field", "id", fieldName).at(0);
        const std::string fieldType = conf->getAttributeValue("type");

        this->removeConnections(fieldName);
        this->disconnectProxies(fieldName);

        SubServicesVecType subServices = m_fieldsSubServices[fieldName];
        ::fwData::Object::sptr dummyObj;
        dummyObj = ::fwData::factory::New(fieldType);
        for( SPTR(SubService) subSrv :  subServices )
        {
            OSLM_ASSERT("SubService on " << fieldName <<" expired !", subSrv->getService() );
            OSLM_ASSERT( subSrv->getService()->getID() <<  " is not started ", subSrv->getService()->isStarted());
            if(m_dummyStopMode)
            {
                subSrv->getService()->swap(dummyObj);
                subSrv->m_dummy = dummyObj;
            }
            else
            {
                if( subSrv->m_hasAutoConnection )
                {
                    subSrv->m_connections.disconnect();
                }

                subSrv->getService()->stop();
                ::fwServices::OSR::unregisterService(subSrv->getService());
                subSrv->m_service.reset();
            }
        }
        if(!m_dummyStopMode)
        {
            m_fieldsSubServices.erase(fieldName);
        }
        else
        {
            this->manageConnections(fieldName, dummyObj, conf);
            this->manageProxies(fieldName, dummyObj, conf);
        }
    }
    else
    {
        OSLM_DEBUG("Object "<<fieldName<<" not managed.");
    }
}

//-----------------------------------------------------------------------------

void SField::initOnDummyObject( const FieldNameType& fieldName )
{
    SLM_ASSERT( "'fieldName' required attribute missing or empty", !fieldName.empty() );

    ::fwData::Object::sptr object = this->getObject();

    OSLM_ASSERT(fieldName << " not found in object.",
                object->getFields().find(fieldName) == object->getFields().end());

    ConfigurationType conf      = m_managerConfiguration->find("field", "id", fieldName).at(0);
    const std::string fieldType = conf->getAttributeValue("type");
    SLM_ASSERT( "'type' required attribute missing or empty", !fieldType.empty() );

    // Any subServices have been registered with field.
    if ( m_fieldsSubServices.count(fieldName) == 0 )
    {
        OSLM_TRACE ( "'"<< fieldName << "' nonexistent'");

        ::fwData::Object::sptr dummyObj;
        dummyObj = ::fwData::factory::New(fieldType);
        SubServicesVecType subVecSrv;
        std::vector< ConfigurationType > services = conf->find("service");
        for( ConfigurationType cfg :  services)
        {
            ::fwServices::IService::sptr srv = this->add( dummyObj, cfg );
            OSLM_ASSERT("Instantiation Service failed ofieldct "<<fieldName, srv);
            srv->configure();
            SPTR(SubService) subSrv = SPTR(SubService)( new SubService());
            subSrv->m_config        = cfg;
            subSrv->m_service       = srv;
            subSrv->m_dummy         = dummyObj;
            subVecSrv.push_back(subSrv);
            subSrv->getService()->start();

            if ( cfg->getAttributeValue("autoConnect") == "yes" )
            {
                subSrv->m_hasAutoConnection = true;
            }
        }
        m_fieldsSubServices[fieldName] = subVecSrv;
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType SField::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Object::s_ADDED_FIELDS_SIG, s_ADD_FIELDS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Object::s_CHANGED_FIELDS_SIG, s_CHANGE_FIELDS_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Object::s_REMOVED_FIELDS_SIG, s_REMOVE_FIELDS_SLOT ) );

    return connections;
}

//-----------------------------------------------------------------------------


} // manager
} // ctrlSelection
