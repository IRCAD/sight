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

#include "ctrlSelection/manager/SwapperSrv.hpp"

namespace ctrlSelection
{

namespace manager
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::ctrlSelection::IManagerSrv, ::ctrlSelection::manager::SwapperSrv, ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

SwapperSrv::SwapperSrv() throw()
{
    addNewHandledEvent( ::fwComEd::CompositeMsg::MODIFIED_FIELDS );
}

//-----------------------------------------------------------------------------

SwapperSrv::~SwapperSrv() throw()
{}

//-----------------------------------------------------------------------------

void SwapperSrv::updating( ::fwServices::ObjectMsg::csptr message ) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast(message);

    if(compositeMsg && compositeMsg->hasEvent( ::fwComEd::CompositeMsg::MODIFIED_FIELDS ) )
    {
        std::vector< std::string > objectIds = compositeMsg->getEventModifiedFields();
        SLM_ASSERT ("No services configuration!", m_managerConfiguration );

        BOOST_FOREACH( std::string objectId, objectIds)
        {
            BOOST_FOREACH( ConfigurationType cfg, m_managerConfiguration->find("object", "id", objectId))
            {
                this->configureObject(cfg);
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    for( SubServicesMapType::iterator iterMap = m_objectsSubServices.begin(); iterMap != m_objectsSubServices.end(); ++iterMap )
    {
        SubServicesVecType subServices = iterMap->second;
        for( SubServicesVecType::iterator iterVect = subServices.begin(); iterVect != subServices.end(); ++iterVect )
        {
            SubService &subSrv = *iterVect;
            SLM_ASSERT("SubService expired !", subSrv.getService() );
            subSrv.getService()->start();
        }
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::stopping()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    for( SubServicesMapType::iterator iterMap = m_objectsSubServices.begin(); iterMap != m_objectsSubServices.end(); ++iterMap )
    {
        SubServicesVecType subServices = iterMap->second;
        for( SubServicesVecType::iterator iterVect = subServices.begin(); iterVect != subServices.end(); ++iterVect )
        {
            SubService &subSrv = *iterVect;
            SLM_ASSERT("SubService expired !", subSrv.getService() );
            subSrv.getService()->stop();
            ::fwServices::erase(subSrv.getService());
            subSrv.m_service.reset();
        }
    }
    m_objectsSubServices.clear();
}

//-----------------------------------------------------------------------------

void SwapperSrv::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    std::vector < ConfigurationType > vectConfig = m_configuration->find("config");
    SLM_ASSERT("Missing <config> tag!", !vectConfig.empty());
    m_managerConfiguration = vectConfig.at(0);

    ::fwRuntime::ConfigurationElementContainer::Iterator iter;
    for (iter = m_managerConfiguration->begin() ; iter != m_managerConfiguration->end() ; ++iter)
    {
        if ((*iter)->getName() == "object")
        {
            this->configureObject( *iter );
        }
    }
}

//-----------------------------------------------------------------------------

void SwapperSrv::configureObject( ConfigurationType conf )
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

    // Object's not registered in manager and exists in Composite
    if ( m_objectsSubServices.count(objectId) == 0 && object )
    {
        OSLM_ASSERT("ObjectType "<<objectType<<" does not match ObjectType in Composite "<<object->getClassname(), objectType == object->getClassname());
        SubServicesVecType subVecSrv;
        BOOST_FOREACH( ConfigurationType cfg, conf->find("service"))
        {
            ::fwServices::IService::sptr srv = ::fwServices::add( object, cfg );
            OSLM_ASSERT("Instantiation Service failed on object "<<objectId, srv);
            srv->configure();
            SubService subSrv;
            subSrv.m_config = *(cfg->begin());
            subSrv.m_service = srv;
            subVecSrv.push_back(subSrv);
         }
        m_objectsSubServices[objectId] = subVecSrv;
    }
    // Object's already registered in manager
    else if(m_objectsSubServices.count(objectId) == 1)
    {
        // Object exists in Composite, so we swap all associated subServices
        if (object)
        {
            SubServicesVecType subServices = m_objectsSubServices[objectId];
            for( SubServicesVecType::iterator iter = subServices.begin(); iter != subServices.end(); ++iter )
            {
                SubService &subSrv = *iter;

                SLM_ASSERT("SubService expired !", subSrv.getService() );
                OSLM_ASSERT( ::fwTools::UUID::get(subSrv.getService()) <<  " is not started ", subSrv.getService()->isStarted());

                OSLM_TRACE ("Swapping subService " << subSrvId << " on "<< objectId );
                if(subSrv.getService()->getObject() != object)
                {
                    subSrv.getService()->swap(object);
                }
                else
                {
                    OSLM_WARN( ::fwTools::UUID::get(subSrv.getService())
                            << "'s object already is '"
                            << subSrv.getService()->getObject()->getUUID()
                            << "', no need to swap");
                }
            }
        }
        //Object was removed from Composite, so we remove all associated subServices
        else
        {
            SubServicesVecType subServices = m_objectsSubServices[objectId];
            for( SubServicesVecType::iterator iter = subServices.begin(); iter != subServices.end(); ++iter )
            {
                SubService &subSrv = *iter;
                SLM_ASSERT("SubService expired !", subSrv.getService() );
                OSLM_ASSERT( ::fwTools::UUID::get(subSrv.getService()) <<  " is not started ", subSrv.getService()->isStarted());
                subSrv.getService()->stop();
                ::fwServices::erase(subSrv.getService());
                subSrv.m_service.reset();
            }
            m_objectsSubServices.erase(objectId);
        }
    }
    // Object isn't present in the Composite and any subServices have been registered with it.
    else
    {
        OSLM_TRACE ( "'"<< objectId << "' nonexistent'");
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

} // manager
} // ctrlSelection
