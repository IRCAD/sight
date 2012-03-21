/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwTools/fwID.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

#include "ctrlSelection/manager/SField.hpp"

namespace ctrlSelection
{

namespace manager
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::ctrlSelection::IManagerSrv, ::ctrlSelection::manager::SField, ::fwTools::Object ) ;

//-----------------------------------------------------------------------------

SField::SField() throw() : m_dummyStopMode(false)
{
    this->addNewHandledEvent( ::fwServices::ObjectMsg::ADDED_FIELDS );
    this->addNewHandledEvent( ::fwServices::ObjectMsg::REMOVED_FIELDS );
    this->addNewHandledEvent( ::fwServices::ObjectMsg::CHANGED_FIELDS );
}

//-----------------------------------------------------------------------------

SField::~SField() throw()
{}

//-----------------------------------------------------------------------------

void SField::updating( ::fwServices::ObjectMsg::csptr message ) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwServices::ObjectMsg::csptr fieldMsg = ::fwServices::ObjectMsg::dynamicConstCast(message);
    SLM_FATAL_IF("Received message must be fieldMsg", fieldMsg == 0 );

    if ( fieldMsg->hasEvent( ::fwServices::ObjectMsg::ADDED_FIELDS ) )
    {
        this->addFields( fieldMsg->getAddedFields() );
    }

    if ( fieldMsg->hasEvent( ::fwServices::ObjectMsg::REMOVED_FIELDS ) )
    {
        this->removeFields( fieldMsg->getRemovedFields() );
    }

    if ( fieldMsg->hasEvent( ::fwServices::ObjectMsg::CHANGED_FIELDS ) )
    {
        this->swapFields( fieldMsg->getNewChangedFields() );
    }
}

//-----------------------------------------------------------------------------

void SField::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SField::updating() throw ( ::fwTools::Failed )
{
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

void SField::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

void SField::stopping()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    for( SubServicesMapType::iterator iterMap = m_fieldsSubServices.begin(); iterMap != m_fieldsSubServices.end(); ++iterMap )
    {
        SubServicesVecType subServices = iterMap->second;
        BOOST_REVERSE_FOREACH( SPTR(SubService) subSrv, subServices )
        {
            OSLM_ASSERT("SubService on "<< iterMap->first <<" expired !", subSrv->getService() );

            if( subSrv->m_hasComChannel )
            {
                subSrv->getComChannel()->stop();
                ::fwServices::OSR::unregisterService(subSrv->getComChannel());
                subSrv->m_comChannel.reset();
            }
            subSrv->getService()->stop();
            ::fwServices::OSR::unregisterService(subSrv->getService());
            subSrv->m_service.reset();
        }
    }
    m_fieldsSubServices.clear();
}

//-----------------------------------------------------------------------------

void SField::configuring()  throw ( ::fwTools::Failed )
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
        m_mode = mode;
    }

    std::vector < ConfigurationType > vectConfig = m_configuration->find("config");
    SLM_ASSERT("Missing <config> tag!", !vectConfig.empty());
    m_managerConfiguration = vectConfig.at(0);
}

//-----------------------------------------------------------------------------

void SField::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwData::Object::sptr object = this->getObject< ::fwData::Object >() ;
    const ::fwData::Object::FieldMapType& fieldsMap = object->getFields();
    ::fwRuntime::ConfigurationElementContainer::Iterator iter;
    for (iter = m_managerConfiguration->begin() ; iter != m_managerConfiguration->end() ; ++iter)
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

