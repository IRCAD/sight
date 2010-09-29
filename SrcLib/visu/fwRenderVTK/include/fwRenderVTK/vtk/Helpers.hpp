/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <vtkType.h>

#include "fwRenderVTK/config.hpp"


class vtkProp;
class vtkPoints;
class vtkRenderer;
class vtkAbstractPropPicker;

namespace fwRenderVTK {

namespace vtk {

FWRENDERVTK_API vtkIdType getNearestPointId(vtkPoints* pts, vtkRenderer* renderer);
FWRENDERVTK_API vtkProp  *getNearestPickedProp(vtkAbstractPropPicker *picker, vtkRenderer *renderer);
FWRENDERVTK_API bool      getNearestPickedPosition(vtkAbstractPropPicker *picker, vtkRenderer *renderer, double *position);

} //vtk

} //fwRenderVTK



