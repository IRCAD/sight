/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef __MARKEDSPHEREHANDLEREPRESENTATION_H
#define __MARKEDSPHEREHANDLEREPRESENTATION_H

#include "vtkHandleRepresentation.h"
#include "vtkSphereHandleRepresentation.h"
#include "vtkCylinderSource.h" // Needed for delegation to sphere

class vtkProperty;
class vtkFollower;
class vtkCleanPolyData;

#include "fwRenderVTK/config.hpp"

namespace fwRenderVTK
{

namespace vtk
{


class FWRENDERVTK_CLASS_API  MarkedSphereHandleRepresentation 
                        : public vtkSphereHandleRepresentation
{
public:
  // Description:
  // Instantiate this class.
  FWRENDERVTK_API static MarkedSphereHandleRepresentation *New();

  // Description:
  // Standard methods for instances of this class.
  vtkTypeRevisionMacro(MarkedSphereHandleRepresentation,vtkHandleRepresentation);
  FWRENDERVTK_API void PrintSelf(ostream& os, vtkIndent indent);

  FWRENDERVTK_API virtual void SetWorldPosition(double p[3]);

  // Description:
  // Set/Get the handle properties when unselected and selected.
  FWRENDERVTK_API void SetMarkerProperty(vtkProperty*);
  FWRENDERVTK_API vtkGetObjectMacro(MarkerProperty,vtkProperty);
  
  // Description:
  // Methods to make this class properly act like a vtkWidgetRepresentation.
  FWRENDERVTK_API virtual void BuildRepresentation();
  FWRENDERVTK_API virtual void GetActors(vtkPropCollection *);
  FWRENDERVTK_API virtual void ReleaseGraphicsResources(vtkWindow *);
  FWRENDERVTK_API virtual int RenderOpaqueGeometry(vtkViewport *viewport);
  FWRENDERVTK_API virtual int RenderTranslucentPolygonalGeometry(vtkViewport *viewport);
  FWRENDERVTK_API virtual int HasTranslucentPolygonalGeometry();

protected:
  MarkedSphereHandleRepresentation();
  ~MarkedSphereHandleRepresentation();

  // the cursor3D
  vtkFollower       *Follower;
  vtkCleanPolyData  *CleanPolyData;
  vtkPolyDataMapper *MarkerMapper;
  vtkCylinderSource *Marker;

  double MarkerRadiusFactor;

  // Properties used to control the appearance of selected objects and
  // the manipulator in general.
  vtkProperty *MarkerProperty;
  void         CreateDefaultProperties();
  
private:
  MarkedSphereHandleRepresentation(const MarkedSphereHandleRepresentation&);  //Not implemented
  void operator=(const MarkedSphereHandleRepresentation&);  //Not implemented
};


} // namespace vtk

} // namespace fwRenderVTK

#endif //__MARKEDSPHEREHANDLEREPRESENTATION_H
