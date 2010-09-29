/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <vtkMath.h>
#include <vtkPoints.h>
#include <vtkPicker.h>
#include <vtkAssemblyNode.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkAssemblyPath.h>
#include <vtkProp3DCollection.h>


#include <fwCore/base.hpp>
#include "fwRenderVTK/vtk/Helpers.hpp"

namespace fwRenderVTK
{

namespace vtk
{

//------------------------------------------------------------------------------

vtkIdType getNearestPointId(vtkPoints* pts, vtkRenderer* renderer)
{
    vtkIdType id=-1;
    double camPosition[3];
    double distance = VTK_DOUBLE_MAX;
    renderer->GetActiveCamera()->GetPosition(camPosition);

    for(vtkIdType i = 0; i<pts->GetNumberOfPoints (); i++)
    {
        double *point = pts->GetPoint(i);
        double distancePtCam = vtkMath::Distance2BetweenPoints(point, camPosition);

        if(distancePtCam < distance)
        {
            id = i;
            distance = distancePtCam;
        }
    }

    return id;
}

//------------------------------------------------------------------------------

vtkProp * getNearestPickedProp(vtkAbstractPropPicker *picker, vtkRenderer *renderer)
{
    vtkProp   *res       = NULL;
    vtkPicker *vtkpicker = vtkPicker::SafeDownCast(picker);
    vtkIdType  id        = -1 ;

    SLM_ASSERT("getNearestPickedProp *need* a picker.", picker);
    SLM_ASSERT("getNearestPickedProp *need* a renderer.", renderer);

    if (vtkpicker)
    {
        id = getNearestPointId(vtkpicker->GetPickedPositions(), renderer);

        if (id>-1 && vtkpicker->GetProp3Ds()->GetNumberOfItems() > id)
        {
//          vtkIdType i = id;
//          vtkAssemblyPath *path = picker->GetPath();
//          vtkAssemblyNode *node;
//          path->InitTraversal();
//          for ( path->InitTraversal(); (node=path->GetNextNode()); )
//          {
//              if (--i == 0)
//              {
//                  res = node->GetViewProp();
//                  break;
//              }
//          }
//          vtkAssemblyNode *node = vtkAssemblyNode::SafeDownCast(picker->GetPath()->GetItemAsObject(id));
//          SLM_ASSERT("vtkAssemblyNode error: not found", node);
//          res = node->GetViewProp();
//          SLM_ASSERT("No vtkProp found in picker's path", res);

            res = vtkProp::SafeDownCast(vtkpicker->GetProp3Ds()->GetItemAsObject(id));
        }
    }
    else
    {
        res = picker->GetProp3D();
    }
    return res;
}

//------------------------------------------------------------------------------

bool getNearestPickedPosition(vtkAbstractPropPicker *picker, vtkRenderer *renderer, double position[3])
{
    bool res = false;
    vtkPicker *vtkpicker = vtkPicker::SafeDownCast(picker);
    vtkIdType  id        = -1 ;

    SLM_ASSERT("getNearestPickedProp *need* a picker.", picker);
    SLM_ASSERT("getNearestPickedProp *need* a renderer.", renderer);

    double *point = NULL;
    if (vtkpicker)
    {
        vtkPoints *pts=vtkpicker->GetPickedPositions();
        id = getNearestPointId(pts, renderer);

        if (id>-1)
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

} //vtk

} //fwRenderVTK



