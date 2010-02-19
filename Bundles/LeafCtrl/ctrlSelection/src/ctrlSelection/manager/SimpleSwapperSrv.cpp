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

#include "ctrlSelection/manager/SimpleSwapperSrv.hpp"

namespace ctrlSelection
{

namespace manager
{

//-----------------------------------------------------------------------------

REGISTER_SERVICE( ::ctrlSelection::IManagerSrv, ::ctrlSelection::manager::SimpleSwapperSrv, ::fwData::Composite ) ;

//-----------------------------------------------------------------------------

SimpleSwapperSrv::SimpleSwapperSrv() throw()
{
    addNewHandledEvent( ::fwComEd::CompositeMsg::ADDED_FIELDS );
    addNewHandledEvent( ::fwComEd::CompositeMsg::REMOVED_FIELDS );
    addNewHandledEvent( ::fwComEd::CompositeMsg::SWAPPED_FIELDS );
}

//-----------------------------------------------------------------------------

SimpleSwapperSrv::~SimpleSwapperSrv() throw()
{}

//-----------------------------------------------------------------------------

void SimpleSwapperSrv::updating( ::fwServices::ObjectMsg::csptr message ) throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();

    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast(message);
    SLM_FATAL_IF("Received message must be compositeMsg", compositeMsg == 0 );

    if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::ADDED_FIELDS ) )
    {
        ::fwData::Composite::sptr fields = compositeMsg->getAddedFields();
        this->addedObjects( fields );
    }

    if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::REMOVED_FIELDS ) )
    {
        ::fwData::Composite::sptr fields = compositeMsg->getRemovedFields();
        this->removedObjects( fields );
    }

    if ( compositeMsg->hasEvent( ::fwComEd::CompositeMsg::SWAPPED_FIELDS ) )
    {
        ::fwData::Composite::sptr fields = compositeMsg->getSwappedNewFields();
        this->swappedObjects( fields );
    }
}


//-----------------------------------------------------------------------------

void SimpleSwapperSrv::addedObjects( ::fwData::Composite::sptr _composite )
{
    BOOST_FOREACH( ::fwData::Composite::Container::value_type  swappedObjectId, _composite->getRefMap())
    {
        OSLM_ASSERT("Sorry, object "<<swappedObjectId.first<<" not found in composite.",
                m_objectsSubServices.find(swappedObjectId.first) != m_objectsSubServices.end() );

        SubServicesVecType subServices = m_objectsSubServices[swappedObjectId.first];
        BOOST_FOREACH( SubServicesVecType::value_type subSrv, subServices )
        {
            OSLM_FATAL_IF("Service " << subSrv->m_serviceUID << " doesn't exist.",
                    ! ::fwTools::UUID::exist(subSrv->m_serviceUID, ::fwTools::UUID::SIMPLE ));
            ::fwServices::IService::sptr service = ::fwServices::get( subSrv->m_serviceUID ) ;
            service->swap(swappedObjectId.second);
        }
    }
}

//-----------------------------------------------------------------------------

void SimpleSwapperSrv::swappedObjects( ::fwData::Composite::sptr _composite )
{
    BOOST_FOREACH( ::fwData::Composite::Container::value_type  swappedObjectId, _composite->getRefMap())
    {
        OSLM_ASSERT("Sorry, object "<<swappedObjectId.first<<" not found in composite.",
                m_objectsSubServices.find(swappedObjectId.first) != m_objectsSubServices.end() );

        SubServicesVecType subServices = m_objectsSubServices[swappedObjectId.first];
        BOOST_FOREACH( SubServicesVecType::value_type subSrv, subServices )
        {
            OSLM_FATAL_IF("Service " << subSrv->m_serviceUID << " doesn't exist.",
                    ! ::fwTools::UUID::exist(subSrv->m_serviceUID, ::fwTools::UUID::SIMPLE ));
            ::fwServices::IService::sptr service = ::fwServices::get( subSrv->m_serviceUID ) ;
            service->swap(swappedObjectId.second);
        }
    }
}

