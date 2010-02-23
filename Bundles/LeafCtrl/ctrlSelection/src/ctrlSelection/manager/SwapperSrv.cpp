/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwTools/UUID.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <fwComEd/CompositeMsg.hpp>
#include <fwData/Composite.hpp>
#include <fwData/Acquisition.hpp>

#include "ctrlSelection/manager/SwapperSrv.hpp"

namespace ctrlSelection
{

namespace manager
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::ctrlSelection::IManagerSrv, ::ctrlSelection::manager::SwapperSrv, ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

SwapperSrv::SwapperSrv() throw() : m_dummyStopMode(false)
{
    addNewHandledEvent( ::fwComEd::CompositeMsg::ADDED_FIELDS );
    addNewHandledEvent( ::fwComEd::CompositeMsg::REMOVED_FIELDS );
    addNewHandledEvent( ::fwComEd::CompositeMsg::SWAPPED_FIELDS );
}

//-----------------------------------------------------------------------------

SwapperSrv::~SwapperSrv() throw()
{}

//-----------------------------------------------------------------------------

void SwapperSrv::updating( ::fwServices::ObjectMsg::csptr message ) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast(message);
    SLM_FATAL_IF("Received message must be compositeMsg", compositeMsg == 0 );

    if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::ADDED_FIELDS ) )
    {
        ::fwData::Composite::sptr fields = compositeMsg->getAddedFields();
        this->addObjects( fields );
    }

    if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::REMOVED_FIELDS ) )
    {
        ::fwData::Composite::sptr fields = compositeMsg->getRemovedFields();
        this->removeObjects( fields );
    }

    if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::SWAPPED_FIELDS ) )
    {
        ::fwData::Composite::sptr fields = compositeMsg->getSwappedNewFields();
        this->swapObjects( fields );
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SwapperSrv::updating() throw ( ::fwTools::Failed )
{}

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
        BOOST_FOREACH( SPTR(SubService) subSrv, subServices )
        {
            OSLM_ASSERT("SubService on "<< iterMap->first <<" expired !", subSrv->getService() );
            subSrv->getService()->stop();
            ::fwServices::erase(subSrv->getService());
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
        SLM_ASSERT("Wrong type mode", (mode == "dummy" ) || (mode == "stop" ));
        m_dummyStopMode = (mode == "dummy" );
    }

    std::vector < ConfigurationType > vectConfig = m_configuration->find("config");
    SLM_ASSERT("Missing <config> tag!", !vectConfig.empty());
    m_managerConfiguration = vectConfig.at(0);
}

//-----------------------------------------------------------------------------

