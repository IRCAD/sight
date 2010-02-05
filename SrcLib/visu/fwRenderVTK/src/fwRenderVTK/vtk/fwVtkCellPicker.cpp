/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/*=========================================================================
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   fw version of vtkCellPicked : same a vtk5.4's cellpicker,
   but inherits from fwVtkPicker.
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 =========================================================================*/


/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: fwVtkCellPicker.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include <boost/assign.hpp>

#include <vtkGenericCell.h>
#include <vtkImageData.h>
#include <vtkMapper.h>
#include <vtkMath.h>
#include <vtkPolyData.h>
#include <vtkObjectFactory.h>
#include <vtkAbstractVolumeMapper.h>
#include <vtkInstantiator.h>

#include <vtkIO/vtk.hpp>
#include "fwRenderVTK/vtk/fwVtkCellPicker.hpp"

vtkCxxRevisionMacro(fwVtkCellPicker, "$Revision: 1.38 $");
vtkStandardNewMacro(fwVtkCellPicker);


fwVtkCellPicker::fwVtkCellPicker()
{
  this->CellId = -1;
  this->SubId = -1;
  for (int i=0; i<3; i++)
    {
    this->PCoords[i] = 0.0;
    }
  this->Cell = vtkGenericCell::New();
}

fwVtkCellPicker::~fwVtkCellPicker()
{
  this->Cell->Delete();
}

double fwVtkCellPicker::IntersectWithLine(double p1[3], double p2[3], double tol,
                                       vtkAssemblyPath *path, 
                                       vtkProp3D *prop3D, 
                                       vtkAbstractMapper3D *m)
{
  vtkIdType numCells, cellId, minCellId;
  int i, minSubId, subId;
  double x[3], tMin, t, pcoords[3], minXYZ[3], minPcoords[3];
  vtkDataSet *input;
  vtkMapper *mapper;
  vtkAbstractVolumeMapper *volumeMapper;

  // Get the underlying dataset
  if ( (mapper=vtkMapper::SafeDownCast(m)) != NULL )
    {
    input = mapper->GetInput();
    }
  else if ( (volumeMapper=vtkAbstractVolumeMapper::SafeDownCast(m)) != NULL )
    {
    input = volumeMapper->GetDataSetInput();
    }
  else
    {
    return VTK_DOUBLE_MAX;
    }

  if ( (numCells = input->GetNumberOfCells()) < 1 )
    {
    return 2.0;
    }

  // Intersect each cell with ray.  Keep track of one closest to
  // the eye (within the tolerance tol) and within the clipping range). 
  // Note that we fudge the "closest to" (tMin+this->Tolerance) a little and
  // keep track of the cell with the best pick based on parametric
  // coordinate (pick the minimum, maximum parametric distance). This 
  // breaks ties in a reasonable way when cells are the same distance 
  // from the eye (like cells lying on a 2D plane).
  //
  minCellId = -1;
  minSubId = -1;
  pcoords[0] = pcoords[1] = pcoords[2] = 0;
  double pDistMin=VTK_DOUBLE_MAX, pDist;
  for (tMin=VTK_DOUBLE_MAX,cellId=0; cellId<numCells; cellId++) 
    {
    input->GetCell(cellId, this->Cell);

    if ( this->Cell->IntersectWithLine(p1, p2, tol, t, x, pcoords, subId) 
    && t <= (tMin+this->Tolerance) )
      {
      pDist = this->Cell->GetParametricDistance(pcoords);
      if ( pDist < pDistMin || (pDist == pDistMin && t < tMin) )
        {
        minCellId = cellId;
        minSubId = subId;
        for (i=0; i<3; i++)
          {
          minXYZ[i] = x[i];
          minPcoords[i] = pcoords[i];
          }
        tMin = t;
        pDistMin = pDist;
//        cout << "cell id: " << minCellId << "\n";
        }//if minimum, maximum
      }//if a close cell
    }//for all cells
  
  //  Now compare this against other actors.
  //
  if ( minCellId>(-1) && tMin < this->GlobalTMin ) 
    {
    this->MarkPicked(path, prop3D, m, tMin, minXYZ);
    this->CellId = minCellId;
    this->SubId = minSubId;
    for (i=0; i<3; i++)
      {
      this->PCoords[i] = minPcoords[i];
      }
    vtkDebugMacro("Picked cell id= " << minCellId);
    }
  return tMin;
}

void fwVtkCellPicker::Initialize()
{
  this->CellId = (-1);
  this->SubId = (-1);
  for (int i=0; i<3; i++)
    {
    this->PCoords[i] = 0.0;
    }
  this->fwVtkPicker::Initialize();
}

void fwVtkCellPicker::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Cell Id: " << this->CellId << "\n";
  os << indent << "SubId: " << this->SubId << "\n";
  os << indent << "PCoords: (" << this->PCoords[0] << ", " 
     << this->PCoords[1] << ", " << this->PCoords[2] << ")\n";
}




fwVtkCellPicker::PickedCellType fwVtkCellPicker::GetPickedCellIds( double p1[3], double p2[3], ::fwData::TriangularMesh::sptr mesh)
{
    PickedCellType res;

    vtkPolyData *polydata = vtkIO::toVTKMesh(mesh);
    
    res = this->GetPickedCellIds(p1, p2, polydata);
    
    polydata->Delete();

    return res;
}


fwVtkCellPicker::PickedCellType fwVtkCellPicker::GetPickedCellIds( double p1[3], double p2[3], vtkPolyData *polydata)
{
    PickedCellType res;

    if ( this->PickPolyData(p1,p2, polydata) )
    {
        double *intersec = this->GetPickPosition();
        fwVec3d point;

        for(int i = 0; i<3; i++)
        {
            point[i] = intersec[i];
        }
        res.push_back( std::make_pair( this->GetCellId(), point) );


        double ray[3];
        double p3a[3];
        double p3b[3];

        this->GetPickPosition(p3a);
        this->GetPickPosition(p3b);

        for (int i = 0; i < 3; i++)
        {
            ray[i] = p2[i] - p1[i];
        }

        vtkMath::Normalize(ray);

        for (int i = 0; i < 3; i++)
        {
            ray[i] *= 1e-5;
            p3a[i] -= ray[i];
            p3b[i] += ray[i];
        }


        boost::assign::push_back(res).range( this->GetPickedCellIds(p1, p3a, polydata) );
        boost::assign::push_back(res).range( this->GetPickedCellIds(p3b, p2, polydata) );

    }

    return res;
}


