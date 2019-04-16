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

#include "fwRenderVTK/vtk/Instantiator.hpp"

#include "fwRenderVTK/vtk/FixedInteractorStyle.hpp"
#include "fwRenderVTK/vtk/fwVtkCellPicker.hpp"
#include "fwRenderVTK/vtk/InteractorStyle2DForNegato.hpp"
#include "fwRenderVTK/vtk/InteractorStyle3DForNegato.hpp"
#include "fwRenderVTK/vtk/fwHandleRepresentation3D.hpp"
#include "fwRenderVTK/vtk/fwVtkAxesActor.hpp"
#include "fwRenderVTK/vtk/fwVtkBoxRepresentation.hpp"
#include "fwRenderVTK/vtk/fwVtkPicker.hpp"
#include "fwRenderVTK/vtk/fwVtkWheelRepresentation.hpp"
#include "fwRenderVTK/vtk/fwVtkWheelWidget.hpp"
#include "fwRenderVTK/vtk/fwVtkWindowLevelLookupTable.hpp"
#include "fwRenderVTK/vtk/MarkedSphereHandleRepresentation.hpp"

// Useful VTK classes that needs to be instantiated from xml.

#include <vtkCellPicker.h>
#include <vtkImageBlend.h>
#include <vtkImageCheckerboard.h>
#include <vtkTransform.h>
#include <vtkPlaneCollection.h>

namespace fwRenderVTK
{

namespace vtk
{

// Macro to bind creation of vtk object, it will generates "vtkObjectFactory<name>" function.
// each "vtkObjectFactory<name>" will call the <name>::New().
VTK_CREATE_CREATE_FUNCTION(fwVtkCellPicker);
VTK_CREATE_CREATE_FUNCTION(InteractorStyle2DForNegato);
VTK_CREATE_CREATE_FUNCTION(InteractorStyle3DForNegato);
VTK_CREATE_CREATE_FUNCTION(FixedInteractorStyle);

// Thoses are not instatiated using "CreateInstance" but can be in the future.
VTK_CREATE_CREATE_FUNCTION(fwHandleRepresentation3D);
VTK_CREATE_CREATE_FUNCTION(fwVtkAxesActor);
VTK_CREATE_CREATE_FUNCTION(fwVtkBoxRepresentation);
VTK_CREATE_CREATE_FUNCTION(fwVtkPicker);
VTK_CREATE_CREATE_FUNCTION(fwVtkWheelRepresentation);
VTK_CREATE_CREATE_FUNCTION(fwVtkWheelWidget);
VTK_CREATE_CREATE_FUNCTION(fwVtkWindowLevelLookupTable);
VTK_CREATE_CREATE_FUNCTION(MarkedSphereHandleRepresentation);

// VTK classes.
VTK_CREATE_CREATE_FUNCTION(vtkImageBlend);
VTK_CREATE_CREATE_FUNCTION(vtkImageCheckerboard);
VTK_CREATE_CREATE_FUNCTION(vtkTransform);
VTK_CREATE_CREATE_FUNCTION(vtkPlaneCollection);
VTK_CREATE_CREATE_FUNCTION(vtkCellPicker);

//------------------------------------------------------------------------------

Instantiator::Instantiator()
{
    // Registers each needed sight-vtk class to be called by name (in xml for example).
    this->RegisterOverride("fwVtkCellPicker",
                           "fwVtkCellPicker",
                           "Select a cell by shooting a ray into graphics window.",
                           1,
                           vtkObjectFactoryCreatefwVtkCellPicker);

    this->RegisterOverride("InteractorStyle2DForNegato",
                           "InteractorStyle2DForNegato",
                           "Interactor for 2D Negato.",
                           1,
                           vtkObjectFactoryCreateInteractorStyle2DForNegato);

    this->RegisterOverride("InteractorStyle3DForNegato",
                           "InteractorStyle3DForNegato",
                           "Iteractor for 3D Negato.",
                           1,
                           vtkObjectFactoryCreateInteractorStyle3DForNegato);

    this->RegisterOverride("FixedInteractorStyle",
                           "FixedInteractorStyle",
                           "Interactor class disabling all user interactions in a VTK scene.",
                           1,
                           vtkObjectFactoryCreateFixedInteractorStyle);

    // Others sight-vtk classes, actualy not instantiated via factory but may be in the future.

    this->RegisterOverride("fwHandleRepresentation3D",
                           "fwHandleRepresentation3D",
                           "3D Representation to be used with vtkHandleWidget (represents a cube or a sphere).",
                           1,
                           vtkObjectFactoryCreatefwHandleRepresentation3D);

    this->RegisterOverride("fwVtkAxesActor",
                           "fwVtkAxesActor",
                           "Specific and movable vtkAxesActor.",
                           1,
                           vtkObjectFactoryCreatefwVtkAxesActor);

    this->RegisterOverride("fwVtkBoxRepresentation",
                           "fwVtkBoxRepresentation",
                           "Specific vtkBoxRepresentation.",
                           1,
                           vtkObjectFactoryCreatefwVtkBoxRepresentation);

    this->RegisterOverride("fwVtkPicker",
                           "fwVtkPicker",
                           "Superclass for 3D geometric pickers (uses ray cast).",
                           1,
                           vtkObjectFactoryCreatefwVtkPicker);

    this->RegisterOverride("fwVtkWheelRepresentation",
                           "fwVtkWheelRepresentation",
                           "Representation of a wheel widget.",
                           1,
                           vtkObjectFactoryCreatefwVtkWheelRepresentation);

    this->RegisterOverride("fwVtkWheelWidget",
                           "fwVtkWheelWidget",
                           "Implements the interactions with the wheel widget.",
                           1,
                           vtkObjectFactoryCreatefwVtkWheelWidget);

    this->RegisterOverride("fwVtkWindowLevelLookupTable",
                           "fwVtkWindowLevelLookupTable",
                           "Specific implementation of vtkWindowLevelLookupTable adds specific out-of-bounds colors.",
                           1,
                           vtkObjectFactoryCreatefwVtkWindowLevelLookupTable);

    this->RegisterOverride("MarkedSphereHandleRepresentation",
                           "MarkedSphereHandleRepresentation",
                           "Specific implementation of vtkSphereHandleRepresentation.",
                           1,
                           vtkObjectFactoryCreateMarkedSphereHandleRepresentation);

    // Registers also useful VTK Classes that needs to be instantiated from xml.

    this->RegisterOverride("vtkImageBlend",
                           "vtkImageBlend",
                           "Blend images together using alpha or opacity",
                           1,
                           vtkObjectFactoryCreatevtkImageBlend);

    this->RegisterOverride("vtkImageCheckerboard",
                           "vtkImageCheckerboard",
                           "Show two images at once using a checkboard pattern.",
                           1,
                           vtkObjectFactoryCreatevtkImageCheckerboard);

    this->RegisterOverride("vtkTransform",
                           "vtkTransform",
                           "Describes linear transformations via a 4x4 matrix.",
                           1,
                           vtkObjectFactoryCreatevtkTransform);

    this->RegisterOverride("vtkPlaneCollection",
                           "vtkPlaneCollection",
                           "Maintains a list of planes.",
                           1,
                           vtkObjectFactoryCreatevtkPlaneCollection);

    this->RegisterOverride("vtkCellPicker",
                           "vtkCellPicker",
                           "Ray-cast cell picker for all kinds of Prop3Ds.",
                           1,
                           vtkObjectFactoryCreatevtkCellPicker);
}

//------------------------------------------------------------------------------

} //vtk

} //fwRenderVTK
