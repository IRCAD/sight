/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwTools/fwID.hpp>

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
            if (m_mode =="startAndUpdate")
            {
            	 subSrv->getService()->update();
            }
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
                OSLM_ASSERT( subSrv->getService()->getID() <<  " is not started ", subSrv->getService()->isStarted());

                OSLM_TRACE("Swapping subService " << subSrv->getService()->getID() << " on "<< objectId );
                if(subSrv->getService()->getObject() != object)
                {
                    subSrv->getService()->swap(object);
                    subSrv->m_dummy.reset();
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
            OSLM_ASSERT( subSrv->getService()->getID() <<  " is not started ", subSrv->getService()->isStarted());
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
            subSrv->getService()->start();
        }
        m_objectsSubServices[objectId] = subVecSrv;
    }
}

//-----------------------------------------------------------------------------

} // manager
} // ctrlSelection
