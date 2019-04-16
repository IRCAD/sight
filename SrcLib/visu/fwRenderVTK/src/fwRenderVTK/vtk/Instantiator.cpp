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
                           "Select a cell by shooting a ray into graphics window",
                           1,
                           vtkObjectFactoryCreatefwVtkCellPicker);

    this->RegisterOverride("InteractorStyle2DForNegato",
                           "InteractorStyle2DForNegato",
                           "Interactor for 2D Negato",
                           1,
                           vtkObjectFactoryCreateInteractorStyle2DForNegato);

    this->RegisterOverride("InteractorStyle3DForNegato",
                           "InteractorStyle3DForNegato",
                           "Iteractor for 3D Negato",
                           1,
                           vtkObjectFactoryCreateInteractorStyle3DForNegato);

    this->RegisterOverride("FixedInteractorStyle",
                           "FixedInteractorStyle",
                           "Interactor class disabling all user interactions in a VTK scene",
                           1,
                           vtkObjectFactoryCreateFixedInteractorStyle);

    // Registers also useful VTK Classes that needs to be instantiated from xml.

    this->RegisterOverride("vtkImageBlend",
                           "vtkImageBlend",
                           "Blend images together using alpha or opacity",
                           1,
                           vtkObjectFactoryCreatevtkImageBlend);

    this->RegisterOverride("vtkImageCheckerboard",
                           "vtkImageCheckerboard",
                           "Show two images at once using a checkboard pattern",
                           1,
                           vtkObjectFactoryCreatevtkImageCheckerboard);

    this->RegisterOverride("vtkTransform",
                           "vtkTransform",
                           "Describes linear transformations via a 4x4 matrix",
                           1,
                           vtkObjectFactoryCreatevtkTransform);

    this->RegisterOverride("vtkPlaneCollection",
                           "vtkPlaneCollection",
                           "Maintains a list of planes",
                           1,
                           vtkObjectFactoryCreatevtkPlaneCollection);

    this->RegisterOverride("vtkCellPicker",
                           "vtkCellPicker",
                           "Ray-cast cell picker for all kinds of Prop3Ds",
                           1,
                           vtkObjectFactoryCreatevtkCellPicker);
}

//------------------------------------------------------------------------------

} //vtk

} //fwRenderVTK
