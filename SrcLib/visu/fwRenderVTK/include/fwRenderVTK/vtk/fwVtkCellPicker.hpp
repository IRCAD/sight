/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: fwVtkCellPicker.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME fwVtkCellPicker - select a cell by shooting a ray into graphics window
// .SECTION Description
// fwVtkCellPicker is used to select a cell by shooting a ray into graphics
// window and intersecting with actor's defining geometry - specifically 
// its cells. Beside returning coordinates, actor and mapper, fwVtkCellPicker
// returns the id of the closest cell within the tolerance along the pick
// ray, and the dataset that was picked.
// .SECTION See Also
// fwVtkPicker vtkPointPicker

#ifndef __FWRENDERVTK_VTKCELLPICKER__
#define __FWRENDERVTK_VTKCELLPICKER__

#include <vector>
#include <fwData/TriangularMesh.hpp>

#include <fwMath/IntrasecTypes.hpp>

#include "fwRenderVTK/vtk/fwVtkPicker.hpp"
#include "fwRenderVTK/config.hpp"

class vtkGenericCell;
class vtkPolyData;

class FWRENDERVTK_CLASS_API fwVtkCellPicker : public fwVtkPicker
{
public:
  FWRENDERVTK_API static fwVtkCellPicker *New();
  vtkTypeRevisionMacro(fwVtkCellPicker,fwVtkPicker);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Get the id of the picked cell. If CellId = -1, nothing was picked.
  vtkGetMacro(CellId, vtkIdType);

  // Description:
  // Get the subId of the picked cell. If SubId = -1, nothing was picked.
  vtkGetMacro(SubId, int);

  // Description:
  // Get the parametric coordinates of the picked cell. Only valid if 
  // pick was made.
  vtkGetVectorMacro(PCoords, double,3);

  typedef std::vector< std::pair<int, fwVec3d> > PickedCellType;

  FWRENDERVTK_API PickedCellType GetPickedCellIds( double p1[3], double p2[3], ::fwData::TriangularMesh::sptr mesh);
  FWRENDERVTK_API PickedCellType GetPickedCellIds( double p1[3], double p2[3], vtkPolyData *polydata);

protected:
  fwVtkCellPicker();
  ~fwVtkCellPicker();

  vtkIdType CellId; // picked cell
  int SubId; // picked cell subId
  double PCoords[3]; // picked cell parametric coordinates

  virtual double IntersectWithLine(double p1[3], double p2[3], double tol, 
                                  vtkAssemblyPath *path, vtkProp3D *p, 
                                  vtkAbstractMapper3D *m);
  void Initialize();
  
private:
  vtkGenericCell *Cell; //used to accelerate picking
  
private:
  fwVtkCellPicker(const fwVtkCellPicker&);  // Not implemented.
  void operator=(const fwVtkCellPicker&);  // Not implemented.
};

#endif


