/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwTools/fwID.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

#include <fwComEd/CompositeMsg.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Acquisition.hpp>

#include "ctrlSelection/manager/SwapperSrv.hpp"

namespace ctrlSelection
{

namespace manager
{

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::ctrlSelection::IManagerSrv, ::ctrlSelection::manager::SwapperSrv, ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

SwapperSrv::SwapperSrv() throw() : m_dummyStopMode(false)
{
    //handlingEventOff ::fwComEd::CompositeMsg::ADDED_KEYS );
    //handlingEventOff ::fwComEd::CompositeMsg::REMOVED_KEYS );
    //handlingEventOff ::fwComEd::CompositeMsg::CHANGED_KEYS );
}

//-----------------------------------------------------------------------------

SwapperSrv::~SwapperSrv() throw()
{}

//-----------------------------------------------------------------------------

void SwapperSrv::receiving( ::fwServices::ObjectMsg::csptr message ) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast(message);
    if (compositeMsg)
    {
        if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::ADDED_KEYS ) )
        {
            ::fwData::Composite::sptr fields = compositeMsg->getAddedKeys();
            this->addObjects( fields );
        }

        if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::REMOVED_KEYS ) )
        {
            ::fwData::Composite::sptr fields = compositeMsg->getRemovedKeys();
            this->removeObjects( fields );
        }

        if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::CHANGED_KEYS ) )
        {
            ::fwData::Composite::sptr fields = compositeMsg->getNewChangedKeys();
            this->swapObjects( fields );
        }
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SwapperSrv::updating() throw ( ::fwTools::Failed )
{
    stopping();
    starting();
}

//-----------------------------------------------------------------------------

void SwapperSrv::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

void SwapperSrv::stopping()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    for( SubServicesMapType::iterator iterMap = m_objectsSubServices.begin(); iterMap != m_objectsSubServices.end(); ++iterMap )
    {
        SubServicesVecType subServices = iterMap->second;
        BOOST_REVERSE_FOREACH( SPTR(SubService) subSrv, subServices )
        {
            OSLM_ASSERT("SubService on "<< iterMap->first <<" expired !", subSrv->getService() );

            if( subSrv->m_hasAutoConnection )
            {
                subSrv->m_connections->disconnect();
            }
            subSrv->getService()->stop();
            ::fwServices::OSR::unregisterService(subSrv->getService());
            subSrv->m_service.reset();
        }
    }
    m_objectsSubServices.clear();
}

//-----------------------------------------------------------------------------

void SwapperSrv::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    std::vector < ConfigurationType > vectMode = m_configuration->find("mode");
    if(!vectMode.empty())
    {
        ConfigurationType modeConfiguration = vectMode.at(0);
        SLM_ASSERT("Missing attribute type", modeConfiguration->hasAttribute("type"));
        std::string mode = modeConfiguration->getAttributeValue("type");
        SLM_ASSERT("Wrong type mode", (mode == "dummy" ) || (mode == "stop" ) || mode=="startAndUpdate");
        m_dummyStopMode = (mode == "dummy" );
        m_mode = mode;
    }

    std::vector < ConfigurationType > vectConfig = m_configuration->find("config");
    SLM_ASSERT("Missing <config> tag!", !vectConfig.empty());
    m_managerConfiguration = vectConfig.at(0);
}

//-----------------------------------------------------------------------------

