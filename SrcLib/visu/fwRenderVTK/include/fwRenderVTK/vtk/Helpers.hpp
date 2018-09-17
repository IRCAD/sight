/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwRenderVTK/config.hpp"

#include <vtkType.h>
#include <vtkSmartPointer.h>

class vtkProp;
class vtkPoints;
class vtkRenderer;
class vtkAbstractPropPicker;
class vtkRenderWindow;

#if VTK_MAJOR_VERSION < 7
#include <vtkShaderProgram2.h>
#include <vtkShader2.h>
#include <vtkShader2Collection.h>
#else
#include <vtkShaderProgram.h>
#include <vtkShader.h>
#endif // VTK_MAJOR_VERSION < 7

namespace fwRenderVTK
{
namespace vtk
{

FWRENDERVTK_API vtkIdType getNearestPointId(vtkPoints* pts, vtkRenderer* renderer);
FWRENDERVTK_API vtkProp* getNearestPickedProp(vtkAbstractPropPicker* picker, vtkRenderer* renderer);
FWRENDERVTK_API bool     getNearestPickedPosition(vtkAbstractPropPicker* picker, vtkRenderer* renderer,
                                                  double* position);

#if VTK_MAJOR_VERSION < 7
FWRENDERVTK_API vtkSmartPointer<vtkShaderProgram2> buildShader(vtkRenderWindow* pWindow,
                                                               const char* pcVertexShader,
                                                               const char* pcFragmentShader );

FWRENDERVTK_API vtkSmartPointer<vtkShaderProgram2> buildShaderFromFile( vtkRenderWindow* pWindow,
                                                                        const char* pcVertexName,
                                                                        const char* pcFragmentName );
#else
FWRENDERVTK_API vtkSmartPointer<vtkShaderProgram> buildShader(vtkRenderWindow* pWindow,
                                                              const char* pcVertexShader,
                                                              const char* pcFragmentShader );

FWRENDERVTK_API vtkSmartPointer<vtkShaderProgram> buildShaderFromFile( vtkRenderWindow* pWindow,
                                                                       const char* pcVertexName,
                                                                       const char* pcFragmentName );
#endif // VTK_MAJOR_VERSION < 7

} //vtk
} //fwRenderVTK