void SField::addFields( const ModifiedFieldsContainerType& fields )
{
    BOOST_FOREACH( ModifiedFieldsContainerType::value_type addedObjectId, fields)
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

::fwServices::IService::sptr SField::add( ::fwTools::Object::sptr obj , ::fwRuntime::ConfigurationElement::sptr _elt )
{
    OSLM_ASSERT("ConfigurationElement node name must be \"service\" not "<<_elt->getName(), _elt->getName() == "service" ) ;
    SLM_ASSERT("Attribute \"type\" is missing", _elt->hasAttribute("type") ) ;
    SLM_ASSERT("Attribute \"implementation\" is missing", _elt->hasAttribute("implementation") ) ;

    ::fwServices::IService::sptr service ;

    std::string serviceType = _elt->getExistingAttributeValue("type") ;
    std::string implementationType = _elt->getExistingAttributeValue("implementation");

    // Add service with possible id
    if( _elt->hasAttribute("uid")  )
    {
        service = ::fwServices::add( obj , serviceType , implementationType , _elt->getExistingAttributeValue("uid") );
    }
    else
    {
        service =  ::fwServices::add( obj , serviceType , implementationType )  ;
    }

    // Search for configuration : inline or offline
    ::fwRuntime::ConfigurationElement::sptr cfg = _elt;
    if( _elt->hasAttribute("config"))
    {
        cfg = ::fwRuntime::ConfigurationElement::constCast( ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig( _elt->getExistingAttributeValue("config") , implementationType ) );
    }

    // Set configuration
    service->setConfiguration( cfg ) ;

    // Configure
    service->configure();

    // Standard communication management
    SLM_ASSERT("autoComChannel attribute missing in service "<< service->getClassname(), _elt->hasAttribute("autoComChannel"));

    std::string autoComChannel = _elt->getExistingAttributeValue("autoComChannel");
    SLM_ASSERT("wrong autoComChannel definition", autoComChannel=="yes" || autoComChannel=="no");
    if(autoComChannel=="yes")
    {
        ::fwServices::ComChannelService::sptr comChannel = ::fwServices::registerCommunicationChannel( obj , service);
        // Add priority for the new comChannel if defined, otherwise the default value is 0.5
        if( _elt->hasAttribute("priority"))
        {
            std::string priorityStr = _elt->getExistingAttributeValue("priority");
            double priority = ::boost::lexical_cast< double >( priorityStr );
            if(priority < 0.0) priority = 0.0;
            if(priority > 1.0) priority = 1.0;
            comChannel->setPriority(priority);
        }
        comChannel->start();
    }

    // Return
    return service ;
}

//-----------------------------------------------------------------------------

void SField::addField( const FieldNameType& fieldName, ::fwData::Object::sptr field )
{
    if(!m_managerConfiguration->find("field", "id", fieldName).empty())
    {
        ConfigurationType conf = m_managerConfiguration->find("field", "id", fieldName).at(0);
        const std::string fieldType   = conf->getAttributeValue("type");

        OSLM_ASSERT("FieldType "<<fieldType<<" does not match ObjectType in Object "<<field->getClassname(),
                fieldType == field->getClassname());
        SubServicesVecType subVecSrv;
        BOOST_FOREACH( ConfigurationType cfg, conf->find("service"))
        {
            ::fwServices::IService::sptr srv = this->add( field, cfg );
            OSLM_ASSERT("Instantiation Service failed on field "<<fieldName, srv);
            srv->configure();
            SPTR(SubService) subSrv =  SPTR(SubService)( new SubService());
            subSrv->m_config = cfg;
            subSrv->m_service = srv;

            subVecSrv.push_back(subSrv);
            subSrv->getService()->start();
            if (m_mode =="startAndUpdate")
            {
                subSrv->getService()->update();
            }

            if ( cfg->hasAttribute("autoComChannel") && cfg->getExistingAttributeValue("autoComChannel") == "yes" )
            {
                subSrv->m_hasComChannel = true;
                subSrv->m_comChannel = ::fwServices::getCommunicationChannel( field, srv);
            }
        }
        m_fieldsSubServices[fieldName] = subVecSrv;
    }
    else
    {
        OSLM_DEBUG("Field "<<fieldName<<" not managed.");
    }
}
//-----------------------------------------------------------------------------

void SField::swapFields( const ModifiedFieldsContainerType& fields )
{
    BOOST_FOREACH( ModifiedFieldsContainerType::value_type swappedObjectId, fields)
    {
        this->swapField(swappedObjectId.first, swappedObjectId.second);
    }
}

//-----------------------------------------------------------------------------

void SField::swapField(const FieldNameType& fieldName, ::fwTools::Object::sptr field)
{
    if(m_fieldsSubServices.find(fieldName) != m_fieldsSubServices.end())
    {
        BOOST_FOREACH( ConfigurationType cfg, m_managerConfiguration->find("field", "id", fieldName))
        {
            SubServicesVecType subServices = m_fieldsSubServices[fieldName];
            BOOST_FOREACH( SPTR(SubService) subSrv, subServices )
            {
                OSLM_ASSERT("SubService on " << fieldName <<" expired !", subSrv->getService() );
                OSLM_ASSERT( subSrv->getService()->getID() <<  " is not started ", subSrv->getService()->isStarted());

                OSLM_TRACE("Swapping subService " << subSrv->getService()->getID() << " on "<< fieldName );
                if(subSrv->getService()->getObject() != field)
                {
                    subSrv->getService()->swap(field);
                    subSrv->m_dummy.reset();
                }
                else
                {
                    OSLM_WARN( subSrv->getService()->getID()
                            << "'s field already is '"
                            << subSrv->getService()->getObject()->getID()
                            << "', no need to swap");
                }
            }
        }
    }
    else
    {
        OSLM_INFO("Object "<<fieldName<<" not found in managed fields.");
    }
}

//-----------------------------------------------------------------------------

void SField::removeFields( const ModifiedFieldsContainerType& fields )
{
    BOOST_FOREACH( ModifiedFieldsContainerType::value_type swappedObjectId, fields)
    {
        this->removeField(swappedObjectId.first);
    }
}

//-----------------------------------------------------------------------------

void SField::removeField( const FieldNameType& fieldName )
{
    if(!m_managerConfiguration->find("field", "id", fieldName).empty())
    {
        ConfigurationType conf = m_managerConfiguration->find("field", "id", fieldName).at(0);
        const std::string fieldType   = conf->getAttributeValue("type");

        SubServicesVecType subServices = m_fieldsSubServices[fieldName];
        ::fwTools::Object::sptr dummyObj = ::fwTools::Factory::New(fieldType);
        BOOST_FOREACH( SPTR(SubService) subSrv, subServices )
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
                if( subSrv->m_hasComChannel )
                {
                    subSrv->getComChannel()->stop();
                    ::fwServices::OSR::unregisterService(subSrv->getComChannel());
                    subSrv->m_comChannel.reset();
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

    ::fwData::Object::sptr object = this->getObject< ::fwData::Object >() ;

    OSLM_ASSERT(fieldName << " not found in object.",
            object->getFields().find(fieldName) == object->getFields().end());

    ConfigurationType conf = m_managerConfiguration->find("field", "id", fieldName).at(0);
    const std::string fieldType    = conf->getAttributeValue("type");
    SLM_ASSERT( "'type' required attribute missing or empty", !fieldType.empty() );

    // Any subServices have been registered with field.
    if ( m_fieldsSubServices.count(fieldName) == 0 )
    {
        OSLM_TRACE ( "'"<< fieldName << "' nonexistent'");

        ::fwTools::Object::sptr dummyObj = ::fwTools::Factory::New(fieldType);
        SubServicesVecType subVecSrv;
        BOOST_FOREACH( ConfigurationType cfg, conf->find("service"))
        {
            ::fwServices::IService::sptr srv = this->add( dummyObj, cfg );
            OSLM_ASSERT("Instantiation Service failed ofieldct "<<fieldName, srv);
            srv->configure();
            SPTR(SubService) subSrv =  SPTR(SubService)( new SubService());
            subSrv->m_config = cfg;
            subSrv->m_service = srv;
            subSrv->m_dummy = dummyObj;
            subVecSrv.push_back(subSrv);
            subSrv->getService()->start();

            if ( cfg->hasAttribute("autoComChannel") && cfg->getExistingAttributeValue("autoComChannel") == "yes" )
            {
                subSrv->m_hasComChannel = true;
                subSrv->m_comChannel = ::fwServices::getCommunicationChannel( dummyObj, srv);
            }
        }
        m_fieldsSubServices[fieldName] = subVecSrv;
    }
}

//-----------------------------------------------------------------------------

} // manager
} // ctrlSelection
