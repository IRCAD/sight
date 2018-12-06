/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 *   Program:   Visualization Toolkit
 *   Module:    $RCSfile: vtkBoxRepresentation.h,v $
 *
 *   Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 *   All rights reserved.
 *   See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 *
 *   This software is distributed WITHOUT ANY WARRANTY; without even
 *   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *   PURPOSE.  See the above copyright notice for more information.
 *
 ***********************************************************************/

#pragma once

// .NAME vtkBoxRepresentation - a class defining the representation for the vtkBoxWidget2
// .SECTION Description
// This class is a concrete representation for the vtkBoxWidget2. It
// represents a box with seven handles: one on each of the six faces, plus a
// center handle. Through interaction with the widget, the box
// representation can be arbitrarily positioned in the 3D space.
//
// To use this representation, you normally use the PlaceWidget() method
// to position the widget at a specified region in space.
//
// .SECTION Caveats
// This class, and vtkBoxWidget2, are second generation VTK
// widgets. An earlier version of this functionality was defined in the
// class vtkBoxWidget.

// .SECTION See Also
// vtkBoxWidget2 vtkBoxWidget

#include "fwRenderVTK/config.hpp"

#include <vtkBoxRepresentation.h>
#include <vtkWidgetRepresentation.h>

class vtkActor;
class vtkPolyDataMapper;
class vtkLineSource;
class vtkSphereSource;
class vtkCellPicker;
class vtkProperty;
class vtkPolyData;
class vtkPoints;
class vtkPolyDataAlgorithm;
class vtkPointHandleRepresentation3D;
class vtkTransform;
class vtkPlanes;
class vtkBox;
class vtkDoubleArray;
class vtkMatrix4x4;

class FWRENDERVTK_CLASS_API fwVtkBoxRepresentation : public vtkBoxRepresentation
{
public:
    // Description:
    // Instantiate the class.
    FWRENDERVTK_API static fwVtkBoxRepresentation* New();

    // Description:
    // Standard methods for the class.
    vtkTypeMacro(fwVtkBoxRepresentation, vtkBoxRepresentation);
    void PrintSelf(ostream& os, vtkIndent indent) override;

    // Description:
    // Control the behavior of the widget. Translation, rotation, and
    // scaling can all be enabled and disabled.
    vtkSetMacro(TranslationEnabled, int);
    vtkGetMacro(TranslationEnabled, int);
    vtkBooleanMacro(TranslationEnabled, int);
    vtkSetMacro(ScalingEnabled, int);
    vtkGetMacro(ScalingEnabled, int);
    vtkBooleanMacro(ScalingEnabled, int);
    vtkSetMacro(RotationEnabled, int);
    vtkGetMacro(RotationEnabled, int);
    vtkBooleanMacro(RotationEnabled, int);

//  // Description:
//  // These are methods that satisfy vtkWidgetRepresentation's API.
    virtual void WidgetInteraction(double e[2]) override;

protected:
    fwVtkBoxRepresentation();

    // Control whether scaling, rotation, and translation are supported
    int TranslationEnabled;
    int ScalingEnabled;
    int RotationEnabled;

};
