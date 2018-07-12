/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SPoint.hpp"

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

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SPoint);

namespace visuVTKAdaptor
{

const ::fwCom::Signals::SignalKeyType SPoint::s_INTERACTION_STARTED_SIG = "interactionStarted";

const ::fwServices::IService::KeyType SPoint::s_POINT_INOUT = "point";

//------------------------------------------------------------------------------

class vtkPointUpdateCallBack : public vtkCommand
{

public:
    //------------------------------------------------------------------------------

    static vtkPointUpdateCallBack* New( ::fwRenderVTK::IAdaptor* service)
    {
        return new vtkPointUpdateCallBack(service);
    }

    vtkPointUpdateCallBack( ::fwRenderVTK::IAdaptor* service ) :
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

        ::fwData::Point::sptr point = m_service->getInOut< ::fwData::Point >(SPoint::s_POINT_INOUT);
        SLM_ASSERT("inout 'point' is not defined", point);

        vtkHandleRepresentation* representation = vtkHandleRepresentation::SafeDownCast(handler->GetRepresentation());
        SLM_ASSERT("handler not instanced", handler);
        double* world = representation->GetWorldPosition();

        if ( (eventId == vtkCommand::InteractionEvent)
             || eventId == vtkCommand::EndInteractionEvent )
        {
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

            std::copy( world, world+3, point->getCoord().begin() );

            auto sig = point->signal< ::fwData::Point::ModifiedSignalType >(::fwData::Point::s_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if (eventId == vtkCommand::StartInteractionEvent)
        {
            auto sig = m_service->signal< SPoint::InteractionStartedSignalType >(SPoint::s_INTERACTION_STARTED_SIG);
            sig->asyncEmit();
        }
    }

protected:

    ::fwRenderVTK::IAdaptor* m_service;

    int m_pickLimiter;
};

//------------------------------------------------------------------------------

SPoint::SPoint() noexcept :
    m_handle( vtkHandleWidget::New() ),
    m_representation( ::fwRenderVTK::vtk::MarkedSphereHandleRepresentation::New() ),
    m_pointUpdateCommand(nullptr),
    m_radius(7.0),
    m_interaction(true)
{
    m_ptColor         = ::fwData::Color::New();
    m_ptSelectedColor = ::fwData::Color::New();

    m_ptSelectedColor->setRGBA("#00FF00");

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
    rep->SetHandleSize(m_radius);

    newSignal<InteractionStartedSignalType>(s_INTERACTION_STARTED_SIG);
}

//------------------------------------------------------------------------------

SPoint::~SPoint() noexcept
{
    m_handle->SetRepresentation(0);
    m_handle->Delete();
    m_handle = nullptr;

    m_representation->Delete();
    m_representation = nullptr;
}

//------------------------------------------------------------------------------

void SPoint::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    const std::string hexaSelectedColor = config.get<std::string>("selectedColor", "");
    m_ptSelectedColor = ::fwData::Color::New();
    if (!hexaSelectedColor.empty())
    {
        m_ptSelectedColor->setRGBA(hexaSelectedColor);
    }

    const std::string hexaColor = config.get<std::string>("color", "");
    m_ptColor = ::fwData::Color::New();
    if (!hexaColor.empty())
    {
        m_ptColor->setRGBA(hexaColor);
    }

    const std::string radius = config.get<std::string>("radius", "");
    if(!radius.empty())
    {
        m_radius = std::stod(radius);
    }

    const std::string interaction = config.get<std::string>("interaction", "");
    if(!interaction.empty())
    {
        SLM_FATAL_IF("value for 'interaction' must be 'on' or 'off', actual: " + interaction,
                     interaction != "on" && interaction != "off");
        m_interaction = (interaction == "on");
    }
}

//------------------------------------------------------------------------------

void SPoint::starting()
{
    this->initialize();

    m_handle->SetInteractor(  this->getInteractor() );
    m_handle->KeyPressActivationOff();

    m_pointUpdateCommand = vtkPointUpdateCallBack::New(this);

    m_handle->AddObserver( "StartInteractionEvent", m_pointUpdateCommand );

    m_handle->On();

    // We don't want to add m_representation to the renderer, m_handle
    // is already managing that.
    this->registerProp(m_representation);

    this->updating();
}

//------------------------------------------------------------------------------

void SPoint::updating()
{
    ::fwData::Point::sptr point = this->getInOut< ::fwData::Point >(s_POINT_INOUT);
    SLM_ASSERT("point not instanced", point);

    double ps[3];

    std::copy(point->getCoord().begin(), point->getCoord().end(), ps  );
    m_representation->SetWorldPosition( ps );

    getRenderer()->ResetCameraClippingRange();

    ::fwRenderVTK::vtk::MarkedSphereHandleRepresentation* rep =
        ::fwRenderVTK::vtk::MarkedSphereHandleRepresentation::SafeDownCast(m_representation);

    SLM_ASSERT("MarkedSphereHandleRepresentation cast failed", rep);

    rep->SetHandleSize(m_radius);

    rep->GetProperty()->SetColor(m_ptColor->red(), m_ptColor->green(), m_ptColor->blue());
    rep->GetProperty()->SetOpacity(m_ptColor->alpha());

    rep->GetSelectedProperty()->SetColor(m_ptSelectedColor->red(), m_ptSelectedColor->green(),
                                         m_ptSelectedColor->blue());
    rep->GetSelectedProperty()->SetOpacity(m_ptSelectedColor->alpha());

    m_handle->SetProcessEvents(m_interaction);
    m_handle->SetManagesCursor(m_interaction);

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SPoint::stopping()
{
    m_handle->Off();
    m_handle->RemoveObserver(m_pointUpdateCommand);

    m_pointUpdateCommand->Delete();
    m_pointUpdateCommand = 0;

    m_handle->SetInteractor(0);

    this->unregisterProps();
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SPoint::setRadius(const double radius)
{
    m_radius = radius;
    updating();
}

//------------------------------------------------------------------------------

void SPoint::setColor(const float red, const float green, const float blue, const float alpha)
{
    m_ptColor->setRGBA(red, green, blue, alpha);
    updating();
}

//------------------------------------------------------------------------------

void SPoint::setSelectedColor(const float red, const float green, const float blue, const float alpha)
{
    m_ptSelectedColor->setRGBA(red, green, blue, alpha);
    updating();
}

//------------------------------------------------------------------------------

void SPoint::setInteraction(const bool interaction)
{
    m_interaction = interaction;
    updating();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SPoint::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_POINT_INOUT, ::fwData::Point::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
