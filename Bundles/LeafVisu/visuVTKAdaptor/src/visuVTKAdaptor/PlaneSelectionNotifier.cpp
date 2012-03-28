/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwComEd/CompositeMsg.hpp>
#include <fwComEd/PlaneListMsg.hpp>
#include <fwComEd/PlaneMsg.hpp>

#include <fwData/None.hpp>
#include <fwData/Plane.hpp>
#include <fwData/PlaneList.hpp>
#include <fwData/Boolean.hpp>

#include <fwServices/Factory.hpp>
#include <fwServices/registry/ObjectService.hpp>
#include <fwServices/macros.hpp>

#include <fwTools/Object.hpp>

#include "visuVTKAdaptor/PlaneSelectionNotifier.hpp"
#include <fwServices/IEditionService.hpp>



REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::PlaneSelectionNotifier, ::fwData::Composite ) ;

namespace visuVTKAdaptor
{

PlaneSelectionNotifier::PlaneSelectionNotifier() throw()
{
    addNewHandledEvent( ::fwComEd::CompositeMsg::MODIFIED_KEYS );
    addNewHandledEvent( ::fwComEd::PlaneListMsg::ADD_PLANE );
    addNewHandledEvent( ::fwComEd::PlaneListMsg::REMOVE_PLANE);
    addNewHandledEvent( ::fwComEd::PlaneListMsg::PLANELIST_VISIBILITY);
    addNewHandledEvent( ::fwComEd::PlaneListMsg::DESELECT_ALL_PLANES );
    addNewHandledEvent( ::fwComEd::PlaneMsg::START_PLANE_INTERACTION );
    addNewHandledEvent( ::fwComEd::PlaneMsg::DESELECT_PLANE );
}

//------------------------------------------------------------------------------

PlaneSelectionNotifier::~PlaneSelectionNotifier() throw()
{
}

//------------------------------------------------------------------------------

void PlaneSelectionNotifier::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    assert(m_configuration->getName() == "config");
    assert(m_configuration->hasAttribute("planelist"));
    assert(m_configuration->hasAttribute("planeselection"));

    this->setPlaneListId( m_configuration->getAttributeValue("planelist") );
    this->setPlaneSelectionId( m_configuration->getAttributeValue("planeselection") );
}

//------------------------------------------------------------------------------

void PlaneSelectionNotifier::doStart() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    SLM_ASSERT("m_currentPlaneList should be expired", m_currentPlaneList.expired());

    if (composite->find(m_planeListId) != composite->end())
    {
        m_currentPlaneList = ::fwData::PlaneList::dynamicCast(composite->getContainer()[m_planeListId]);
    }

    ::fwData::PlaneList::sptr planeList = m_currentPlaneList.lock();
    if(planeList)
    {
        //::fwServices::registerCommunicationChannel(planeList, this->getSptr() )->start();
        ::fwServices::ComChannelService::sptr communicationChannelService;
        ::fwServices::IService::sptr          service;
        service = ::fwServices::registerCommunicationChannel(planeList, this->getSptr());
        communicationChannelService = ::fwServices::ComChannelService::dynamicCast(service);

        communicationChannelService->setPriority(0.2);
        communicationChannelService->start();

        BOOST_FOREACH( ::fwData::Plane::sptr plane, planeList->getPlanes() )
        {
            ::fwServices::registerCommunicationChannel(plane, this->getSptr() )->start();
        }
    }
}

//------------------------------------------------------------------------------

void PlaneSelectionNotifier::doUpdate() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void PlaneSelectionNotifier::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void PlaneSelectionNotifier::doStop() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    ::fwData::PlaneList::sptr planeList = m_currentPlaneList.lock();
    if(planeList)
    {

        BOOST_FOREACH( ::fwData::Plane::sptr plane, planeList->getPlanes() )
        {
            ::fwServices::unregisterCommunicationChannel(plane, this->getSptr() );
        }

        ::fwServices::unregisterCommunicationChannel(planeList, this->getSptr() );

        m_currentPlaneList.reset();
    }
}

//------------------------------------------------------------------------------

