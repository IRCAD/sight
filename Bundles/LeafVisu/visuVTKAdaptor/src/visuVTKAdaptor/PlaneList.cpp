/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwData/PlaneList.hpp>
#include <fwData/Plane.hpp>
#include <fwData/Boolean.hpp>


#include <fwComEd/PlaneMsg.hpp>
#include <fwComEd/PlaneListMsg.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/registry/ObjectService.hpp>

#include <vtkCommand.h>
#include <vtkCellPicker.h>
#include <vtkActor.h>
#include <vtkAssemblyNode.h>
#include <vtkAssemblyPath.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

#include "visuVTKAdaptor/Plane.hpp"
#include "visuVTKAdaptor/PlaneList.hpp"
#include <fwServices/IEditionService.hpp>

REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::PlaneList, ::fwData::PlaneList ) ;

namespace visuVTKAdaptor
{



void notifyDeletePlane( ::fwData::PlaneList::sptr planeList, ::fwServices::IService* _service, ::fwData::Plane::sptr plane )
{
    ::fwComEd::PlaneListMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::PlaneListMsg::DESELECT_ALL_PLANES );
    msg->addEvent( ::fwComEd::PlaneListMsg::REMOVE_PLANE, plane );
    SLM_ASSERT("NULL Service", _service);

    ::fwServices::IEditionService::notify( _service->getSptr(), planeList, msg, ::fwServices::ComChannelService::NOTIFY_SOURCE );
}

class vtkPlaneDeleteCallBack : public vtkCommand
{

public :
    static vtkPlaneDeleteCallBack *New( ::fwRenderVTK::IVtkAdaptorService *service)
    { return new vtkPlaneDeleteCallBack(service); }

    vtkPlaneDeleteCallBack( ::fwRenderVTK::IVtkAdaptorService *service )
    : m_service(service),
      m_picker( vtkCellPicker::New() ),
      m_propCollection( vtkPropCollection::New() )
    {
        m_picker->PickFromListOn();
        m_picker->SetTolerance(0.001);

        m_display[2]=0.0;
    }

    ~vtkPlaneDeleteCallBack( )
    {
        m_picker->Delete();
        m_picker = NULL;

        m_propCollection->Delete();
        m_propCollection = NULL;
    }


    void fillPickList()
    {
        m_picker->InitializePickList();
        m_propCollection->RemoveAllItems();
        m_service->getAllSubProps(m_propCollection);
        m_propCollection->InitTraversal();

        vtkProp *prop;

        while ( (prop = m_propCollection->GetNextProp()) )
        {
            m_picker->AddPickList(prop);
        }
    }

    virtual void Execute( vtkObject *caller, unsigned long eventId, void *)
    {
        int pos[2];
        m_service->getInteractor()->GetLastEventPosition(pos);

        if ( eventId == vtkCommand::RightButtonPressEvent )
        {
            std::copy(pos, pos+1, m_lastPos);
            m_display[0] = pos[0];
            m_display[1] = pos[1];

            this->fillPickList();
            if (m_picker->Pick( m_display , m_service->getRenderer() ) )
            {
                if(getSelectedPlane())
                {
                    SetAbortFlag(1);
                }
                else
                {
                    m_pickedPlane.reset();
                }
            }
        }
        else if ( eventId == vtkCommand::RightButtonReleaseEvent && std::equal(pos, pos+1, m_lastPos) && !m_pickedPlane.expired() )
        {
            // backup of plane
            ::fwData::Plane::sptr planeBackup(m_pickedPlane);

            ::fwData::PlaneList::sptr planeList = m_service->getObject< ::fwData::PlaneList >();
            planeList->getRefPlanes().erase
            (
                    std::find( planeList->getRefPlanes().begin(), planeList->getRefPlanes().end(), m_pickedPlane.lock())
            );
            notifyDeletePlane(planeList, m_service, planeBackup);
        }
    }
    bool getSelectedPlane()
    {
        bool isFind = false;
        vtkPropCollection *propc = m_picker->GetActors();
        vtkProp *prop;

        propc->InitTraversal();
        while ( (prop = propc->GetNextProp()) )
        {
            m_pickedPlane = ::fwData::Plane::dynamicCast(m_service->getAssociatedObject(prop,1));
            if( !m_pickedPlane.expired() )
            {
                ::fwData::PlaneList::sptr planeList = m_service->getObject< ::fwData::PlaneList >();
                if(!planeList->getRefPlanes().empty())
                {
                    ::fwData::PlaneList::PlaneListContainer::iterator itr = std::find( planeList->getRefPlanes().begin(), planeList->getRefPlanes().end(), m_pickedPlane.lock());
                    if(itr != planeList->getRefPlanes().end() )
                    {
                        isFind = true;
                        break;
                    }
                }
            }
        }
        return isFind;
    }

protected :
    ::fwRenderVTK::IVtkAdaptorService *m_service;
    vtkPicker * m_picker;
    vtkPropCollection * m_propCollection;
    double m_display[3];
    int m_lastPos[2];
    ::fwData::Plane::wptr m_pickedPlane;

};

