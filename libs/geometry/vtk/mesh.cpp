/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include <io/vtk/helper/mesh.hpp>

#include <vtkCenterOfMass.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

namespace sight::geometry::vtk
{

//------------------------------------------------------------------------------

data::point::sptr compute_center_of_mass(const data::mesh::csptr _mesh, const bool _use_scalar_as_weights)
{
    vtkSmartPointer<vtkPolyData> poly_data = vtkSmartPointer<vtkPolyData>::New();
    sight::io::vtk::helper::mesh::to_vtk_mesh(_mesh, poly_data);

    // Compute the center of mass
    vtkSmartPointer<vtkCenterOfMass> center_of_mass_filter =
        vtkSmartPointer<vtkCenterOfMass>::New();
    center_of_mass_filter->SetInputData(poly_data);
    center_of_mass_filter->SetUseScalarsAsWeights(_use_scalar_as_weights);
    center_of_mass_filter->Update();

    std::array<double, 3> center_of_mass {};
    center_of_mass_filter->GetCenter(center_of_mass.data());
    data::point::sptr center = std::make_shared<data::point>(center_of_mass[0], center_of_mass[1], center_of_mass[2]);
    return center;
}

//------------------------------------------------------------------------------

} // namespace sight::geometry::vtk
