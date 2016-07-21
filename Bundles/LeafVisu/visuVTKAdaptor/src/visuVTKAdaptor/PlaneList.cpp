/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef ANDROID

#include "visuVTKAdaptor/Plane.hpp"
#include "visuVTKAdaptor/PlaneList.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Plane.hpp>
#include <fwData/PlaneList.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <vtkActor.h>
#include <vtkAssemblyNode.h>
#include <vtkAssemblyPath.h>
#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::PlaneList, ::fwData::PlaneList );

namespace visuVTKAdaptor
{

const ::fwCom::Signals::SignalKeyType s_SELECTED_SIG = "selected";

const ::fwCom::Slots::SlotKeyType s_UPDATE_SELECTION_SLOT = "updateSelection";
const ::fwCom::Slots::SlotKeyType s_UPDATE_PLANES_SLOT    = "updatePlanes";
const ::fwCom::Slots::SlotKeyType s_SHOW_PLANES_SLOT      = "showPlanes";

//------------------------------------------------------------------------------


void notifyDeletePlane( ::fwData::PlaneList::sptr planeList, ::fwData::Plane::sptr plane )
{
    auto sig = planeList->signal< ::fwData::PlaneList::PlaneRemovedSignalType >(
        ::fwData::PlaneList::s_PLANE_REMOVED_SIG );
    sig->asyncEmit(plane);
}

class vtkPlaneDeleteCallBack : public vtkCommand
{

public:
    static vtkPlaneDeleteCallBack* New( ::fwRenderVTK::IVtkAdaptorService* service)
    {
        return new vtkPlaneDeleteCallBack(service);
    }

    vtkPlaneDeleteCallBack( ::fwRenderVTK::IVtkAdaptorService* service ) :
        m_service(service),
        m_picker( vtkCellPicker::New() ),
        m_propCollection( vtkPropCollection::New() )
    {
        m_lastPos[0] = -1;
        m_lastPos[1] = -1;
        m_picker->PickFromListOn();
        m_picker->SetTolerance(0.001);

        m_display[2] = 0.0;
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

        vtkProp* prop;

        while ( (prop = m_propCollection->GetNextProp()) )
        {
            m_picker->AddPickList(prop);
        }
    }

    virtual void Execute( vtkObject* caller, unsigned long eventId, void*)
    {
        int pos[2];
        m_service->getInteractor()->GetLastEventPosition(pos);

        if ( eventId == vtkCommand::RightButtonPressEvent )
        {
            std::copy(pos, pos+1, m_lastPos);
            m_display[0] = pos[0];
            m_display[1] = pos[1];

            this->fillPickList();
            if (m_picker->Pick( m_display, m_service->getRenderer() ) )
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
        else if ( eventId == vtkCommand::RightButtonReleaseEvent &&
                  std::equal(pos, pos+1, m_lastPos) && !m_pickedPlane.expired() )
        {
            // backup of plane
            ::fwData::Plane::sptr planeBackup(m_pickedPlane);

            ::fwData::PlaneList::sptr planeList = m_service->getObject< ::fwData::PlaneList >();
            planeList->getRefPlanes().erase
            (
                std::find( planeList->getRefPlanes().begin(), planeList->getRefPlanes().end(), m_pickedPlane.lock())
            );
            notifyDeletePlane(planeList, planeBackup);
        }
    }
    bool getSelectedPlane()
    {
        bool isFind              = false;
        vtkPropCollection* propc = m_picker->GetActors();
        vtkProp* prop;

        propc->InitTraversal();
        while ( (prop = propc->GetNextProp()) )
        {
            m_pickedPlane = ::fwData::Plane::dynamicCast(m_service->getAssociatedObject(prop,1));
            if( !m_pickedPlane.expired() )
            {
                ::fwData::PlaneList::sptr planeList = m_service->getObject< ::fwData::PlaneList >();
                if(!planeList->getRefPlanes().empty())
                {
                    ::fwData::PlaneList::PlaneListContainer::iterator itr = std::find(
                        planeList->getRefPlanes().begin(), planeList->getRefPlanes().end(), m_pickedPlane.lock());
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

protected:
    ::fwRenderVTK::IVtkAdaptorService *m_service;
    vtkPicker* m_picker;
    vtkPropCollection* m_propCollection;
    double m_display[3];
    int m_lastPos[2];
    ::fwData::Plane::wptr m_pickedPlane;

};

//------------------------------------------------------------------------------

PlaneList::PlaneList() throw()  : m_rightButtonCommand(nullptr), m_planeCollectionId("")
{
    newSlot(s_UPDATE_SELECTION_SLOT, &PlaneList::updateSelection, this);
    newSlot(s_UPDATE_PLANES_SLOT, &PlaneList::updatePlanes, this);
    newSlot(s_SHOW_PLANES_SLOT, &PlaneList::showPlanes, this);

    newSignal< SelectedignalType >(s_SELECTED_SIG);
}

//------------------------------------------------------------------------------

PlaneList::~PlaneList() throw()
{
}

//------------------------------------------------------------------------------

void PlaneList::doConfigure() throw(fwTools::Failed)
{
    SLM_ASSERT("Configuration must begin with <config>", m_configuration->getName() == "config");
    this->setPlaneCollectionId( m_configuration->getAttributeValue("planecollection") );
}

//------------------------------------------------------------------------------

void PlaneList::doStart() throw(fwTools::Failed)
{
    m_rightButtonCommand = vtkPlaneDeleteCallBack::New(this);
    this->getInteractor()->AddObserver( "RightButtonPressEvent", m_rightButtonCommand, 1 );
    this->getInteractor()->AddObserver( "RightButtonReleaseEvent", m_rightButtonCommand, 1 );

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
        for( ::fwData::Plane::sptr plane :  planeList->getPlanes() )
        {
            ::fwRenderVTK::IVtkAdaptorService::sptr servicePlane =
                ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >
                    ( plane, "::visuVTKAdaptor::Plane" );
            SLM_ASSERT("servicePlane not instanced", servicePlane);

            servicePlane->setRenderService(this->getRenderService());
            servicePlane->setRenderId( this->getRenderId() );
            servicePlane->setPickerId( this->getPickerId() );
            servicePlane->setAutoRender( this->getAutoRender() );

            if (!m_planeCollectionId.empty())
            {
                Plane::dynamicCast(servicePlane)->setVtkPlaneCollection( this->getVtkObject(m_planeCollectionId) );
            }
            servicePlane->start();

            m_planeConnections.connect(servicePlane, Plane::s_INTERACTION_STARTED_SIG, this->getSptr(),
                                       s_UPDATE_SELECTION_SLOT);

            this->registerService(servicePlane);
        }
    }
}

//------------------------------------------------------------------------------

void PlaneList::updatePlanes()
{
    this->doStop();
    this->doStart();
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void PlaneList::showPlanes(bool visible)
{
    ::fwData::PlaneList::sptr planeList = this->getObject< ::fwData::PlaneList >();

    this->doStop();
    if(visible)
    {
        this->doStart();
    }
    this->setVtkPipelineModified();
    this->requestRender();
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

void PlaneList::updateSelection(::fwData::Plane::sptr plane)
{
    auto sig = this->signal< SelectedignalType >(s_SELECTED_SIG);
    sig->asyncEmit(plane);
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
    m_planeConnections.disconnect();

    this->unregisterServices();
}


} //namespace visuVTKAdaptor

#endif // ANDROID
