/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef ANDROID

#include "visuVTKAdaptor/SPlaneList.hpp"

#include "visuVTKAdaptor/SPlane.hpp"

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

#include <vtkActor.h>
#include <vtkAssemblyNode.h>
#include <vtkAssemblyPath.h>
#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SPlaneList);

namespace visuVTKAdaptor
{

static const ::fwCom::Signals::SignalKeyType s_SELECTED_SIG = "selected";

static const ::fwCom::Slots::SlotKeyType s_UPDATE_SELECTION_SLOT = "updateSelection";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_PLANES_SLOT    = "updatePlanes";
static const ::fwCom::Slots::SlotKeyType s_SHOW_PLANES_SLOT      = "showPlanes";

static const ::fwServices::IService::KeyType s_PLANES_INOUT = "planes";

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
    //------------------------------------------------------------------------------

    static vtkPlaneDeleteCallBack* New( ::fwRenderVTK::IAdaptor* service)
    {
        return new vtkPlaneDeleteCallBack(service);
    }

    vtkPlaneDeleteCallBack( ::fwRenderVTK::IAdaptor* service ) :
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

    //------------------------------------------------------------------------------

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

    //------------------------------------------------------------------------------

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

            ::fwData::PlaneList::sptr planeList = m_service->getInOut< ::fwData::PlaneList >(s_PLANES_INOUT);
            SLM_ASSERT("PlaneList '" + s_PLANES_INOUT + "' is missing", planeList);

            planeList->getRefPlanes().erase
            (
                std::find( planeList->getRefPlanes().begin(), planeList->getRefPlanes().end(), m_pickedPlane.lock())
            );
            notifyDeletePlane(planeList, planeBackup);
        }
    }
    //------------------------------------------------------------------------------

    bool getSelectedPlane()
    {
        bool isFind              = false;
        vtkPropCollection* propc = m_picker->GetActors();
        vtkProp* prop;

        propc->InitTraversal();
        while ( (prop = propc->GetNextProp()) )
        {
            m_pickedPlane = ::fwData::Plane::dynamicCast(m_service->getAssociatedObject(prop, 1));
            if( !m_pickedPlane.expired() )
            {
                ::fwData::PlaneList::sptr planeList = m_service->getInOut< ::fwData::PlaneList >(s_PLANES_INOUT);
                SLM_ASSERT("PlaneList '" + s_PLANES_INOUT + "' is missing", planeList);

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
    ::fwRenderVTK::IAdaptor *m_service;
    vtkPicker* m_picker;
    vtkPropCollection* m_propCollection;
    double m_display[3];
    int m_lastPos[2];
    ::fwData::Plane::wptr m_pickedPlane;

};

//------------------------------------------------------------------------------

SPlaneList::SPlaneList() noexcept  :
    m_rightButtonCommand(nullptr),
    m_planeCollectionId("")
{
    newSlot(s_UPDATE_SELECTION_SLOT, &SPlaneList::updateSelection, this);
    newSlot(s_UPDATE_PLANES_SLOT, &SPlaneList::updatePlanes, this);
    newSlot(s_SHOW_PLANES_SLOT, &SPlaneList::showPlanes, this);

    newSignal< SelectedignalType >(s_SELECTED_SIG);
}

//------------------------------------------------------------------------------

SPlaneList::~SPlaneList() noexcept
{
}

//------------------------------------------------------------------------------

void SPlaneList::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");

    this->setPlaneCollectionId( config.get("planecollection", "") );
}

//------------------------------------------------------------------------------

void SPlaneList::starting()
{
    this->initialize();

    m_rightButtonCommand = vtkPlaneDeleteCallBack::New(this);
    this->getInteractor()->AddObserver( "RightButtonPressEvent", m_rightButtonCommand, 1 );
    this->getInteractor()->AddObserver( "RightButtonReleaseEvent", m_rightButtonCommand, 1 );

    this->updating();
}

//------------------------------------------------------------------------------

void SPlaneList::updating()
{
    ::fwData::PlaneList::sptr planeList = this->getInOut< ::fwData::PlaneList >(s_PLANES_INOUT);
    SLM_ASSERT("PlaneList '" + s_PLANES_INOUT + "' is missing", planeList);

    bool showPlanes = planeList->getField("ShowPlanes", ::fwData::Boolean::New(true))->value();
    if(showPlanes)
    {
        for(const ::fwData::Plane::sptr& plane :  planeList->getPlanes())
        {
            // create the srv configuration for objects auto-connection
            IService::Config serviceConfig;
            ::fwRenderVTK::IAdaptor::sptr servicePlane =
                this->createSubAdaptor("::visuVTKAdaptor::SPlane", serviceConfig);
            this->registerServiceInOut(plane, SPlane::s_PLANE_INOUT, servicePlane, true, serviceConfig);

            servicePlane->setConfiguration(serviceConfig);
            servicePlane->setRenderService(this->getRenderService());
            servicePlane->setRendererId(this->getRendererId());
            servicePlane->setPickerId(this->getPickerId());
            servicePlane->setAutoRender(this->getAutoRender());

            if (!m_planeCollectionId.empty())
            {
                SPlane::dynamicCast(servicePlane)->setVtkPlaneCollection( this->getVtkObject(m_planeCollectionId) );
            }
            servicePlane->start();

            m_planeConnections.connect(servicePlane, SPlane::s_INTERACTION_STARTED_SIG, this->getSptr(),
                                       s_UPDATE_SELECTION_SLOT);
        }
    }
}

//------------------------------------------------------------------------------

void SPlaneList::updatePlanes()
{
    this->stopping();
    this->starting();
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SPlaneList::showPlanes(bool visible)
{
    this->stopping();
    if(visible)
    {
        this->starting();
    }
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SPlaneList::updateSelection(::fwData::Plane::sptr plane)
{
    for (const ::fwRenderVTK::IAdaptor::wptr& adaptor: m_subServices)
    {
        SPlane::sptr planeAdaptor = SPlane::dynamicCast(adaptor.lock());

        if (planeAdaptor->getInOut< ::fwData::Plane >(SPlane::s_PLANE_INOUT) == plane)
        {
            planeAdaptor->selectPlane(true);
        }
        else
        {
            planeAdaptor->selectPlane(false);
        }
    }

    auto sig = this->signal< SelectedignalType >(s_SELECTED_SIG);
    sig->asyncEmit(plane);
}

//------------------------------------------------------------------------------

void SPlaneList::stopping()
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

//------------------------------------------------------------------------------

SPlaneList::KeyConnectionsMap SPlaneList::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_PLANES_INOUT, ::fwData::PlaneList::s_MODIFIED_SIG, s_UPDATE_PLANES_SLOT);

    return connections;
}

} //namespace visuVTKAdaptor

#endif // ANDROID
