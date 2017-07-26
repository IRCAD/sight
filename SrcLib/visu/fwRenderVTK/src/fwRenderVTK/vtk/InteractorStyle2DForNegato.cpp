/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwRenderVTK/vtk/InteractorStyle2DForNegato.hpp"

#include <fwCore/base.hpp>

#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkObjectFactory.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

vtkStandardNewMacro(InteractorStyle2DForNegato);

//------------------------------------------------------------------------------

InteractorStyle2DForNegato::InteractorStyle2DForNegato() :
    vtkInteractorStyleTrackballCamera()
{
}

//------------------------------------------------------------------------------

InteractorStyle2DForNegato::~InteractorStyle2DForNegato()
{
}

//------------------------------------------------------------------------------

void InteractorStyle2DForNegato::OnChar()
{
    vtkRenderWindowInteractor* rwi = this->Interactor;

    switch (rwi->GetKeyCode())
    {
        case 'r':
            this->FindPokedRenderer(rwi->GetEventPosition()[0],
                                    rwi->GetEventPosition()[1]);
            this->CurrentRenderer->ResetCamera();

            if (this->getAutoRender())
            {
                rwi->Render();
            }
            break;
    }
}

//------------------------------------------------------------------------------

void InteractorStyle2DForNegato::OnLeftButtonDown()
{
    this->FindPokedRenderer(this->Interactor->GetEventPosition()[0],
                            this->Interactor->GetEventPosition()[1]);
    if (this->CurrentRenderer == NULL)
    {
        return;
    }

    this->GrabFocus(this->EventCallbackCommand);
    if (this->Interactor->GetShiftKey())
    {
        m_oldPickPoint[0] = this->Interactor->GetEventPosition()[0];
        m_oldPickPoint[1] = this->Interactor->GetEventPosition()[1];
        this->StartPan();
    }
}

//------------------------------------------------------------------------------

void InteractorStyle2DForNegato::OnRightButtonDown()
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

void InteractorStyle2DForNegato::OnMiddleButtonDown()
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

void InteractorStyle2DForNegato::OnMouseMove()
{
    int x = this->Interactor->GetEventPosition()[0];
    int y = this->Interactor->GetEventPosition()[1];

    m_newPickPoint[0] = this->Interactor->GetEventPosition()[0];
    m_newPickPoint[1] = this->Interactor->GetEventPosition()[1];

    switch (this->State)
    {
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
    }
    m_oldPickPoint[0] = m_newPickPoint[0];
    m_oldPickPoint[1] = m_newPickPoint[1];
}

//----------------------------------------------------------------------------

void InteractorStyle2DForNegato::OnMouseWheelBackward()
{
    if (!this->Interactor->GetShiftKey())
    {
        int x = this->Interactor->GetEventPosition()[0];
        int y = this->Interactor->GetEventPosition()[1];
        this->FindPokedRenderer(x, y);
        this->GrabFocus(this->EventCallbackCommand);
        this->StartDolly();
        double factor = this->MotionFactor * -0.2 * this->MouseWheelMotionFactor;
        this->Dolly(pow(1.1, factor));
        this->EndDolly();
        this->ReleaseFocus();
    }
}

//----------------------------------------------------------------------------

void InteractorStyle2DForNegato::OnMouseWheelForward()
{
    if (!this->Interactor->GetShiftKey())
    {
        int x = this->Interactor->GetEventPosition()[0];
        int y = this->Interactor->GetEventPosition()[1];
        this->FindPokedRenderer(x, y);
        this->GrabFocus(this->EventCallbackCommand);
        this->StartDolly();
        double factor = this->MotionFactor * 0.2 * this->MouseWheelMotionFactor;
        this->Dolly(pow(1.1, factor));
        this->EndDolly();
        this->ReleaseFocus();
    }
}

//----------------------------------------------------------------------------

void InteractorStyle2DForNegato::Pan()
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

    vtkRenderWindowInteractor* rwi = this->Interactor;
    double viewFocus[4], focalDepth, viewPoint[3];
    double newPickPoint[4], oldPickPoint[4];
    double motionVector[3];

    // Calculate the focal depth since we'll be using it a lot
    vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
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

    if (this->getAutoRender())
    {
        rwi->Render();
    }
}

//----------------------------------------------------------------------------

void InteractorStyle2DForNegato::Dolly()
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
    double* center = this->CurrentRenderer->GetCenter();
    int dy         = static_cast<int>(m_newPickPoint[1] - m_oldPickPoint[1]);
    double dyf     = this->MotionFactor * dy / center[1];
    this->Dolly(pow(1.1, dyf));
}

//----------------------------------------------------------------------------

void InteractorStyle2DForNegato::Dolly(double factor)
{
    if (this->CurrentRenderer == NULL)
    {
        return;
    }

    vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
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

    if (this->getAutoRender())
    {
        this->Interactor->Render();
    }
}
