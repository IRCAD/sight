/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <fwCore/base.hpp>
#include <vtkInstantiator.h>

#include "fwRenderVTK/vtk/Instantiator.hpp"


extern vtkObject* vtkInstantiatorfwVtkPickerNew();
extern vtkObject* vtkInstantiatorfwVtkCellPickerNew();
extern vtkObject* vtkInstantiatorInteractorStyle2DForNegatoNew();
extern vtkObject* vtkInstantiatorInteractorStyle3DForNegatoNew();

namespace fwRenderVTK {

namespace vtk {

void Instantiator::ClassInitialize()
{
    vtkInstantiator::RegisterInstantiator("fwVtkPicker", vtkInstantiatorfwVtkPickerNew);
    vtkInstantiator::RegisterInstantiator("fwVtkCellPicker", vtkInstantiatorfwVtkCellPickerNew);
    vtkInstantiator::RegisterInstantiator("InteractorStyle2DForNegato", vtkInstantiatorInteractorStyle2DForNegatoNew);
    vtkInstantiator::RegisterInstantiator("InteractorStyle3DForNegato", vtkInstantiatorInteractorStyle3DForNegatoNew);

#ifdef DEBUG
    vtkObject *o = vtkInstantiator::CreateInstance("fwVtkPicker");
    SLM_ASSERT("Unable to instantiate a fwVtkPicker",o);
    o->Delete();

    o = vtkInstantiator::CreateInstance("fwVtkCellPicker");
    SLM_ASSERT("Unable to instantiate a fwVtkCellPicker",o);
    o->Delete();

    o = vtkInstantiator::CreateInstance("InteractorStyle2DForNegato");
    SLM_ASSERT("Unable to instantiate a InteractorStyle2DForNegato",o);
    o->Delete();

    o = vtkInstantiator::CreateInstance("InteractorStyle3DForNegato");
    SLM_ASSERT("Unable to instantiate a InteractorStyle3DForNegato",o);
    o->Delete();
#endif
}

void Instantiator::ClassFinalize()
{
    vtkInstantiator::UnRegisterInstantiator("fwVtkPicker", vtkInstantiatorfwVtkPickerNew);
    vtkInstantiator::UnRegisterInstantiator("fwVtkCellPicker", vtkInstantiatorfwVtkCellPickerNew);
    vtkInstantiator::UnRegisterInstantiator("InteractorStyle2DForNegato", vtkInstantiatorInteractorStyle2DForNegatoNew);
    vtkInstantiator::UnRegisterInstantiator("InteractorStyle3DForNegato", vtkInstantiatorInteractorStyle2DForNegatoNew);
}
} //vtk

} //fwRenderVTK
