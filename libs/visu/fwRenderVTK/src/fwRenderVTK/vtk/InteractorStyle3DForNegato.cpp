/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "fwRenderVTK/vtk/InteractorStyle3DForNegato.hpp"

#include "vtkCellPicker.h"

#include <fwCore/base.hpp>

#include <vtkCallbackCommand.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkMath.h>
#include <vtkObjectFactory.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>

namespace fwRenderVTK
{

namespace vtk
{

vtkStandardNewMacro(InteractorStyle3DForNegato);

//------------------------------------------------------------------------------

InteractorStyle3DForNegato::InteractorStyle3DForNegato() :
    vtkInteractorStyleTrackballCamera()
{
}

//------------------------------------------------------------------------------

InteractorStyle3DForNegato::~InteractorStyle3DForNegato()
{
}

//------------------------------------------------------------------------------

void InteractorStyle3DForNegato::OnChar()
{
    if(this->CurrentRenderer == nullptr)
    {
        return;
    }

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
        case 'f':
        case 'F':
        {
            this->AnimState = VTKIS_ANIM_ON;
            vtkAssemblyPath* path = NULL;
            this->FindPokedRenderer(rwi->GetEventPosition()[0],
                                    rwi->GetEventPosition()[1]);
            rwi->GetPicker()->Pick(rwi->GetEventPosition()[0],
                                   rwi->GetEventPosition()[1],
                                   0.0,
                                   this->CurrentRenderer);
            vtkAbstractPropPicker* picker;
            if ((picker = vtkAbstractPropPicker::SafeDownCast(rwi->GetPicker())))
            {
                path = picker->GetPath();
            }
            if (path != NULL)
            {
                rwi->FlyTo(this->CurrentRenderer, picker->GetPickPosition());
            }
            this->AnimState = VTKIS_ANIM_OFF;
        }
        break;

    }
}

//------------------------------------------------------------------------------

void InteractorStyle3DForNegato::OnKeyUp()
{
    vtkRenderWindowInteractor* rwi = this->Interactor;

    switch (rwi->GetKeyCode())
    {
        case 'q':
            OnLeftButtonUp();
            break;
    }
}

//------------------------------------------------------------------------------

void InteractorStyle3DForNegato::OnKeyDown()
{
    vtkRenderWindowInteractor* rwi = this->Interactor;

    switch (rwi->GetKeyCode())
    {
        case 'q':
            OnLeftButtonDown();
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
    double* center = this->CurrentRenderer->GetCenter();
    int dy         = static_cast<int>(m_newPickPoint[1] - m_oldPickPoint[1]);
    double dyf     = this->MotionFactor * dy / center[1];
    this->Dolly(pow(1.1, dyf));
}

//----------------------------------------------------------------------------

void InteractorStyle3DForNegato::Dolly(double factor)
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

//----------------------------------------------------------------------------

void InteractorStyle3DForNegato::Rotate()
{
    if (this->CurrentRenderer == NULL)
    {
        return;
    }
    vtkRenderWindowInteractor* rwi = this->Interactor;
    int dx                         = static_cast<int>(m_newPickPoint[0] - m_oldPickPoint[0]);
    int dy                         = static_cast<int>(m_newPickPoint[1] - m_oldPickPoint[1]);
    int* size                      = this->CurrentRenderer->GetRenderWindow()->GetSize();
    double delta_elevation         = -20.0 / size[1];
    double delta_azimuth           = -20.0 / size[0];
    double rxf                     = dx * delta_azimuth * this->MotionFactor;
    double ryf                     = dy * delta_elevation * this->MotionFactor;

    vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
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

    if (this->getAutoRender())
    {
        rwi->Render();
    }
}

//----------------------------------------------------------------------------

void InteractorStyle3DForNegato::Spin()
{
    if ( this->CurrentRenderer == NULL )
    {
        return;
    }
    vtkRenderWindowInteractor* rwi = this->Interactor;
    double* center                 = this->CurrentRenderer->GetCenter();
    double newAngle                =
        vtkMath::DegreesFromRadians( atan2( m_newPickPoint[1] - center[1],
                                            m_newPickPoint[0] - center[0] ) );
    double oldAngle =
        vtkMath::DegreesFromRadians( atan2( m_oldPickPoint[1] - center[1],
                                            m_oldPickPoint[0] - center[0] ) );
    vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();
    camera->Roll( newAngle - oldAngle );
    camera->OrthogonalizeViewUp();

    if (this->getAutoRender())
    {
        rwi->Render();
    }
}

//----------------------------------------------------------------------------

} // namespace vtk

} // namespace fwRenderVTK