void SwapperSrv::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    if(m_dummyStopMode)
    {
        ::fwRuntime::ConfigurationElementContainer::Iterator iter;
        for (iter = m_managerConfiguration->begin() ; iter != m_managerConfiguration->end() ; ++iter)
        {
            if ((*iter)->getName() == "object")
            {
                this->initOnDummyObject( *iter );
            }
        }
    }
    BOOST_FOREACH( SubServicesMapType::value_type subServicesElt, m_objectsSubServices)
    {
        SubServicesVecType subServices = subServicesElt.second;
        BOOST_FOREACH( SPTR(SubService) subSrv, subServices )
        {
            OSLM_ASSERT("SubService on " << subServicesElt.first <<" expired !", subSrv->getService() );
            subSrv->getService()->start();
        }
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::addObjects( ::fwData::Composite::sptr _composite )
{
    BOOST_FOREACH( ::fwData::Composite::Container::value_type  addedObjectId, _composite->getRefMap())
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

void SwapperSrv::addObject( const std::string objectId, ::fwTools::Object::sptr object )
{
    if(!m_managerConfiguration->find("object", "id", objectId).empty())
    {
        ConfigurationType conf = m_managerConfiguration->find("object", "id", objectId).at(0);
        const std::string objectType   = conf->getAttributeValue("type");

        OSLM_ASSERT("ObjectType "<<objectType<<" does not match ObjectType in Composite "<<object->getClassname(),
                objectType == object->getClassname());
        SubServicesVecType subVecSrv;
        BOOST_FOREACH( ConfigurationType cfg, conf->find("service"))
        {
            ::fwServices::IService::sptr srv = ::fwServices::add( object, cfg );
            OSLM_ASSERT("Instantiation Service failed on object "<<objectId, srv);
            srv->configure();
            SPTR(SubService) subSrv =  SPTR(SubService)( new SubService());
            subSrv->m_config = cfg;
            subSrv->m_service = srv;
            subVecSrv.push_back(subSrv);
            subSrv->getService()->start();
        }
        m_objectsSubServices[objectId] = subVecSrv;
    }
    else
    {
        OSLM_WARN("Sorry, object "<<objectId<<" not found in configuration.");
    }
}
//-----------------------------------------------------------------------------

void SwapperSrv::swapObjects( ::fwData::Composite::sptr _composite )
{
    BOOST_FOREACH( ::fwData::Composite::Container::value_type  swappedObjectId, _composite->getRefMap())
    {
        this->swapObject(swappedObjectId.first, swappedObjectId.second);
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::swapObject(const std::string objectId, ::fwTools::Object::sptr object)
{
    if(m_objectsSubServices.find(objectId) != m_objectsSubServices.end())
    {
        BOOST_FOREACH( ConfigurationType cfg, m_managerConfiguration->find("object", "id", objectId))
        {
            SubServicesVecType subServices = m_objectsSubServices[objectId];
            BOOST_FOREACH( SPTR(SubService) subSrv, subServices )
            {
                OSLM_ASSERT("SubService on " << objectId <<" expired !", subSrv->getService() );
                OSLM_ASSERT( ::fwTools::UUID::get(subSrv->getService()) <<  " is not started ", subSrv->getService()->isStarted());

                OSLM_TRACE("Swapping subService " << ::fwTools::UUID::get(subSrv->getService()) << " on "<< objectId );
                if(subSrv->getService()->getObject() != object)
                {
                    subSrv->getService()->swap(object);
                    subSrv->m_dummy.reset();
                }
                else
                {
                    OSLM_WARN( ::fwTools::UUID::get(subSrv->getService())
                            << "'s object already is '"
                            << subSrv->getService()->getObject()->getUUID()
                            << "', no need to swap");
                }
            }
        }
    }
    else
    {
        OSLM_WARN("Object "<<objectId<<" not found in managed objects.");
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::removeObjects( ::fwData::Composite::sptr _composite )
{
    BOOST_FOREACH( ::fwData::Composite::Container::value_type  swappedObjectId, _composite->getRefMap())
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
        ::fwTools::Object::sptr dummyObj = ::fwTools::Factory::New(objectType);
        BOOST_FOREACH( SPTR(SubService) subSrv, subServices )
        {
            OSLM_ASSERT("SubService on " << objectId <<" expired !", subSrv->getService() );
            OSLM_ASSERT( ::fwTools::UUID::get(subSrv->getService()) <<  " is not started ", subSrv->getService()->isStarted());
            if(m_dummyStopMode)
            {
                subSrv->getService()->swap(dummyObj);
                subSrv->m_dummy = dummyObj;
            }
            else
            {
                subSrv->getService()->stop();
                ::fwServices::erase(subSrv->getService());
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
        OSLM_WARN("Object "<<objectId<<" not found in managed objects.");
    }
}
//-----------------------------------------------------------------------------

void SwapperSrv::initOnDummyObject( ConfigurationType conf )
{
    SLM_ASSERT("Missing <object> tag!", conf->getName() == "object");
     ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >() ;

     const std::string objectId      = conf->getAttributeValue("id");
     const std::string objectType    = conf->getAttributeValue("type");

     SLM_ASSERT( "'objectId' required attribute missing or empty", !objectId.empty() );
     SLM_ASSERT( "'type' required attribute missing or empty", !objectType.empty() );

     const unsigned int compositeObjectCount = composite->getRefMap().count(objectId);

     OSLM_TRACE_IF(objectId << " not found in composite.", !(compositeObjectCount == 1));

     ::fwTools::Object::sptr object;
     if (compositeObjectCount)
     {
         object = ::fwTools::Object::dynamicCast(composite->getRefMap()[objectId]);
     }

     // Any subServices have been registered with object.
     if ( m_objectsSubServices.count(objectId) == 0 )
     {
         // Object exists in Composite, so we create all associated subServices with it
         if(object)
         {
             SubServicesVecType subVecSrv;
             BOOST_FOREACH( ConfigurationType cfg, conf->find("service"))
             {
                 ::fwServices::IService::sptr srv = ::fwServices::add( object, cfg );
                 OSLM_ASSERT("Instantiation Service failed on object "<<object, srv);
                 srv->configure();
                 SPTR(SubService) subSrv =  SPTR(SubService)( new SubService());
                 subSrv->m_config = cfg;
                 subSrv->m_service = srv;
                 subVecSrv.push_back(subSrv);
             }
             m_objectsSubServices[objectId] = subVecSrv;
         }
         // Object isn't present in the Composite, so we create all associated subServices with a dummy object
         else
         {
             OSLM_TRACE ( "'"<< objectId << "' nonexistent'");

             ::fwTools::Object::sptr dummyObj = ::fwTools::Factory::New(objectType);
             SubServicesVecType subVecSrv;
             BOOST_FOREACH( ConfigurationType cfg, conf->find("service"))
             {
                 ::fwServices::IService::sptr srv = ::fwServices::add( dummyObj, cfg );
                 OSLM_ASSERT("Instantiation Service failed on object "<<objectId, srv);
                 srv->configure();
                 SPTR(SubService) subSrv =  SPTR(SubService)( new SubService());
                 subSrv->m_config = cfg;
                 subSrv->m_service = srv;
                 subSrv->m_dummy = dummyObj;
                 subVecSrv.push_back(subSrv);
             }
             m_objectsSubServices[objectId] = subVecSrv;
         }
     }
}

//-----------------------------------------------------------------------------

} // manager
} // ctrlSelection
