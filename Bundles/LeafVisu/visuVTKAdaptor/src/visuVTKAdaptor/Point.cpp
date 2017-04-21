/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef ANDROID

#include "visuVTKAdaptor/Point.hpp"

#include "fwRenderVTK/vtk/Helpers.hpp"
#include "fwRenderVTK/vtk/MarkedSphereHandleRepresentation.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwData/Material.hpp>
#include <fwData/Point.hpp>

#include <fwServices/macros.hpp>

#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkHandleWidget.h>
#include <vtkPicker.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropCollection.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSphereHandleRepresentation.h>
#include <vtkSphereSource.h>
#include <vtkWidgetEventTranslator.h>

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Point, ::fwData::Point );

namespace visuVTKAdaptor
{

const ::fwCom::Signals::SignalKeyType Point::s_INTERACTION_STARTED_SIG = "interactionStarted";

//------------------------------------------------------------------------------

class vtkPointUpdateCallBack : public vtkCommand
{

public:
    //------------------------------------------------------------------------------

    static vtkPointUpdateCallBack* New( ::fwRenderVTK::IVtkAdaptorService* service)
    {
        return new vtkPointUpdateCallBack(service);
    }

    vtkPointUpdateCallBack( ::fwRenderVTK::IVtkAdaptorService* service ) :
        m_service(service),
        m_pickLimiter(0)
    {
    }

    //------------------------------------------------------------------------------

    virtual void Execute( vtkObject* caller, unsigned long eventId, void*)
    {
        vtkHandleWidget* handler = vtkHandleWidget::SafeDownCast(caller);
        if (!handler)
        {
            return;
        }

        if ( eventId == vtkCommand::StartInteractionEvent)
        {
            handler->AddObserver("EndInteractionEvent", this );
            handler->AddObserver("InteractionEvent", this );

        }
        else if ( eventId == vtkCommand::EndInteractionEvent )
        {
            handler->RemoveObservers("EndInteractionEvent", this );
            handler->RemoveObservers("InteractionEvent", this );
        }

        ::fwData::Point::sptr point = m_service->getObject< ::fwData::Point >();

        vtkHandleRepresentation* representation = vtkHandleRepresentation::SafeDownCast(handler->GetRepresentation());
        SLM_ASSERT("handler not instanced", handler);
        double* world = representation->GetWorldPosition();

        if ( (m_pickLimiter-- == 0 && eventId == vtkCommand::InteractionEvent)
             || eventId == vtkCommand::EndInteractionEvent )
        {
            m_pickLimiter = 2;

            double display[3];
            int x, y;
            handler->GetInteractor()->GetLastEventPosition(x, y);
            display[0] = x;
            display[1] = y;
            display[2] = 0;

            if ( m_service->getPicker() && m_service->getPicker()->Pick( display, m_service->getRenderer() ) )
            {
                ::fwRenderVTK::vtk::getNearestPickedPosition(m_service->getPicker(), m_service->getRenderer(), world);
            }
        }
        else if (eventId == vtkCommand::StartInteractionEvent)
        {
            auto sig = m_service->signal< Point::InteractionStartedSignalType >(Point::s_INTERACTION_STARTED_SIG);
            sig->asyncEmit();
        }

        std::copy( world, world+3, point->getRefCoord().begin() );

        auto sig = point->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
        {
            ::fwCom::SlotBase::sptr slot;
            slot = m_service->slot(::fwServices::IService::s_UPDATE_SLOT );
            ::fwCom::Connection::Blocker block(sig->getConnection(slot));
            sig->asyncEmit();
        }
        m_service->update();
    }

protected:

    ::fwRenderVTK::IVtkAdaptorService * m_service;

