/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include "vtkAssemblyPath.h"
#include "vtkCellPicker.h"
#include "vtkCamera.h"
#include "vtkCleanPolyData.h"
#include "vtkCoordinate.h"
#include "vtkCylinderSource.h"
#include "vtkFollower.h"
#include "vtkInteractorObserver.h"
#include "vtkLine.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataNormals.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"


#include "fwRenderVTK/vtk/MarkedSphereHandleRepresentation.hpp"

namespace fwRenderVTK
{

namespace vtk
{

vtkCxxRevisionMacro(MarkedSphereHandleRepresentation, "$Revision: 0.0 $");
vtkStandardNewMacro(MarkedSphereHandleRepresentation);

//----------------------------------------------------------------------
MarkedSphereHandleRepresentation::MarkedSphereHandleRepresentation() : vtkSphereHandleRepresentation()
{
    this->Marker = vtkCylinderSource::New();
    this->Marker->SetCenter(0.,-1.,0.);
    this->Marker->SetResolution(64);
    this->Marker->SetHeight(0.);

    this->CleanPolyData = vtkCleanPolyData::New();
    this->CleanPolyData->PointMergingOn();
    this->CleanPolyData->CreateDefaultLocator();
    this->CleanPolyData->SetInputConnection(0, this->Marker->GetOutputPort(0));

    vtkPolyDataNormals *MarkerNormals = vtkPolyDataNormals::New();
    MarkerNormals->SetInputConnection( 0, this->CleanPolyData->GetOutputPort(0) );

    this->MarkerMapper = vtkPolyDataMapper::New();
    this->MarkerMapper->SetInput( MarkerNormals->GetOutput() );
    MarkerNormals->Delete();

    this->Follower = vtkFollower::New();
    this->Follower->SetMapper(this->MarkerMapper);
    this->Follower->RotateX(90);

    // Set up the initial properties, parent's one is called in parent's constructor
    this->CreateDefaultProperties();

    this->MarkerRadiusFactor = 1.5;
    this->SetMarkerProperty(this->MarkerProperty);
    this->Marker->SetRadius(this->MarkerRadiusFactor * this->Sphere->GetRadius());
}

//----------------------------------------------------------------------
MarkedSphereHandleRepresentation::~MarkedSphereHandleRepresentation()
{
  this->Marker->Delete();
  this->CleanPolyData->Delete();
  this->MarkerMapper->Delete();
  this->Follower->Delete();
  this->MarkerProperty->Delete();
}

//-------------------------------------------------------------------------
void MarkedSphereHandleRepresentation::SetWorldPosition(double p[3])
{
  this->vtkSphereHandleRepresentation::SetWorldPosition(p);
  this->Follower->SetPosition(this->GetWorldPosition());// p may have been clamped
}

//----------------------------------------------------------------------
void MarkedSphereHandleRepresentation::CreateDefaultProperties()
{
    this->MarkerProperty = vtkProperty::New();
    this->MarkerProperty->SetColor(1., 1., 0.);
    this->MarkerProperty->SetOpacity(0.5);
}

//----------------------------------------------------------------------
void MarkedSphereHandleRepresentation::BuildRepresentation()
{
  // The net effect is to resize the handle
//  if ( this->GetMTime() > this->BuildTime ||
//       (this->Renderer && this->Renderer->GetVTKWindow() &&
//        this->Renderer->GetVTKWindow()->GetMTime() > this->BuildTime) )
//  {
      if ( ! this->Placed )
      {
          this->ValidPick = 1;
          this->Placed = 1;
      }

      this->SizeBounds();
      this->Sphere->Update();

      this->Follower->SetCamera( this->GetRenderer()->GetActiveCamera() );
      this->Marker->SetRadius(this->MarkerRadiusFactor * this->Sphere->GetRadius() );
      this->Marker->Update();
      this->BuildTime.Modified();
//  }

}
//
//----------------------------------------------------------------------
void MarkedSphereHandleRepresentation::GetActors(vtkPropCollection *pc)
{
  this->Actor->GetActors(pc);
  this->Follower->GetActors(pc);
}

//----------------------------------------------------------------------
void MarkedSphereHandleRepresentation::ReleaseGraphicsResources(vtkWindow *win)
{
  this->Actor->ReleaseGraphicsResources(win);
  this->Follower->ReleaseGraphicsResources(win);
}

//----------------------------------------------------------------------
int MarkedSphereHandleRepresentation::RenderOpaqueGeometry(vtkViewport *viewport)
{
  this->BuildRepresentation();
  int ret = 0;
  if (this->GetRenderer()->GetActiveCamera()->GetParallelProjection())
  {
      ret = this->Follower->RenderOpaqueGeometry(viewport);
  }
  return this->Actor->RenderOpaqueGeometry(viewport) + ret;
}

//----------------------------------------------------------------------
int MarkedSphereHandleRepresentation::RenderTranslucentPolygonalGeometry(vtkViewport *viewport)
{
  this->BuildRepresentation();
  int ret = 0;
  if (this->GetRenderer()->GetActiveCamera()->GetParallelProjection())
  {
     ret = this->Follower->RenderTranslucentPolygonalGeometry(viewport);
  }
  return this->Actor->RenderTranslucentPolygonalGeometry(viewport) + ret;
}

//----------------------------------------------------------------------
int MarkedSphereHandleRepresentation::HasTranslucentPolygonalGeometry()
{
    return 1;
}

//----------------------------------------------------------------------
void MarkedSphereHandleRepresentation::SetMarkerProperty(vtkProperty * p)
{
    vtkSetObjectBodyMacro(MarkerProperty, vtkProperty, p);
    if (p)
    {
        this->Follower->SetProperty( p );
    }
}

//----------------------------------------------------------------------
void MarkedSphereHandleRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
    this->vtkSphereHandleRepresentation::PrintSelf(os,indent);

    if ( this->MarkerProperty )
    {
        os << indent << "Selected Property: " << this->MarkerProperty << "\n";
    }
    else
    {
        os << indent << "Marker Property: (none)\n";
    }

    this->Sphere->PrintSelf(os,indent.GetNextIndent());
}

} // namespace vtk

} // namespace fwRenderVTK

