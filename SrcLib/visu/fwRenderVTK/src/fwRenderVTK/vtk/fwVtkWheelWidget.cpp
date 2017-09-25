/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderVTK/vtk/fwVtkWheelWidget.hpp"

#include <fwServices/macros.hpp>

#include <glm/gtx/vector_angle.hpp>
#include <glm/vec2.hpp>

#include <vtkActor2D.h>
#include <vtkCallbackCommand.h>
#include <vtkObjectFactory.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkWidgetCallbackMapper.h>
#include <vtkWidgetEvent.h>

vtkStandardNewMacro(fwVtkWheelWidget);

//----------------------------------------------------------------------------------

fwVtkWheelWidget::fwVtkWheelWidget()
{
    // Set the initial state
    this->WidgetState = fwVtkWheelWidget::Hovering;

    // Okay, define the events
    this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
                                            vtkWidgetEvent::Select,
                                            this, fwVtkWheelWidget::SelectAction);
    this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent,
                                            vtkWidgetEvent::Move,
                                            this, fwVtkWheelWidget::MoveAction);
    this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent,
                                            vtkWidgetEvent::EndSelect,
                                            this, fwVtkWheelWidget::EndSelectAction);

    // Default update callback, crashes the program.
    m_wheelUpdateCallback = [] (double, double, double)
                            {
                                OSLM_FATAL("No update callback set.");
                            };
}

//----------------------------------------------------------------------

void fwVtkWheelWidget::CreateDefaultRepresentation()
{
    if( !this->WidgetRep )
    {
        this->WidgetRep = fwVtkWheelRepresentation::New();
    }
}

//----------------------------------------------------------------------------------

void fwVtkWheelWidget::SetWheelUpdateCallback(std::function<void (double, double, double)> f)
{
    m_wheelUpdateCallback = f;
}

//----------------------------------------------------------------------------------

void fwVtkWheelWidget::MoveAction(vtkAbstractWidget* w)
{
    fwVtkWheelWidget* self = reinterpret_cast<fwVtkWheelWidget*>(w);

    int x = self->Interactor->GetEventPosition()[0];
    int y = self->Interactor->GetEventPosition()[1];

    // Motion while selecting is ignored
    bool renderRequired = false;

    fwVtkWheelRepresentation* widgetRep = dynamic_cast<fwVtkWheelRepresentation*>(self->WidgetRep);

    SLM_ASSERT("Widget representation is not a wheel.", widgetRep);

    if ( self->WidgetState == fwVtkWheelWidget::Selecting )
    {
        const auto& actPos      = widgetRep->GetWheelActor()->GetPosition();
        const int* viewportSize = self->GetRepresentation()->GetRenderer()->GetRenderWindow()->GetSize();

        x = ::glm::clamp(x, 0, viewportSize[0]);
        y = ::glm::clamp(y, 0, viewportSize[1]);

        widgetRep->GetWheelActor()->SetPosition(x - self->m_initMouseX + actPos[0], y - self->m_initMouseY + actPos[1]);
        self->m_initMouseX = x;
        self->m_initMouseY = y;

        const ::glm::dvec2 center = widgetRep->GetCenterInScreenSpace();
        const double orientation  = widgetRep->GetOrientation();
        self->m_wheelUpdateCallback(center.x, center.y, orientation);

        renderRequired = true;
    }
    else if( self->WidgetState == fwVtkWheelWidget::Rotating)
    {
        const ::glm::dvec2 center = widgetRep->GetCenterInScreenSpace();

        const auto v1 = ::glm::normalize(::glm::dvec2(self->m_initMouseX - center.x, self->m_initMouseY - center.y));
        const auto v2 = ::glm::normalize(::glm::dvec2(x - center.x, y - center.y));

        const double vectProd = v1.x * v2.y - v1.y * v2.x;
        const double angleSin = std::asin(vectProd);
        const double angleCos = ::glm::angle(v1, v2);

        const double angle = ::glm::sign(angleSin) * angleCos;

        widgetRep->SetOrientation(self->m_initOrientation + angle);

        const double orientation = widgetRep->GetOrientation();
        self->m_wheelUpdateCallback(center.x, center.y, orientation);

        renderRequired = true;
    }
    else if ( self->WidgetState == fwVtkWheelWidget::Hovering )
    {
        widgetRep->SetHovering( widgetRep->isOnWheel(x, y) || widgetRep->isInCenter(x, y) );
        renderRequired = true;
    }

    if( renderRequired )
    {
        self->Render();
    }
}

//----------------------------------------------------------------------------------

void fwVtkWheelWidget::SelectAction(vtkAbstractWidget* w)
{
    fwVtkWheelWidget* self = reinterpret_cast<fwVtkWheelWidget*>(w);

    const int x     = self->Interactor->GetEventPosition()[0];
    const int y     = self->Interactor->GetEventPosition()[1];
    const int shift = self->Interactor->GetShiftKey();

    if(shift == 0)
    {
        return;
    }

    const fwVtkWheelRepresentation* widgetRep = dynamic_cast<fwVtkWheelRepresentation*>(self->WidgetRep);

    SLM_ASSERT("Widget representation is not a wheel.", widgetRep);

    if(widgetRep->isInCenter(x, y))
    {
        self->WidgetState  = ::fwVtkWheelWidget::Selecting;
        self->m_initMouseX = x;
        self->m_initMouseY = y;
    }
    else if(widgetRep->isOnWheel(x, y))
    {
        self->WidgetState       = ::fwVtkWheelWidget::Rotating;
        self->m_initMouseX      = x;
        self->m_initMouseY      = y;
        self->m_initOrientation = widgetRep->GetOrientation();
    }

    // We are definitely selected, Highlight as necessary.
    self->EventCallbackCommand->SetAbortFlag(1);
    self->Render();
}

//----------------------------------------------------------------------------------

void fwVtkWheelWidget::EndSelectAction(vtkAbstractWidget* w)
{
    fwVtkWheelWidget* self = reinterpret_cast<fwVtkWheelWidget*>(w);

    self->WidgetState = fwVtkWheelWidget::Hovering;

    self->Render();
}

//----------------------------------------------------------------------------------

void fwVtkWheelWidget::PrintSelf(ostream&, vtkIndent)
{
    SLM_FATAL("Not implemented.");
}

//----------------------------------------------------------------------------------

void fwVtkWheelWidget::SetRepresentation(fwVtkWheelRepresentation* rep)
{
    this->WidgetRep = rep;
}

//----------------------------------------------------------------------------------

fwVtkWheelRepresentation* fwVtkWheelWidget::GetRepresentation() const
{
    return dynamic_cast<fwVtkWheelRepresentation*>(this->WidgetRep);
}

