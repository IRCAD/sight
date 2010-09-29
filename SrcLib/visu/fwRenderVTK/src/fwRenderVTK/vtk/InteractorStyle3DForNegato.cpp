/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <vtkMath.h>
#include <vtkCamera.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>

#include "fwRenderVTK/vtk/InteractorStyle3DForNegato.hpp"

vtkCxxRevisionMacro(InteractorStyle3DForNegato, "$Revision: 1.24 $");
vtkStandardNewMacro(InteractorStyle3DForNegato);

//------------------------------------------------------------------------------

InteractorStyle3DForNegato::InteractorStyle3DForNegato() :
    vtkInteractorStyleTrackballCamera()
{
}

//------------------------------------------------------------------------------

InteractorStyle3DForNegato::~InteractorStyle3DForNegato()
{}

//------------------------------------------------------------------------------

void InteractorStyle3DForNegato::OnChar()
{
    vtkRenderWindowInteractor *rwi = this->Interactor;

    switch (rwi->GetKeyCode())
    {
    case 'r' :
        this->FindPokedRenderer(rwi->GetEventPosition()[0],
                rwi->GetEventPosition()[1]);
        this->CurrentRenderer->ResetCamera();
        rwi->Render();
        break;
    }
}

//------------------------------------------------------------------------------

void InteractorStyle3DForNegato::OnLeftButtonDown()
{
    this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
            this->Interactor->GetEventPosition()[1]);
    if (this->CurrentRenderer == NULL)
    {
        return;
    }

    this->GrabFocus(this->EventCallbackCommand);
    m_oldPickPoint[0] = this->Interactor->GetEventPosition()[0];
    m_oldPickPoint[1] = this->Interactor->GetEventPosition()[1];

    if (this->Interactor->GetShiftKey())
    {
        this->StartRotate();
    }
    else
    {
        if (this->Interactor->GetControlKey())
        {
            // press Ctrl key
            this->StartSpin();
        }
        else
        {
            this->StartRotate();
        }
    }
}

//------------------------------------------------------------------------------

void InteractorStyle3DForNegato::OnRightButtonDown()
{
    this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
            this->Interactor->GetEventPosition()[1]);
    if (this->CurrentRenderer == NULL)
    {
        return;
    }

    this->GrabFocus(this->EventCallbackCommand);

    m_oldPickPoint[0] = this->Interactor->GetEventPosition()[0];
    m_oldPickPoint[1] = this->Interactor->GetEventPosition()[1];
    this->StartDolly();
}

//------------------------------------------------------------------------------

void InteractorStyle3DForNegato::OnMiddleButtonDown()
{
    this->FindPokedRenderer(this->Interactor->GetEventPosition()[0], this->Interactor->GetEventPosition()[1]);

    if (this->CurrentRenderer != NULL)
    {
        m_oldPickPoint[0] = this->Interactor->GetEventPosition()[0];
        m_oldPickPoint[1] = this->Interactor->GetEventPosition()[1];
        this->StartPan();
    }
}

//------------------------------------------------------------------------------

void InteractorStyle3DForNegato::OnMouseMove()
{
    int x = this->Interactor->GetEventPosition()[0];
    int y = this->Interactor->GetEventPosition()[1];

    m_newPickPoint[0] = this->Interactor->GetEventPosition()[0];
    m_newPickPoint[1] = this->Interactor->GetEventPosition()[1];

    switch (this->State)
    {
    case VTKIS_ROTATE:
        this->FindPokedRenderer(x, y);
        this->Rotate();
        this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
        break;

    case VTKIS_PAN:
        this->FindPokedRenderer(x, y);
        this->Pan();
        this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
        break;

    case VTKIS_DOLLY:
        this->FindPokedRenderer(x, y);
        this->Dolly();
        this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
        break;

    case VTKIS_SPIN:
        this->FindPokedRenderer(x, y);
        this->Spin();
        this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
        break;
    }
    m_oldPickPoint[0] = m_newPickPoint[0];
    m_oldPickPoint[1] = m_newPickPoint[1];
}

//----------------------------------------------------------------------------

void InteractorStyle3DForNegato::OnMouseWheelBackward()
{
    if (!this->Interactor->GetShiftKey())
    {
        this->GrabFocus(this->EventCallbackCommand);
        this->StartDolly();
        double factor = this->MotionFactor * -0.2 * this->MouseWheelMotionFactor;
        this->Dolly(pow(1.1, factor));
        this->EndDolly();
        this->ReleaseFocus();
    }
}

//----------------------------------------------------------------------------

void InteractorStyle3DForNegato::OnMouseWheelForward()
{
    if (!this->Interactor->GetShiftKey())
    {
        this->GrabFocus(this->EventCallbackCommand);
        this->StartDolly();
        double factor = this->MotionFactor * 0.2 * this->MouseWheelMotionFactor;
        this->Dolly(pow(1.1, factor));
        this->EndDolly();
        this->ReleaseFocus();
    }
}

//----------------------------------------------------------------------------

