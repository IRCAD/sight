/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef ANDROID

#include "visuVTKAdaptor/SPlane.hpp"

#include "visuVTKAdaptor/SPoint.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Color.hpp>
#include <fwData/Plane.hpp>

#include <fwMath/IntrasecTypes.hpp>
#include <fwMath/PlaneFunctions.hpp>

#include <fwRenderVTK/vtk/Helpers.hpp>
#include <fwRenderVTK/vtk/MarkedSphereHandleRepresentation.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <vtkActor.h>
#include <vtkPlaneCollection.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropCollection.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SPlane, ::fwData::Plane );

namespace visuVTKAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_POINTS_SLOT     = "updatePoints";
static const ::fwCom::Slots::SlotKeyType s_START_INTERACTION_SLOT = "startInteraction";
static const ::fwCom::Slots::SlotKeyType s_SELECT_PLANE_SLOT      = "selectPlane";

const ::fwCom::Signals::SignalKeyType SPlane::s_INTERACTION_STARTED_SIG = "interactionStarted";

const ::fwServices::IService::KeyType SPlane::s_PLANE_INOUT = "plane";

//------------------------------------------------------------------------------

SPlane::SPlane() noexcept :
    m_vtkPlane(nullptr),
    m_actorPlan(nullptr),
    m_vtkImplicitPlane(nullptr),
    m_vtkPlaneCollection(nullptr)
{
    newSlot(s_UPDATE_POINTS_SLOT, &SPlane::updatePoints, this);
    newSlot(s_START_INTERACTION_SLOT, &SPlane::startInteraction, this);
    newSlot(s_SELECT_PLANE_SLOT, &SPlane::selectPlane, this);

    newSignal< InteractionStartedSignalType >(s_INTERACTION_STARTED_SIG);
}

//------------------------------------------------------------------------------

SPlane::~SPlane() noexcept
{
}

//------------------------------------------------------------------------------

void SPlane::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_planeCollectionId = config.get("planecollection", "");
}

//------------------------------------------------------------------------------

void SPlane::starting()
{
    this->initialize();

    ::fwData::Plane::csptr plane = this->getInOut< ::fwData::Plane >(s_PLANE_INOUT);
    SLM_ASSERT("Plane is missing", plane);

    for( const ::fwData::Point::sptr& point :  plane->getPoints() )
    {
        // create the srv configuration for objects auto-connection
        auto servicePoint = this->registerService< ::fwRenderVTK::IAdaptor >("::visuVTKAdaptor::SPoint");
        servicePoint->registerInOut(point, SPoint::s_POINT_INOUT, true);

        servicePoint->setRenderService(this->getRenderService());
        servicePoint->setRendererId( this->getRendererId() );
        servicePoint->setPickerId( this->getPickerId() );
        servicePoint->start();

        m_connections.connect(point, ::fwData::Object::s_MODIFIED_SIG, this->getSptr(), s_UPDATE_POINTS_SLOT);
        m_connections.connect(servicePoint, ::visuVTKAdaptor::SPoint::s_INTERACTION_STARTED_SIG,
                              this->getSptr(), s_START_INTERACTION_SLOT);
    }

    if (!m_planeCollectionId.empty())
    {
        m_vtkPlaneCollection = vtkPlaneCollection::SafeDownCast(this->getVtkObject(m_planeCollectionId));
    }

    if (m_vtkPlaneCollection)
    {
        m_vtkImplicitPlane = vtkPlane::New();
        m_vtkPlaneCollection->AddItem(m_vtkImplicitPlane);
    }

    this->setVtkPipelineModified();
    this->updating();
}

//------------------------------------------------------------------------------

void SPlane::updating()
{
    ::fwData::Plane::csptr plane = this->getInOut< ::fwData::Plane >(s_PLANE_INOUT);
    SLM_ASSERT("Plane is missing", plane);

    ::fwData::Point::csptr pt0 = plane->getPoints()[0];
    ::fwData::Point::csptr pt1 = plane->getPoints()[1];
    ::fwData::Point::csptr pt2 = plane->getPoints()[2];

    fwPlane planeDesc;
    ::fwMath::setValues(planeDesc, pt0->getCoord(), pt1->getCoord(), pt2->getCoord());

    if(m_vtkImplicitPlane)
    {
        fwVec3d normal = ::fwMath::getNormal(planeDesc);

        m_vtkImplicitPlane->SetOrigin(pt0->getCoord()[0], pt0->getCoord()[1], pt0->getCoord()[2]);
        m_vtkImplicitPlane->SetNormal(normal[0], normal[1], normal[2]);
        m_vtkImplicitPlane->Modified();
    }
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SPlane::updatePoints()
{
    this->updating();

    ::fwData::Plane::csptr plane = this->getInOut< ::fwData::Plane >(s_PLANE_INOUT);
    SLM_ASSERT("Plane is missing", plane);

    auto sig = plane->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SPlane::startInteraction()
{
    ::fwData::Plane::sptr plane = this->getInOut< ::fwData::Plane >(s_PLANE_INOUT);
    SLM_ASSERT("Plane is missing", plane);

    this->selectPlane(true);

    auto sig = this->signal< InteractionStartedSignalType >( s_INTERACTION_STARTED_SIG);
    sig->asyncEmit(plane);
}

//------------------------------------------------------------------------------

void SPlane::stopping()
{
    if (m_vtkPlaneCollection && m_vtkImplicitPlane)
    {
        m_vtkPlaneCollection->RemoveItem(m_vtkImplicitPlane);
        m_vtkImplicitPlane->Delete();
        m_vtkImplicitPlane = 0;
    }

    m_connections.disconnect();

    this->unregisterServices();
    this->removeAllPropFromRenderer();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SPlane::setVtkPlaneCollection( vtkObject* col )
{
    if (m_vtkPlaneCollection != col)
    {
        if (m_vtkPlaneCollection)
        {
            if (m_vtkImplicitPlane)
            {
                m_vtkPlaneCollection->RemoveItem(m_vtkImplicitPlane);
            }
            m_vtkPlaneCollection = 0;
        }

        if ( col )
        {
            m_vtkPlaneCollection = vtkPlaneCollection::SafeDownCast(col);
            if (m_vtkImplicitPlane)
            {
                m_vtkPlaneCollection->AddItem(m_vtkImplicitPlane);
            }
        }
    }
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SPlane::selectPlane(bool select)
{
    auto subServices = this->getRegisteredServices();
    for (const ::fwServices::IService::wptr& adaptor: subServices)
    {
        auto servicePoint = ::visuVTKAdaptor::SPoint::dynamicCast(adaptor.lock());
        if(servicePoint)
        {
            if(select)
            {
                servicePoint->setColor(1., 0., 0.);
            }
            else
            {
                servicePoint->setColor(1., 1., 1.);
            }
        }
    }
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SPlane::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_PLANE_INOUT, ::fwData::Plane::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_PLANE_INOUT, ::fwData::Plane::s_SELECTED_SIG, s_SELECT_PLANE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor

#endif //ANDROID
