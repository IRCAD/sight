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

}

//------------------------------------------------------------------------------

} //vtk

} //fwRenderVTK
