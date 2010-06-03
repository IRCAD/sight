/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vtkCommand.h>
#include <vtkHandleWidget.h>
#include <vtkSphereHandleRepresentation.h>

#include <fwData/Point.hpp>
#include <fwData/Material.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/PointMsg.hpp>

#include <vtkSphereSource.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkPicker.h>
#include <vtkPropCollection.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkWidgetEventTranslator.h>

#include "fwRenderVTK/vtk/Helpers.hpp"
#include "fwRenderVTK/vtk/MarkedSphereHandleRepresentation.hpp"
#include "visuVTKAdaptor/Point.hpp"



REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Point, ::fwData::Point ) ;

namespace visuVTKAdaptor
{

class vtkPointUpdateCallBack : public vtkCommand
{

public :
    static vtkPointUpdateCallBack *New( ::fwRenderVTK::IVtkAdaptorService *service)
    { return new vtkPointUpdateCallBack(service); }

    vtkPointUpdateCallBack( ::fwRenderVTK::IVtkAdaptorService *service )
    : m_service(service),
      m_pickLimiter (0)
    {}

    virtual void Execute( vtkObject *caller, unsigned long eventId, void *)
    {
        vtkHandleWidget *handler = vtkHandleWidget::SafeDownCast(caller);
        if (!handler) {return;}

        if ( eventId == vtkCommand::StartInteractionEvent)
        {
            handler->AddObserver("EndInteractionEvent" ,this );
            handler->AddObserver("InteractionEvent" ,this );

        }
        else if ( eventId == vtkCommand::EndInteractionEvent )
        {
            handler->RemoveObservers("EndInteractionEvent" ,this );
            handler->RemoveObservers("InteractionEvent" ,this );
        }

        ::fwData::Point::sptr point= m_service->getObject< ::fwData::Point >();

        vtkHandleRepresentation *representation = vtkHandleRepresentation::SafeDownCast(handler->GetRepresentation());
        assert(handler);
        double *world = representation->GetWorldPosition();

        ::fwComEd::PointMsg::NewSptr msg;// (  new fwServices::ObjectMsg(point) );

        if ( (m_pickLimiter-- == 0 && eventId == vtkCommand::InteractionEvent)
                || eventId == vtkCommand::EndInteractionEvent )
        {
            m_pickLimiter=2;

            double display[3];
            int x,y;
            handler->GetInteractor()->GetLastEventPosition(x,y);
            display[0]=x;
            display[1]=y;
            display[2]=0;

            if ( m_service->getPicker() && m_service->getPicker()->Pick( display , m_service->getRenderer() ) )
            {
                ::fwRenderVTK::vtk::getNearestPickedPosition(m_service->getPicker(), m_service->getRenderer(), world);
            }
        }
        else if (eventId == vtkCommand::StartInteractionEvent)
        {
            msg->addEvent( ::fwComEd::PointMsg::START_POINT_INTERACTION );
        }

        std::copy( world, world+3 , point->getRefCoord().begin() );

        msg->addEvent( ::fwComEd::PointMsg::POINT_IS_MODIFIED );//setAllModified();

        ::fwServices::IEditionService::notify( m_service->getSptr(), point, msg );
        m_service->update();
    }

protected :

    ::fwRenderVTK::IVtkAdaptorService * m_service;

    int m_pickLimiter;
};

//------------------------------------------------------------------------------

Point::Point() throw() :
    m_handle( vtkHandleWidget::New() ),
//    m_representation( vtkSphereHandleRepresentation::New() ),
    m_representation( ::fwRenderVTK::vtk::MarkedSphereHandleRepresentation::New() ),
    m_pointUpdateCommand(0)
{
    m_handle->SetRepresentation(m_representation);
    m_handle->SetPriority(0.8);

    vtkWidgetEventTranslator *translator = m_handle->GetEventTranslator();

    translator->RemoveTranslation(vtkCommand::MiddleButtonPressEvent);
    translator->RemoveTranslation(vtkCommand::MiddleButtonReleaseEvent);
    translator->RemoveTranslation(vtkCommand::RightButtonPressEvent);
    translator->RemoveTranslation(vtkCommand::RightButtonReleaseEvent);

    ::fwRenderVTK::vtk::MarkedSphereHandleRepresentation *rep =
        ::fwRenderVTK::vtk::MarkedSphereHandleRepresentation::SafeDownCast(m_representation);

    rep->GetSelectedProperty()->SetOpacity(.3);
    rep->GetMarkerProperty()->SetOpacity(.3);
    rep->SetHandleSize(7);

    addNewHandledEvent( ::fwComEd::PointMsg::POINT_IS_MODIFIED );
}

//------------------------------------------------------------------------------

Point::~Point() throw()
{
    SLM_TRACE_FUNC();

    m_handle->SetRepresentation(0);
    m_handle->Delete();
    m_handle = 0;

    m_representation->Delete();
    m_representation = 0;

}

//------------------------------------------------------------------------------

void Point::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setPickerId( m_configuration->getAttributeValue("picker") );
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
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

    assert(point);

    double ps[3];
    assert ( point->getCRefCoord().size()==3 );
    std::copy(point->getCRefCoord().begin(),point->getCRefCoord().end(), ps  );
    m_representation->SetWorldPosition( ps );
//  getRenderService()->update();
    getRenderer()->ResetCameraClippingRange();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Point::doUpdate( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_ASSERT("ACH : receive a msg that no concern his object", _msg->getSubject().lock() == this->getObject() );
    ::fwComEd::PointMsg::csptr pointMsg = ::fwComEd::PointMsg::dynamicConstCast( _msg );
    if ( pointMsg && pointMsg->hasEvent( ::fwComEd::PointMsg::POINT_IS_MODIFIED ) )
    {
        this->doUpdate();
    }
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

void Point::setColor(double red, double green, double blue, double alpha)
{
    ::fwRenderVTK::vtk::MarkedSphereHandleRepresentation *rep =
            ::fwRenderVTK::vtk::MarkedSphereHandleRepresentation::SafeDownCast(m_representation);
    SLM_ASSERT("MarkedSphereHandleRepresentation cast failed", rep);
    rep->GetProperty()->SetColor(red, green, blue);
    rep->GetProperty()->SetOpacity(alpha);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Point::setSelectedColor(double red, double green, double blue, double alpha)
{
    ::fwRenderVTK::vtk::MarkedSphereHandleRepresentation *rep =
                ::fwRenderVTK::vtk::MarkedSphereHandleRepresentation::SafeDownCast(m_representation);
    SLM_ASSERT("MarkedSphereHandleRepresentation cast failed", rep);
    rep->GetSelectedProperty()->SetColor(red, green, blue);
    rep->GetSelectedProperty()->SetOpacity(alpha);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
