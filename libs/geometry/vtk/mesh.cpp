/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include "mesh.hpp"

#include <io/vtk/helper/Mesh.hpp>

#include <vtkCenterOfMass.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

namespace sight::geometry::vtk
{

//------------------------------------------------------------------------------

data::Point::sptr computeCenterOfMass(const data::Mesh::csptr mesh, const bool useScalarAsWeights)
{
    vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
    sight::io::vtk::helper::Mesh::toVTKMesh(mesh, polyData);

    // Compute the center of mass
    vtkSmartPointer<vtkCenterOfMass> centerOfMassFilter =
        vtkSmartPointer<vtkCenterOfMass>::New();
    centerOfMassFilter->SetInputData(polyData);
    centerOfMassFilter->SetUseScalarsAsWeights(useScalarAsWeights);
    centerOfMassFilter->Update();

    std::array<double, 3> centerOfMass {};
    centerOfMassFilter->GetCenter(centerOfMass.data());
    data::Point::sptr center = data::Point::New(centerOfMass[0], centerOfMass[1], centerOfMass[2]);
    return center;
}

//------------------------------------------------------------------------------

} // namespace sight::geometry::vtk