void InteractorStyle3DForNegato::Pan()
{
    if (this->CurrentRenderer == NULL)
    {
        return;
    }

    // do nothing if mouse is still on the same pos
    if( (m_newPickPoint[0] == m_oldPickPoint[0]) &&
            (m_newPickPoint[1] == m_oldPickPoint[1]) )
    {
        return;
    }

    vtkRenderWindowInteractor *rwi = this->Interactor;
    double viewFocus[4], focalDepth, viewPoint[3];
    double newPickPoint[4], oldPickPoint[4];
    double motionVector[3];

    // Calculate the focal depth since we'll be using it a lot
    vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
    camera->GetFocalPoint(viewFocus);
    this->ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2],
            viewFocus);
    focalDepth = viewFocus[2];

    this->ComputeDisplayToWorld( m_newPickPoint[0],
            m_newPickPoint[1],
            focalDepth,
            newPickPoint);

    // Has to recalc old mouse point since the viewport has moved,
    // so can't move it outside the loop
    this->ComputeDisplayToWorld(m_oldPickPoint[0],
            m_oldPickPoint[1],
            focalDepth,
            oldPickPoint);

    // Camera motion is reversed
    motionVector[0] = oldPickPoint[0] - newPickPoint[0];
    motionVector[1] = oldPickPoint[1] - newPickPoint[1];
    motionVector[2] = oldPickPoint[2] - newPickPoint[2];

    camera->GetFocalPoint(viewFocus);
    camera->GetPosition(viewPoint);

    camera->SetFocalPoint(motionVector[0] + viewFocus[0],
            motionVector[1] + viewFocus[1],
            motionVector[2] + viewFocus[2]);

    camera->SetPosition(motionVector[0] + viewPoint[0],
            motionVector[1] + viewPoint[1],
            motionVector[2] + viewPoint[2]);

    if (rwi->GetLightFollowCamera())
    {
        this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }
    rwi->Render();
}

//----------------------------------------------------------------------------

void InteractorStyle3DForNegato::Dolly()
{
    if (this->CurrentRenderer == NULL)
    {
        return;
    }
    if( (m_newPickPoint[0] == m_oldPickPoint[0]) &&
            (m_newPickPoint[1] == m_oldPickPoint[1]) )
    {
        return;
    }
    vtkRenderWindowInteractor *rwi = this->Interactor;
    double *center = this->CurrentRenderer->GetCenter();
    int dy = m_newPickPoint[1] - m_oldPickPoint[1];
    double dyf = this->MotionFactor * dy / center[1];
    this->Dolly(pow(1.1, dyf));
}

//----------------------------------------------------------------------------

void InteractorStyle3DForNegato::Dolly(double factor)
{
    if (this->CurrentRenderer == NULL)
    {
        return;
    }

    vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
    if (camera->GetParallelProjection())
    {
        camera->SetParallelScale(camera->GetParallelScale() / factor);
    }
    else
    {
        camera->Dolly(factor);
        if (this->AutoAdjustCameraClippingRange)
        {
            this->CurrentRenderer->ResetCameraClippingRange();
        }
    }

    if (this->Interactor->GetLightFollowCamera())
    {
        this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }

    this->Interactor->Render();
}

//----------------------------------------------------------------------------

void InteractorStyle3DForNegato::Rotate()
{
    if (this->CurrentRenderer == NULL)
    {
        return;
    }
    vtkRenderWindowInteractor *rwi = this->Interactor;
    int dx = m_newPickPoint[0] - m_oldPickPoint[0];
    int dy = m_newPickPoint[1] - m_oldPickPoint[1];
    int *size = this->CurrentRenderer->GetRenderWindow()->GetSize();
    double delta_elevation = -20.0 / size[1];
    double delta_azimuth = -20.0 / size[0];
    double rxf = dx * delta_azimuth * this->MotionFactor;
    double ryf = dy * delta_elevation * this->MotionFactor;

    vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
    camera->Azimuth(rxf);
    camera->Elevation(ryf);
    camera->OrthogonalizeViewUp();
    if (this->AutoAdjustCameraClippingRange)
    {
        this->CurrentRenderer->ResetCameraClippingRange();
    }
    if (rwi->GetLightFollowCamera())
    {
        this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
    }

    rwi->Render();
}

//----------------------------------------------------------------------------

void InteractorStyle3DForNegato::Spin()
{
    if ( this->CurrentRenderer == NULL )
    {
        return;
    }
    vtkRenderWindowInteractor *rwi = this->Interactor;
    double *center = this->CurrentRenderer->GetCenter();
    double newAngle =
            vtkMath::DegreesFromRadians( atan2( m_newPickPoint[1] - center[1],
                    m_newPickPoint[0] - center[0] ) );
    double oldAngle =
            vtkMath::DegreesFromRadians( atan2( m_oldPickPoint[1] - center[1],
                    m_oldPickPoint[0] - center[0] ) );
    vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
    camera->Roll( newAngle - oldAngle );
    camera->OrthogonalizeViewUp();

    rwi->Render();
}
