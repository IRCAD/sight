/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
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

#ifndef ANDROID

#if VTK_MAJOR_VERSION < 7
#include <vtkShaderProgram2.h>
#include <vtkShader2.h>
#include <vtkShader2Collection.h>
#else
#include <vtkShaderProgram.h>
#include <vtkShader.h>
#endif // VTK_MAJOR_VERSION < 7

#endif // ANDROID

namespace fwRenderVTK
{
namespace vtk
{

FWRENDERVTK_API vtkIdType getNearestPointId(vtkPoints* pts, vtkRenderer* renderer);
FWRENDERVTK_API vtkProp* getNearestPickedProp(vtkAbstractPropPicker* picker, vtkRenderer* renderer);
FWRENDERVTK_API bool     getNearestPickedPosition(vtkAbstractPropPicker* picker, vtkRenderer* renderer,
                                                  double* position);

#ifndef ANDROID
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
#endif

} //vtk
} //fwRenderVTK

#endif //__FWRENDERVTK_VTK_HELPERS_HPP__

