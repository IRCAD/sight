/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#pragma once

#include <sight/io/vtk/config.hpp>

#include <data/mesh.hpp>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

namespace sight::io::vtk::helper
{

/**
 * @brief   Helper to manage vtkPolyData and data::mesh.
 */
class SIGHT_IO_VTK_CLASS_API mesh
{
public:

    /*!
     * @brief Convert a vtkPolyData to a data::mesh::sptr.
     *
     * @param[in] _poly_data vtkPolyData.
     * @param[out] _mesh data::mesh::sptr.
     */
    SIGHT_IO_VTK_API static void from_vtk_mesh(vtkSmartPointer<vtkPolyData> _poly_data, data::mesh::sptr _mesh);
    /*!
     * @brief Convert a vtkUnstructuredGrid to a data::mesh::sptr.
     *
     * @param[in] _grid vtkUnstructuredGrid.
     * @param[out] _mesh data::mesh::sptr.
     */

    SIGHT_IO_VTK_API static void from_vtk_grid(vtkSmartPointer<vtkUnstructuredGrid> _grid, data::mesh::sptr _mesh);

    /*!
     * @brief Convert a data::mesh::csptr to a vtkUnstructuredGrid.
     *
     * @param[in] _mesh data::mesh::csptr.
     * @param[out] _grid vtkUnstructuredGrid.
     */
    SIGHT_IO_VTK_API static void to_vtk_grid(
        const data::mesh::csptr& _mesh,
        vtkSmartPointer<vtkUnstructuredGrid> _grid
    );

    /*!
     * @brief Convert a data::mesh::csptr to a vtkPolyData.
     *
     * @param[in] _mesh data::mesh::csptr.
     * @param[out] _poly_data vtkPolyData.
     */
    SIGHT_IO_VTK_API static void to_vtk_mesh(const data::mesh::csptr& _mesh, vtkSmartPointer<vtkPolyData> _poly_data);

    /*!
     * @brief Compute the volume of the mesh using MassProperties vtk class
     * @param[in] _mesh current mesh
     * @return volume of the mesh
     */
    SIGHT_IO_VTK_API static double compute_volume(const data::mesh::csptr& _mesh);
};

} // namespace sight::io::vtk::helper
