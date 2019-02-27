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
    // Method to avoid vtk resetting the visibility (reimplemented from vtkSphereHandleRepresentation)
    FWRENDERVTK_API virtual int ComputeInteractionState(int X, int Y, int vtkNotUsed(modify)) override;

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