    int m_pickLimiter;
};

//------------------------------------------------------------------------------

Point::Point() throw() :
    m_handle( vtkHandleWidget::New() ),
    m_representation( ::fwRenderVTK::vtk::MarkedSphereHandleRepresentation::New() ),
    m_pointUpdateCommand(nullptr)
{
    m_handle->SetRepresentation(m_representation);
    m_handle->SetPriority(0.8f);

    vtkWidgetEventTranslator* translator = m_handle->GetEventTranslator();

    translator->RemoveTranslation(vtkCommand::MiddleButtonPressEvent);
    translator->RemoveTranslation(vtkCommand::MiddleButtonReleaseEvent);
    translator->RemoveTranslation(vtkCommand::RightButtonPressEvent);
    translator->RemoveTranslation(vtkCommand::RightButtonReleaseEvent);

    ::fwRenderVTK::vtk::MarkedSphereHandleRepresentation* rep =
        ::fwRenderVTK::vtk::MarkedSphereHandleRepresentation::SafeDownCast(m_representation);

    rep->GetSelectedProperty()->SetOpacity(.3);
    rep->GetMarkerProperty()->SetOpacity(.3);
    rep->SetHandleSize(7.0);

    newSignal<InteractionStartedSignalType>(s_INTERACTION_STARTED_SIG);
}

//------------------------------------------------------------------------------

Point::~Point() throw()
{
    m_handle->SetRepresentation(0);
    m_handle->Delete();
    m_handle = nullptr;

    m_representation->Delete();
    m_representation = nullptr;
}

//------------------------------------------------------------------------------

void Point::doConfigure() throw(fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void Point::doStart() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    m_handle->SetInteractor(  this->getInteractor() );
    m_handle->KeyPressActivationOff();

    m_pointUpdateCommand = vtkPointUpdateCallBack::New(this);

    m_handle->AddObserver( "StartInteractionEvent", m_pointUpdateCommand );

    m_handle->On();

    // We don't want to add m_representation to the renderer, m_handle
    // is already managing that.
    this->registerProp(m_representation);

    this->doUpdate();
}

//------------------------------------------------------------------------------

void Point::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Point::doUpdate() throw(fwTools::Failed)
{
    ::fwData::Point::sptr point = this->getObject < ::fwData::Point >();

    SLM_ASSERT("point not instanced", point);

    double ps[3];
    assert( point->getCRefCoord().size() == 3 );
    std::copy(point->getCRefCoord().begin(), point->getCRefCoord().end(), ps  );
    m_representation->SetWorldPosition( ps );
    //getRenderService()->update();
    getRenderer()->ResetCameraClippingRange();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Point::doStop() throw(fwTools::Failed)
{
    m_handle->Off();
    m_handle->RemoveObserver(m_pointUpdateCommand);

    m_pointUpdateCommand->Delete();
    m_pointUpdateCommand = 0;

    m_handle->SetInteractor(0);

    this->unregisterProps();
}

//------------------------------------------------------------------------------

void Point::setRadius(double radius)
{
    ::fwRenderVTK::vtk::MarkedSphereHandleRepresentation* rep =
        ::fwRenderVTK::vtk::MarkedSphereHandleRepresentation::SafeDownCast(m_representation);
    SLM_ASSERT("MarkedSphereHandleRepresentation cast failed", rep);
    rep->SetHandleSize(radius);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Point::setColor(double red, double green, double blue, double alpha)
{
    ::fwRenderVTK::vtk::MarkedSphereHandleRepresentation* rep =
        ::fwRenderVTK::vtk::MarkedSphereHandleRepresentation::SafeDownCast(m_representation);
    SLM_ASSERT("MarkedSphereHandleRepresentation cast failed", rep);
    rep->GetProperty()->SetColor(red, green, blue);
    rep->GetProperty()->SetOpacity(alpha);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Point::setSelectedColor(double red, double green, double blue, double alpha)
{
    ::fwRenderVTK::vtk::MarkedSphereHandleRepresentation* rep =
        ::fwRenderVTK::vtk::MarkedSphereHandleRepresentation::SafeDownCast(m_representation);
    SLM_ASSERT("MarkedSphereHandleRepresentation cast failed", rep);
    rep->GetSelectedProperty()->SetColor(red, green, blue);
    rep->GetSelectedProperty()->SetOpacity(alpha);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType Point::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Point::s_MODIFIED_SIG, s_UPDATE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor

#endif // ANDROID
