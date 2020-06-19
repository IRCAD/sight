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

#include "fwRenderVTK/vtk/Helpers.hpp"

#include <fwCore/base.hpp>

#include <vtkAssemblyNode.h>
#include <vtkAssemblyPath.h>
#include <vtkCamera.h>
#include <vtkMath.h>
#include <vtkOpenGLRenderWindow.h>
#include <vtkPicker.h>
#include <vtkPoints.h>
#include <vtkProp3DCollection.h>
#include <vtkRenderer.h>

namespace fwRenderVTK
{

namespace vtk
{

//------------------------------------------------------------------------------

vtkIdType getNearestPointId(vtkPoints* pts, vtkRenderer* renderer)
{
    vtkIdType id = -1;
    double camPosition[3];
    double distance = VTK_DOUBLE_MAX;
    renderer->GetActiveCamera()->GetPosition(camPosition);

    for(vtkIdType i = 0; i < pts->GetNumberOfPoints(); i++)
    {
        double* point        = pts->GetPoint(i);
        double distancePtCam = vtkMath::Distance2BetweenPoints(point, camPosition);

        if(distancePtCam < distance)
        {
            id       = i;
            distance = distancePtCam;
        }
    }

    return id;
}

//------------------------------------------------------------------------------

vtkProp* getNearestPickedProp(vtkAbstractPropPicker* picker, vtkRenderer* renderer)
{
    vtkProp* res         = NULL;
    vtkPicker* vtkpicker = vtkPicker::SafeDownCast(picker);

    SLM_ASSERT("getNearestPickedProp *need* a picker.", picker);
    SLM_ASSERT("getNearestPickedProp *need* a renderer.", renderer);

    if (vtkpicker)
    {
        vtkIdType id = getNearestPointId(vtkpicker->GetPickedPositions(), renderer);

        if (id > -1 && vtkpicker->GetProp3Ds()->GetNumberOfItems() > id)
        {
            res = vtkProp::SafeDownCast(vtkpicker->GetProp3Ds()->GetItemAsObject(static_cast<int>(id)));
        }
    }
    else
    {
        res = picker->GetProp3D();
    }
    return res;
}

//------------------------------------------------------------------------------

bool getNearestPickedPosition(vtkAbstractPropPicker* picker, vtkRenderer* renderer, double position[3])
{
    bool res             = false;
    vtkPicker* vtkpicker = vtkPicker::SafeDownCast(picker);

    SLM_ASSERT("getNearestPickedProp *need* a picker.", picker);
    SLM_ASSERT("getNearestPickedProp *need* a renderer.", renderer);

    double* point = NULL;
    if (vtkpicker)
    {
        vtkPoints* pts = vtkpicker->GetPickedPositions();
        vtkIdType id   = getNearestPointId(pts, renderer);

        if (id > -1)
        {
            point = pts->GetPoint(id);
        }
        else
        {
            point = vtkpicker->GetPickPosition();
        }
    }
    else
    {
        point = picker->GetPickPosition();
    }

    if(point)
    {
        std::copy(point, point + 3, position);
        res = true;
    }
    return res;
}

//------------------------------------------------------------------------------

#if VTK_MAJOR_VERSION >= 7
vtkSmartPointer<vtkShaderProgram>
#else
vtkSmartPointer<vtkShaderProgram2>
#endif
//------------------------------------------------------------------------------

buildShader( vtkOpenGLRenderWindow* pWindow,
             const char* pcVertexShader,
             const char* pcFragmentShader )
{
    SLM_ASSERT( "NULL parameter", pWindow && pcVertexShader && pcFragmentShader );

    vtkOpenGLRenderWindow* pOpenGLWindow = vtkOpenGLRenderWindow::SafeDownCast(pWindow);
    if(!pOpenGLWindow)
    {
        SLM_ERROR("Shader only supported using OpenGL.");
        return NULL;
    }

#if VTK_MAJOR_VERSION >= 7
    vtkSmartPointer<vtkShaderProgram> pProgram = vtkSmartPointer<vtkShaderProgram>::New();
    {
        // The vertex shader
        vtkShader* shader = vtkShader::New();
        shader->SetType(vtkShader::Vertex);
        shader->SetSource(pcVertexShader);
        pProgram->SetVertexShader(shader);
        shader->Delete();
    }
    {
        // The fragment shader
        vtkShader* shader = vtkShader::New();
        shader->SetType(vtkShader::Fragment);
        shader->SetSource(pcFragmentShader);
        pProgram->SetFragmentShader(shader);
        shader->Delete();
    }
#else
    vtkSmartPointer<vtkShaderProgram2> pProgram = vtkSmartPointer<vtkShaderProgram2>::New();
    {
        vtkShader2* shader = vtkShader2::New();
        shader->SetType(VTK_SHADER_TYPE_VERTEX);
        shader->SetSourceCode(pcVertexShader);
        shader->SetContext(pProgram->GetContext());
        pProgram->GetShaders()->AddItem(shader);
        shader->Delete();
    }
    {
        // The fragment shader
        vtkShader2* shader = vtkShader2::New();
        shader->SetType(VTK_SHADER_TYPE_FRAGMENT);
        shader->SetSourceCode(pcFragmentShader);
        shader->SetContext(pProgram->GetContext());
        pProgram->GetShaders()->AddItem(shader);
        shader->Delete();
    }
#endif // VTK_MAJOR_VERSION >= 7

    return pProgram;
}

//------------------------------------------------------------------------------

void openShader(const char* _pcName, std::string& _strShader)
{
    // Read data.txt
    std::string line;
    std::ifstream myfile(_pcName);
    if ( myfile.is_open() )
    {
        while ( myfile.good() )
        {
            getline( myfile, line );
            _strShader += line + "\n";
        }
        myfile.close();
    }
    else
    {
        SLM_ERROR("Unable to open file : " + std::string(_pcName));
    }
}

//------------------------------------------------------------------------------

#if VTK_MAJOR_VERSION >= 7
vtkSmartPointer<vtkShaderProgram>
#else
vtkSmartPointer<vtkShaderProgram2>
#endif
//------------------------------------------------------------------------------

buildShaderFromFile(
    vtkRenderWindow* pWindow, const char* pcVertexName, const char* pcFragmentName)
{
    SLM_ASSERT( "NULL parameter", pWindow && pcVertexName && pcFragmentName );

    vtkOpenGLRenderWindow* pOpenGLWindow = vtkOpenGLRenderWindow::SafeDownCast(pWindow);
    if(!pOpenGLWindow)
    {
        SLM_ERROR("Shader only supported using OpenGL.");
        return NULL;
    }

#if VTK_MAJOR_VERSION >= 7
    vtkSmartPointer<vtkShaderProgram> pProgram = vtkSmartPointer<vtkShaderProgram>::New();
    {
        // The vertex shader
        std::string strShader("");
        openShader(pcVertexName, strShader);

        vtkShader* shader = vtkShader::New();
        shader->SetType(vtkShader::Vertex);
        shader->SetSource(strShader.c_str());
        pProgram->SetVertexShader(shader);
        shader->Delete();
    }
    {
        // The fragment shader
        std::string strShader("");
        openShader(pcFragmentName, strShader);

        vtkShader* shader = vtkShader::New();
        shader->SetType(vtkShader::Fragment);
        shader->SetSource(strShader.c_str());
        pProgram->SetFragmentShader(shader);
        shader->Delete();
    }
#else
    vtkSmartPointer<vtkShaderProgram2> pProgram = vtkSmartPointer<vtkShaderProgram2>::New();
    {
        // The vertex shader
        std::string strShader("");
        openShader(pcVertexName, strShader);

        vtkShader2* shader = vtkShader2::New();
        shader->SetType(VTK_SHADER_TYPE_VERTEX);
        shader->SetSourceCode(strShader.c_str());
        shader->SetContext(pProgram->GetContext());
        pProgram->GetShaders()->AddItem(shader);
        shader->Delete();
    }
    {
        // The fragment shader
        std::string strShader("");
        openShader(pcFragmentName, strShader);

        vtkShader2* shader = vtkShader2::New();
        shader->SetType(VTK_SHADER_TYPE_FRAGMENT);
        shader->SetSourceCode(strShader.c_str());
        shader->SetContext(pProgram->GetContext());
        pProgram->GetShaders()->AddItem(shader);
        shader->Delete();
    }
#endif

    return pProgram;
}

} //vtk
} //fwRenderVTK
