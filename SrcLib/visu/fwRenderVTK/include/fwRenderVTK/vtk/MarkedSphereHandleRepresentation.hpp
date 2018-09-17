/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwRenderVTK/config.hpp"

#include <vtkCylinderSource.h> // Needed for delegation to sphere
#include <vtkHandleRepresentation.h>
#include <vtkSphereHandleRepresentation.h>

class vtkProperty;
class vtkFollower;
class vtkCleanPolyData;

namespace fwRenderVTK
{

namespace vtk
{

class FWRENDERVTK_CLASS_API MarkedSphereHandleRepresentation :
    public vtkSphereHandleRepresentation
{
public:
    // Description:
    // Instantiate this class.
    FWRENDERVTK_API static MarkedSphereHandleRepresentation* New();

    // Description:
    // Standard methods for instances of this class.
    vtkTypeMacro(MarkedSphereHandleRepresentation, vtkHandleRepresentation);
    FWRENDERVTK_API void PrintSelf(ostream& os, vtkIndent indent) override;

    FWRENDERVTK_API virtual void SetWorldPosition(double p[3]) override;

    // Description:
    // Set/Get the handle properties when unselected and selected.
    FWRENDERVTK_API void SetMarkerProperty(vtkProperty*);
    FWRENDERVTK_API vtkGetObjectMacro(MarkerProperty, vtkProperty);

    // Description:
    // Methods to make this class properly act like a vtkWidgetRepresentation.
    FWRENDERVTK_API virtual void BuildRepresentation() override;
    FWRENDERVTK_API virtual void GetActors(vtkPropCollection*) override;
    FWRENDERVTK_API virtual void ReleaseGraphicsResources(vtkWindow*) override;
    FWRENDERVTK_API virtual int RenderOpaqueGeometry(vtkViewport* viewport) override;
    FWRENDERVTK_API virtual int RenderTranslucentPolygonalGeometry(vtkViewport* viewport) override;
    FWRENDERVTK_API virtual int HasTranslucentPolygonalGeometry() override;

protected:
    MarkedSphereHandleRepresentation();
    ~MarkedSphereHandleRepresentation();

    // the cursor3D
    vtkFollower* Follower;
    vtkCleanPolyData* CleanPolyData;
    vtkPolyDataMapper* MarkerMapper;
    vtkCylinderSource* Marker;

    double MarkerRadiusFactor;

    // Properties used to control the appearance of selected objects and
    // the manipulator in general.
    vtkProperty* MarkerProperty;
    void         CreateDefaultProperties();

private:
    MarkedSphereHandleRepresentation(const MarkedSphereHandleRepresentation&); //Not implemented
    void operator=(const MarkedSphereHandleRepresentation&); //Not implemented
};

} // namespace vtk

} // namespace fwRenderVTK