void SwapperSrv::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >() ;
    ::fwRuntime::ConfigurationElementContainer::Iterator iter;
    for (iter = m_managerConfiguration->begin() ; iter != m_managerConfiguration->end() ; ++iter)
    {
        if ((*iter)->getName() == "object")
        {
            const std::string objectId      = (*iter)->getAttributeValue("id");

            if (composite->find(objectId) != composite->end())
            {
                this->addObject(objectId, (*composite)[objectId]);
            }
            else if (m_dummyStopMode)
            {
                // Initialize on dummy objects
                this->initOnDummyObject( objectId );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::addObjects( ::fwData::Composite::sptr _composite )
{
    BOOST_FOREACH( ::fwData::Composite::ValueType addedObjectId, _composite->getContainer())
    {
        if(m_objectsSubServices.find(addedObjectId.first) != m_objectsSubServices.end())
        {
            // Services are on dummyObject
            this->swapObject(addedObjectId.first, addedObjectId.second);
        }
        else
        {
            this->addObject(addedObjectId.first, addedObjectId.second);
        }
    }
}

//-----------------------------------------------------------------------------

::fwServices::IService::sptr SwapperSrv::add( ::fwData::Object::sptr obj , ::fwRuntime::ConfigurationElement::sptr _elt )
{
    OSLM_ASSERT("ConfigurationElement node name must be \"service\" not "<<_elt->getName(), _elt->getName() == "service" ) ;
    SLM_ASSERT("Attribute \"type\" is missing", _elt->hasAttribute("type") ) ;
    SLM_ASSERT("Attribute \"impl\" is missing", _elt->hasAttribute("impl") ) ;

    ::fwServices::IService::sptr service ;

    std::string serviceType = _elt->getExistingAttributeValue("type") ;
    std::string implementationType = _elt->getExistingAttributeValue("impl");

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

    // Return
    return service ;
}

//-----------------------------------------------------------------------------

void SwapperSrv::addObject( const std::string objectId, ::fwData::Object::sptr object )
{
    if(!m_managerConfiguration->find("object", "id", objectId).empty())
    {
        ConfigurationType conf = m_managerConfiguration->find("object", "id", objectId).at(0);
        const std::string objectType   = conf->getAttributeValue("type");

        OSLM_ASSERT("ObjectType "<<objectType<<" does not match ObjectType in Composite "<<object->getClassname(),
                objectType == object->getClassname());
        SubServicesVecType subVecSrv;
        std::vector< ConfigurationType > confVec = conf->find("service");
        BOOST_FOREACH( ConfigurationType cfg, confVec )
        {
            ::fwServices::IService::sptr srv = this->add( object, cfg );
            OSLM_ASSERT("Instantiation Service failed on object "<<objectId, srv);

            SPTR(SubService) subSrv =  SPTR(SubService)( new SubService());
            subSrv->m_config = cfg;
            subSrv->m_service = srv;

            // Standard communication management
            SLM_ASSERT("autoConnect attribute missing in service "<< srv->getClassname(), cfg->hasAttribute("autoConnect"));

            if ( cfg->getExistingAttributeValue("autoConnect") == "yes" )
            {
                subSrv->m_hasAutoConnection = true;
                if (!subSrv->m_connections)
                {
                    subSrv->m_connections = ::fwServices::helper::SigSlotConnection::New();
                }
                subSrv->m_connections->connect( object, srv, srv->getObjSrvConnections() );
            }

            subVecSrv.push_back(subSrv);
            subSrv->getService()->start();
            if (m_mode =="startAndUpdate")
            {
                subSrv->getService()->update();
            }
        }
        m_objectsSubServices[objectId] = subVecSrv;
    }
    else
    {
        OSLM_DEBUG("Object "<<objectId<<" not managed.");
    }
}
//-----------------------------------------------------------------------------

void SwapperSrv::swapObjects( ::fwData::Composite::sptr _composite )
{
    BOOST_FOREACH( ::fwData::Composite::ValueType swappedObjectId, _composite->getContainer())
    {
        this->swapObject(swappedObjectId.first, swappedObjectId.second);
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::swapObject(const std::string objectId, ::fwData::Object::sptr object)
{
    if(m_objectsSubServices.find(objectId) != m_objectsSubServices.end())
    {
        std::vector< ConfigurationType > confVec = m_managerConfiguration->find("object", "id", objectId);
        BOOST_FOREACH( ConfigurationType cfg, confVec )
        {
            SubServicesVecType subServices = m_objectsSubServices[objectId];
            BOOST_FOREACH( SPTR(SubService) subSrv, subServices )
            {
                OSLM_ASSERT("SubService on " << objectId <<" expired !", subSrv->getService() );
                OSLM_ASSERT( subSrv->getService()->getID() <<  " is not started ", subSrv->getService()->isStarted());

                OSLM_TRACE("Swapping subService " << subSrv->getService()->getID() << " on "<< objectId );
                if(subSrv->getService()->getObject() != object)
                {
                    subSrv->getService()->swap(object);
                    subSrv->m_dummy.reset();

                    if (subSrv->m_hasAutoConnection)
                    {
                        subSrv->m_connections->disconnect();
                        subSrv->m_connections->connect( object, subSrv->getService(), subSrv->getService()->getObjSrvConnections() );
                    }
                }
                else
                {
                    OSLM_WARN( subSrv->getService()->getID()
                            << "'s object already is '"
                            << subSrv->getService()->getObject()->getID()
                            << "', no need to swap");
                }
            }
        }
    }
    else
    {
        OSLM_INFO("Object "<<objectId<<" not found in managed objects.");
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::removeObjects( ::fwData::Composite::sptr _composite )
{
    BOOST_FOREACH( ::fwData::Composite::ValueType swappedObjectId, _composite->getContainer())
    {
        this->removeObject(swappedObjectId.first);
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::removeObject( const std::string objectId )
{
    if(!m_managerConfiguration->find("object", "id", objectId).empty())
    {
        ConfigurationType conf = m_managerConfiguration->find("object", "id", objectId).at(0);
        const std::string objectType   = conf->getAttributeValue("type");

        SubServicesVecType subServices = m_objectsSubServices[objectId];
        ::fwData::Object::sptr dummyObj;
        dummyObj = ::fwData::factory::New(objectType);
        BOOST_FOREACH( SPTR(SubService) subSrv, subServices )
        {
            OSLM_ASSERT("SubService on " << objectId <<" expired !", subSrv->getService() );
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
                    subSrv->m_connections->disconnect();
                }

                subSrv->getService()->stop();
                ::fwServices::OSR::unregisterService(subSrv->getService());
                subSrv->m_service.reset();
            }
        }
        if(!m_dummyStopMode)
        {
            m_objectsSubServices.erase(objectId);
        }
    }
    else
    {
        OSLM_DEBUG("Object "<<objectId<<" not managed.");
    }
}
//-----------------------------------------------------------------------------

void SwapperSrv::initOnDummyObject( std::string objectId )
{
    SLM_ASSERT( "'objectId' required attribute missing or empty", !objectId.empty() );

    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >() ;

    OSLM_ASSERT(objectId << " not found in composite.", composite->find(objectId) == composite->end());

    ConfigurationType conf = m_managerConfiguration->find("object", "id", objectId).at(0);
    const std::string objectType    = conf->getAttributeValue("type");
    SLM_ASSERT( "'type' required attribute missing or empty", !objectType.empty() );

    // Any subServices have been registered with object.
    if ( m_objectsSubServices.count(objectId) == 0 )
    {
        OSLM_TRACE ( "'"<< objectId << "' nonexistent'");

        ::fwData::Object::sptr dummyObj;
        dummyObj = ::fwData::factory::New(objectType);
        SubServicesVecType subVecSrv;
        std::vector < ConfigurationType > confVec = conf->find("service");
        BOOST_FOREACH( ConfigurationType cfg, confVec )
        {
            ::fwServices::IService::sptr srv = this->add( dummyObj, cfg );
            OSLM_ASSERT("Instantiation Service failed on object "<<objectId, srv);

            SPTR(SubService) subSrv =  SPTR(SubService)( new SubService());
            subSrv->m_config = cfg;
            subSrv->m_service = srv;
            subSrv->m_dummy = dummyObj;

            if ( cfg->getExistingAttributeValue("autoConnect") == "yes" )
            {
                subSrv->m_hasAutoConnection = true;
            }

            subVecSrv.push_back(subSrv);
            subSrv->getService()->start();
        }
        m_objectsSubServices[objectId] = subVecSrv;
    }
}

//-----------------------------------------------------------------------------

} // manager
} // ctrlSelection