void PlaneSelectionNotifier::doUpdate( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwComEd::PlaneListMsg::csptr planeListMsg = ::fwComEd::PlaneListMsg::dynamicConstCast( msg );
    ::fwComEd::PlaneMsg::csptr planeMsg         = ::fwComEd::PlaneMsg::dynamicConstCast( msg );
    ::fwComEd::CompositeMsg::csptr compositeMsg = ::fwComEd::CompositeMsg::dynamicConstCast( msg );

    if ( compositeMsg )
    {
        SLM_ASSERT( "The received message is not an MODIFIED_KEYS event (CompositeMsg)", compositeMsg->hasEvent( ::fwComEd::CompositeMsg::MODIFIED_KEYS ) );

        std::vector< std::string > objectIds = compositeMsg->getModifiedKeys();

        if (std::find(objectIds.begin(), objectIds.end(), m_planeListId) != objectIds.end())
        {
            this->doStop();
            this->doStart();
            this->deselectPlane();
        }
    }
    else if ( planeListMsg )
    {
        ::fwData::PlaneList::sptr planeList = ::fwData::PlaneList::dynamicCast(planeListMsg->getSubject().lock());
        SLM_ASSERT("Messsage's subject should be a PlaneList", planeList);
        bool showPlanes;
        showPlanes = planeList->getField("ShowPlanes", ::fwData::Boolean::New(true));

        if(!planeList->getRefPlanes().empty())
        {
            if (planeListMsg->hasEvent( ::fwComEd::PlaneListMsg::ADD_PLANE))
            {
                //New plane selected is the last one in planeList
                ::fwData::Plane::sptr plane = *(planeList->getRefPlanes().rbegin());
                this->selectPlane(plane);

                ::fwServices::registerCommunicationChannel(plane, this->getSptr() )->start();
            }
            else if(showPlanes && planeListMsg->hasEvent( ::fwComEd::PlaneListMsg::PLANELIST_VISIBILITY ))
            {
                //New plane selected is the last one in planeList
                ::fwData::Plane::sptr plane = *(planeList->getRefPlanes().rbegin());
                this->selectPlane(plane);
            }
        }
        if (planeListMsg->hasEvent( ::fwComEd::PlaneListMsg::REMOVE_PLANE))
        {
            //Remove comChannel
            ::fwTools::Object::csptr dataInfo = planeListMsg->getDataInfo(::fwComEd::PlaneListMsg::REMOVE_PLANE);

            SLM_ASSERT("Sorry, Missing data info", dataInfo);
            ::fwData::Plane::sptr plane = ::fwData::Plane::dynamicCast(::fwTools::Object::constCast(dataInfo));

            ::fwServices::unregisterCommunicationChannel(plane, this->getSptr() );
        }
        if ( (!showPlanes && planeListMsg->hasEvent( ::fwComEd::PlaneListMsg::PLANELIST_VISIBILITY ))
                || planeListMsg->hasEvent( ::fwComEd::PlaneListMsg::DESELECT_ALL_PLANES ) )
        {
            this->deselectPlane();
        }
    }
    else if (planeMsg)
    {
        if (planeMsg->hasEvent( ::fwComEd::PlaneMsg::START_PLANE_INTERACTION ))
        {
            this->selectPlane(::fwData::Object::dynamicCast(planeMsg->getSubject().lock()));
        }
        if (planeMsg->hasEvent( ::fwComEd::PlaneMsg::DESELECT_PLANE ))
        {
            this->deselectPlane();
        }
    }
}

//------------------------------------------------------------------------------

void PlaneSelectionNotifier::selectPlane( ::fwData::Object::sptr plane )
{
    SLM_TRACE_FUNC();
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    ::fwData::Object::sptr oldPlane = composite->getContainer()[m_planeSelectionId];

    if (plane && plane != oldPlane)
    {
        std::vector< std::string > modifiedFields;
        modifiedFields.push_back(m_planeSelectionId);

        std::vector< ::fwData::Object::sptr > oldObjects;
        oldObjects.push_back( oldPlane );

        ::fwComEd::CompositeMsg::NewSptr compositeMsg;
        compositeMsg->addModifiedKeysEvent(modifiedFields,oldObjects);
        composite->getContainer()[m_planeSelectionId] = plane;
        ::fwServices::IEditionService::notify(this->getSptr(), composite, compositeMsg);
    }
}

void PlaneSelectionNotifier::deselectPlane()
{
     SLM_TRACE_FUNC();
     ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
     if ( ! ::fwData::None::dynamicCast(composite->getContainer()[m_planeSelectionId]))
     {
        std::vector< ::fwData::Object::sptr > oldObjects;

        ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
        oldObjects.push_back( composite->getContainer()[m_planeSelectionId] );
        composite->getContainer()[m_planeSelectionId].reset();
        composite->getContainer()[m_planeSelectionId] = ::fwData::None::New();

        std::vector< std::string > modifiedFields;
        modifiedFields.push_back(m_planeSelectionId);

        ::fwComEd::CompositeMsg::NewSptr compositeMsg;
        compositeMsg->addModifiedKeysEvent(modifiedFields,oldObjects);

        ::fwServices::IEditionService::notify(this->getSptr(), composite, compositeMsg);
     }
}


} //namespace visuVTKAdaptor

