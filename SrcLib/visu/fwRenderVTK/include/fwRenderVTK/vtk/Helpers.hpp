/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERVTK_VTK_HELPERS_HPP__
#define __FWRENDERVTK_VTK_HELPERS_HPP__

#include <vtkType.h>
#include <vtkSmartPointer.h>

#include "fwRenderVTK/config.hpp"

class vtkProp;
class vtkPoints;
class vtkRenderer;
class vtkAbstractPropPicker;
class vtkRenderWindow;
class vtkShaderProgram2;

namespace fwRenderVTK
{

namespace vtk
{

FWRENDERVTK_API vtkIdType getNearestPointId(vtkPoints* pts, vtkRenderer* renderer);
FWRENDERVTK_API vtkProp  *getNearestPickedProp(vtkAbstractPropPicker *picker, vtkRenderer *renderer);
FWRENDERVTK_API bool      getNearestPickedPosition(vtkAbstractPropPicker *picker, vtkRenderer *renderer,
                                                   double *position);

FWRENDERVTK_API vtkSmartPointer<vtkShaderProgram2> buildShader(vtkRenderWindow* pWindow,
                                                               const char* pcVertexShader,
                                                               const char* pcFragmentShader );

FWRENDERVTK_API vtkSmartPointer<vtkShaderProgram2> buildShaderFromFile( vtkRenderWindow* pWindow,
                                                                        const char* pcVertexName,
                                                                        const char* pcFragmentName );

} //vtk

} //fwRenderVTK

#endif //__FWRENDERVTK_VTK_HELPERS_HPP__