//------------------------------------------------------------------------------

PlaneList::PlaneList() throw()
    : m_planeCollectionId("")
{
    addNewHandledEvent( ::fwComEd::PlaneListMsg::ADD_PLANE );
    addNewHandledEvent( ::fwComEd::PlaneListMsg::REMOVE_PLANE );
    addNewHandledEvent( ::fwComEd::PlaneListMsg::PLANELIST_VISIBILITY );
}

//------------------------------------------------------------------------------

PlaneList::~PlaneList() throw()
{
}

//------------------------------------------------------------------------------

void PlaneList::configuring() throw(fwTools::Failed)
{

    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setPickerId( m_configuration->getAttributeValue("picker") );
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
    this->setPlaneCollectionId( m_configuration->getAttributeValue("planecollection") );
}

//------------------------------------------------------------------------------

void PlaneList::doStart() throw(fwTools::Failed)
{
    m_rightButtonCommand = vtkPlaneDeleteCallBack::New(this);
    this->getInteractor()->AddObserver( "RightButtonPressEvent" , m_rightButtonCommand, 1 );
    this->getInteractor()->AddObserver( "RightButtonReleaseEvent" , m_rightButtonCommand, 1 );

    this->doUpdate();
}

//------------------------------------------------------------------------------

void PlaneList::doUpdate() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwData::PlaneList::sptr planeList = this->getObject< ::fwData::PlaneList >();

    bool showPlanes;
    showPlanes = planeList->getField("ShowPlanes", ::fwData::Boolean::New(true))->value();
    if(showPlanes)
    {
        BOOST_FOREACH( ::fwData::Plane::sptr plane, planeList->getPlanes() )
        {
            ::fwRenderVTK::IVtkAdaptorService::sptr servicePlane =
                    ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >
                    ( plane, "::visuVTKAdaptor::Plane" );
            SLM_ASSERT("servicePlane not instanced", servicePlane);

            servicePlane->setRenderService(this->getRenderService());
            servicePlane->setRenderId( this->getRenderId() );
            servicePlane->setPickerId( this->getPickerId() );

            if (!m_planeCollectionId.empty())
            {
                Plane::dynamicCast(servicePlane)->setVtkPlaneCollection( this->getVtkObject(m_planeCollectionId) );
            }
            servicePlane->start();

            this->registerService(servicePlane);
        }
    }
}

//------------------------------------------------------------------------------

void PlaneList::doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwComEd::PlaneListMsg::csptr planeListMsg = ::fwComEd::PlaneListMsg::dynamicConstCast( msg );
    if ( planeListMsg )
    {
        if (planeListMsg->hasEvent( ::fwComEd::PlaneListMsg::ADD_PLANE )
                || planeListMsg->hasEvent( ::fwComEd::PlaneListMsg::REMOVE_PLANE) )
        {
            this->doStop();
            this->doStart();
        }
        else if ( planeListMsg->hasEvent( ::fwComEd::PlaneListMsg::PLANELIST_VISIBILITY ) )
        {
            ::fwData::PlaneList::sptr planeList = this->getObject< ::fwData::PlaneList >();

            this->doStop();
            bool showPlanes;
            showPlanes = planeList->getField("ShowPlanes", ::fwData::Boolean::New(true))->value();
            OSLM_INFO( "Receive event ShowPlanes : " << showPlanes );
            if(showPlanes)
            {
                this->doStart();
            }
        }
    }
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void PlaneList::doSwap() throw(fwTools::Failed)
{
    this->doStop();

    ::fwData::PlaneList::sptr planeList = this->getObject< ::fwData::PlaneList >();
    bool showPlanes;
    showPlanes = planeList->getField("ShowPlanes", ::fwData::Boolean::New(true))->value();
    if(showPlanes)
    {
        this->doStart();
    }
}

//------------------------------------------------------------------------------

void PlaneList::doStop() throw(fwTools::Failed)
{
    if ( m_rightButtonCommand ) // can be not instanciated
    {
        this->getInteractor()->RemoveObserver(m_rightButtonCommand);
        m_rightButtonCommand->Delete();
        m_rightButtonCommand = 0;
    }

    this->unregisterServices();
}


} //namespace visuVTKAdaptor
