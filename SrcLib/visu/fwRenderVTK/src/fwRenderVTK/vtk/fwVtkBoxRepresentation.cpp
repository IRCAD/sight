/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/*=========================================================================
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   Patched version of vtkBoxRepresentation,
   see http://public.kitware.com/pipermail/vtkusers/2010-July/110328.html
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 =========================================================================*/

/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: fwVtkBoxRepresentation.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <vtkBoxRepresentation.h>
#include <vtkActor.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkCallbackCommand.h>
#include <vtkBox.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkInteractorObserver.h>
#include <vtkMath.h>
#include <vtkCellArray.h>
#include <vtkCellPicker.h>
#include <vtkTransform.h>
#include <vtkDoubleArray.h>
#include <vtkBox.h>
#include <vtkPlanes.h>
#include <vtkCamera.h>
#include <vtkAssemblyPath.h>
#include <vtkWindow.h>
#include <vtkObjectFactory.h>

#include "fwRenderVTK/vtk/fwVtkBoxRepresentation.hpp"

vtkCxxRevisionMacro(fwVtkBoxRepresentation, "$Revision: 1.9 $");
vtkStandardNewMacro(fwVtkBoxRepresentation);

//----------------------------------------------------------------------------
fwVtkBoxRepresentation::fwVtkBoxRepresentation() : vtkBoxRepresentation()
{
  // Enable/disable the translation, rotation, and scaling of the widget
  this->TranslationEnabled = 1;
  this->RotationEnabled = 1;
  this->ScalingEnabled = 1;
}

//----------------------------------------------------------------------
void fwVtkBoxRepresentation::WidgetInteraction(double e[2])
{
  // Convert events to appropriate coordinate systems
  vtkCamera *camera = this->Renderer->GetActiveCamera();
  if ( !camera )
    {
    return;
    }
  double focalPoint[4], pickPoint[4], prevPickPoint[4];
  double z, vpn[3];
  camera->GetViewPlaneNormal(vpn);

  // Compute the two points defining the motion vector
  double pos[3];
  if ( this->LastPicker == this->HexPicker )
    {
    this->HexPicker->GetPickPosition(pos);
    }
  else
    {
    this->HandlePicker->GetPickPosition(pos);
    }
  vtkInteractorObserver::ComputeWorldToDisplay(this->Renderer,
                                               pos[0], pos[1], pos[2],
                                               focalPoint);
  z = focalPoint[2];
  vtkInteractorObserver::ComputeDisplayToWorld(this->Renderer,this->LastEventPosition[0],
                                               this->LastEventPosition[1], z, prevPickPoint);
  vtkInteractorObserver::ComputeDisplayToWorld(this->Renderer, e[0], e[1], z, pickPoint);

  // Process the motion
  if ( this->ScalingEnabled && this->InteractionState == fwVtkBoxRepresentation::MoveF0 )
    {
    this->MoveMinusXFace(prevPickPoint,pickPoint);
    }

  else if ( this->ScalingEnabled && this->InteractionState == fwVtkBoxRepresentation::MoveF1 )
    {
    this->MovePlusXFace(prevPickPoint,pickPoint);
    }

  else if ( this->ScalingEnabled && this->InteractionState == fwVtkBoxRepresentation::MoveF2 )
    {
    this->MoveMinusYFace(prevPickPoint,pickPoint);
    }

  else if ( this->ScalingEnabled && this->InteractionState == fwVtkBoxRepresentation::MoveF3 )
    {
    this->MovePlusYFace(prevPickPoint,pickPoint);
    }

  else if ( this->ScalingEnabled && this->InteractionState == fwVtkBoxRepresentation::MoveF4 )
    {
    this->MoveMinusZFace(prevPickPoint,pickPoint);
    }

  else if ( this->ScalingEnabled && this->InteractionState == fwVtkBoxRepresentation::MoveF5 )
    {
    this->MovePlusZFace(prevPickPoint,pickPoint);
    }

  else if ( this->TranslationEnabled && this->InteractionState == fwVtkBoxRepresentation::Translating )
    {
    this->Translate(prevPickPoint, pickPoint);
    }

  else if ( this->ScalingEnabled && this->InteractionState == fwVtkBoxRepresentation::Scaling )
    {
    this->Scale(prevPickPoint, pickPoint,
                static_cast<int>(e[0]), static_cast<int>(e[1]));
    }

  else if ( this->RotationEnabled && this->InteractionState == fwVtkBoxRepresentation::Rotating )
    {
    this->Rotate(static_cast<int>(e[0]), static_cast<int>(e[1]), prevPickPoint, pickPoint, vpn);
    }

  // Store the start position
  this->LastEventPosition[0] = e[0];
  this->LastEventPosition[1] = e[1];
  this->LastEventPosition[2] = 0.0;
}

//----------------------------------------------------------------------------
void fwVtkBoxRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Translation Enabled: " << (this->TranslationEnabled ? "On\n" : "Off\n");
  os << indent << "Scaling Enabled: " << (this->ScalingEnabled ? "On\n" : "Off\n");
  os << indent << "Rotation Enabled: " << (this->RotationEnabled ? "On\n" : "Off\n");
}
