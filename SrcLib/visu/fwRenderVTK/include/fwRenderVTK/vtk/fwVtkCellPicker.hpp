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
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 *   Program:   Visualization Toolkit
 *   Module:    $RCSfile: fwVtkCellPicker.h,v $
 *
 *   Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 *   All rights reserved.
 *   See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 *
 *   This software is distributed WITHOUT ANY WARRANTY; without even
 *   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *   PURPOSE.  See the above copyright notice for more information.
 *
 ***********************************************************************/
// .NAME fwVtkCellPicker - select a cell by shooting a ray into graphics window
// .SECTION Description
// fwVtkCellPicker is used to select a cell by shooting a ray into graphics
// window and intersecting with actor's defining geometry - specifically
// its cells. Beside returning coordinates, actor and mapper, fwVtkCellPicker
// returns the id of the closest cell within the tolerance along the pick
// ray, and the dataset that was picked.
// .SECTION See Also
// fwVtkPicker vtkPointPicker

#pragma once

#include "fwRenderVTK/config.hpp"
#include "fwRenderVTK/vtk/fwVtkPicker.hpp"

#include <fwMath/IntrasecTypes.hpp>

#include <vector>

class vtkGenericCell;
class vtkPolyData;

namespace fwRenderVTK
{

namespace vtk
{

class FWRENDERVTK_CLASS_API fwVtkCellPicker : public fwVtkPicker
{
public:
    FWRENDERVTK_API static fwVtkCellPicker* New();
    vtkTypeMacro(fwVtkCellPicker, fwVtkPicker);
    void PrintSelf(ostream& os, vtkIndent indent) override;

    // Description:
    // Get the id of the picked cell. If CellId = -1, nothing was picked.
    vtkGetMacro(CellId, vtkIdType);

    // Description:
    // Get the subId of the picked cell. If SubId = -1, nothing was picked.
    vtkGetMacro(SubId, int);

    // Description:
    // Get the parametric coordinates of the picked cell. Only valid if
    // pick was made.
    vtkGetVectorMacro(PCoords, double, 3);

    typedef std::vector< std::pair<int, fwVec3d> > PickedCellType;

    FWRENDERVTK_API PickedCellType GetPickedCellIds( double p1[3], double p2[3], vtkPolyData* polydata);

protected:
    fwVtkCellPicker();
    ~fwVtkCellPicker();

    vtkIdType CellId; // picked cell
    int SubId; // picked cell subId
    double PCoords[3]; // picked cell parametric coordinates

    virtual double IntersectWithLine(const double p1[3], const double p2[3], double tol,
                                     vtkAssemblyPath* path, vtkProp3D* p,
                                     vtkAbstractMapper3D* m) override;
    void Initialize() override;

private:
    vtkGenericCell* Cell; //used to accelerate picking

private:
    fwVtkCellPicker(const fwVtkCellPicker&);  // Not implemented.
    void operator=(const fwVtkCellPicker&);  // Not implemented.
};

} // namespace vtk

} // namespace fwRenderVTK