//-----------------------------------------------------------------------------

void SimpleSwapperSrv::removedObjects( ::fwData::Composite::sptr _composite )
{
    BOOST_FOREACH( ::fwData::Composite::Container::value_type  swappedObjectId, _composite->getRefMap())
    {
        OSLM_ASSERT("Sorry, object "<<swappedObjectId.first<<" not found in composite.",
                m_objectsSubServices.find(swappedObjectId.first) != m_objectsSubServices.end() );

        SubServicesVecType subServices = m_objectsSubServices[swappedObjectId.first];
        BOOST_FOREACH( SubServicesVecType::value_type subSrv, subServices )
        {
            OSLM_ASSERT("Object "<<subSrv->m_dummyObjectUID<<" doesn't exist.",
                    ::fwTools::UUID::exist(subSrv->m_dummyObjectUID, ::fwTools::UUID::SIMPLE ));
            ::fwData::Object::sptr dummyObj = ::fwTools::UUID::get< ::fwData::Object >(subSrv->m_dummyObjectUID, ::fwTools::UUID::SIMPLE );

            OSLM_FATAL_IF("Service " << subSrv->m_serviceUID << " doesn't exist.",
                    ! ::fwTools::UUID::exist(subSrv->m_serviceUID, ::fwTools::UUID::SIMPLE ));
            ::fwServices::IService::sptr service = ::fwServices::get( subSrv->m_serviceUID ) ;
            service->swap(dummyObj);
        }
    }
}

//-----------------------------------------------------------------------------

void SimpleSwapperSrv::starting()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SimpleSwapperSrv::stopping()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
}

//-----------------------------------------------------------------------------

void SimpleSwapperSrv::configuring()  throw ( ::fwTools::Failed )
{
    SLM_TRACE_FUNC();
    std::vector < ConfigurationType > vectConfig = m_configuration->find("config");
    SLM_ASSERT("Missing <config> tag!", !vectConfig.empty());
    m_managerConfiguration = vectConfig.at(0);
    m_objectsSubServices.clear();
    BOOST_FOREACH( ConfigurationType conf, m_managerConfiguration->find("object"))
    {
        SLM_ASSERT("'dummyObjectUID' attribute required", conf->hasAttribute("dummyObjectUID"));
        const std::string objectId      = conf->getAttributeValue("id");
        const std::string dummyObjectUID = conf->getExistingAttributeValue("dummyObjectUID");

        OSLM_ASSERT("Object "<<dummyObjectUID<<" doesn't exist.",
                ::fwTools::UUID::exist(dummyObjectUID, ::fwTools::UUID::SIMPLE ));

        SubServicesVecType subVecSrv;
        BOOST_FOREACH( ConfigurationType cfg, conf->find("service"))
        {
            SPTR(SubService) subSrv = SPTR(SubService)( new SubService());
            subSrv->m_dummyObjectUID = dummyObjectUID;

            SLM_ASSERT("'uid' attribute required", cfg->hasAttribute("uid"));
            subSrv->m_serviceUID = cfg->getExistingAttributeValue("uid");
            OSLM_ASSERT("Service "<<subSrv->m_serviceUID<<" doesn't exist.",
                    ::fwTools::UUID::exist(subSrv->m_serviceUID, ::fwTools::UUID::SIMPLE ));

            subVecSrv.push_back(subSrv);
        }
        m_objectsSubServices[objectId] = subVecSrv;
    }
}

//-----------------------------------------------------------------------------

void SimpleSwapperSrv::reconfiguring()  throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SimpleSwapperSrv::updating() throw ( ::fwTools::Failed )
{}

//-----------------------------------------------------------------------------

void SimpleSwapperSrv::info( std::ostream &_sstream )
{}

//-----------------------------------------------------------------------------

} // manager
} // ctrlSelection
