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
 ***********************************************************************/

#include "fwRenderVTK/vtk/Instantiator.hpp"

#include "fwRenderVTK/vtk/fwVtkBoxRepresentation.hpp"
#include "fwRenderVTK/vtk/fwVtkCellPicker.hpp"
#include "fwRenderVTK/vtk/fwVtkPicker.hpp"
#include "fwRenderVTK/vtk/InteractorStyle2DForNegato.hpp"
#include "fwRenderVTK/vtk/InteractorStyle3DForNegato.hpp"

#include <fwCore/base.hpp>

#include <vtkInstantiator.h>
#include <vtkVersion.h>

#if (VTK_MAJOR_VERSION < 6 || (VTK_MAJOR_VERSION == 6 && VTK_MINOR_VERSION < 1))
extern vtkObject* vtkInstantiatorfwVtkBoxRepresentationNew();
extern vtkObject* vtkInstantiatorfwVtkPickerNew();
extern vtkObject* vtkInstantiatorfwVtkCellPickerNew();
extern vtkObject* vtkInstantiatorInteractorStyle2DForNegatoNew();
extern vtkObject* vtkInstantiatorInteractorStyle3DForNegatoNew();
#else
vtkInstantiatorNewMacro(fwVtkBoxRepresentation);
vtkInstantiatorNewMacro(fwVtkPicker);
vtkInstantiatorNewMacro(fwVtkCellPicker);
vtkInstantiatorNewMacro(InteractorStyle2DForNegato);
vtkInstantiatorNewMacro(InteractorStyle3DForNegato);
#endif

namespace fwRenderVTK
{

namespace vtk
{

//------------------------------------------------------------------------------

void Instantiator::ClassInitialize()
{
    vtkInstantiator::RegisterInstantiator("fwVtkBoxRepresentation", vtkInstantiatorfwVtkBoxRepresentationNew);
    vtkInstantiator::RegisterInstantiator("fwVtkPicker", vtkInstantiatorfwVtkPickerNew);
    vtkInstantiator::RegisterInstantiator("fwVtkCellPicker", vtkInstantiatorfwVtkCellPickerNew);
    vtkInstantiator::RegisterInstantiator("InteractorStyle2DForNegato", vtkInstantiatorInteractorStyle2DForNegatoNew);
    vtkInstantiator::RegisterInstantiator("InteractorStyle3DForNegato", vtkInstantiatorInteractorStyle3DForNegatoNew);

#ifdef DEBUG
    vtkObject* o;
    o = vtkInstantiator::CreateInstance("fwVtkBoxRepresentation");
    SLM_ASSERT("Unable to instantiate a fwVtkBoxRepresentation", o);
    o->Delete();

    o = vtkInstantiator::CreateInstance("fwVtkPicker");
    SLM_ASSERT("Unable to instantiate a fwVtkPicker", o);
    o->Delete();

    o = vtkInstantiator::CreateInstance("fwVtkCellPicker");
    SLM_ASSERT("Unable to instantiate a fwVtkCellPicker", o);
    o->Delete();

    o = vtkInstantiator::CreateInstance("InteractorStyle2DForNegato");
    SLM_ASSERT("Unable to instantiate a InteractorStyle2DForNegato", o);
    o->Delete();

    o = vtkInstantiator::CreateInstance("InteractorStyle3DForNegato");
    SLM_ASSERT("Unable to instantiate a InteractorStyle3DForNegato", o);
    o->Delete();
#endif
}

//------------------------------------------------------------------------------

void Instantiator::ClassFinalize()
{
    vtkInstantiator::UnRegisterInstantiator("fwVtkBoxRepresentation", vtkInstantiatorfwVtkBoxRepresentationNew);
    vtkInstantiator::UnRegisterInstantiator("fwVtkPicker", vtkInstantiatorfwVtkPickerNew);
    vtkInstantiator::UnRegisterInstantiator("fwVtkCellPicker", vtkInstantiatorfwVtkCellPickerNew);
    vtkInstantiator::UnRegisterInstantiator("InteractorStyle2DForNegato", vtkInstantiatorInteractorStyle2DForNegatoNew);
    vtkInstantiator::UnRegisterInstantiator("InteractorStyle3DForNegato", vtkInstantiatorInteractorStyle3DForNegatoNew);
}
} //vtk

} //fwRenderVTK
