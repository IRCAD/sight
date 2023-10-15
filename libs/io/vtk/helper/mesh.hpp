/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/vtk/config.hpp"

#include <data/mesh.hpp>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

namespace sight::io::vtk::helper
{

/**
 * @brief   Helper to manage vtkPolyData and data::mesh.
 */
class IO_VTK_CLASS_API mesh
{
public:

    /*!
     * @brief Convert a vtkPolyData to a data::mesh::sptr.
     *
     * @param[in] _polyData vtkPolyData.
     * @param[out] _mesh data::mesh::sptr.
     */
    IO_VTK_API static void fromVTKMesh(vtkSmartPointer<vtkPolyData> _polyData, data::mesh::sptr _mesh);
    /*!
     * @brief Convert a vtkUnstructuredGrid to a data::mesh::sptr.
     *
     * @param[in] grid vtkUnstructuredGrid.
     * @param[out] mesh data::mesh::sptr.
     */

    IO_VTK_API static void fromVTKGrid(vtkSmartPointer<vtkUnstructuredGrid> grid, data::mesh::sptr mesh);

    /*!
     * @brief Convert a data::mesh::csptr to a vtkUnstructuredGrid.
     *
     * @param[in] mesh data::mesh::csptr.
     * @param[out] grid vtkUnstructuredGrid.
     */
    IO_VTK_API static void toVTKGrid(const data::mesh::csptr& mesh, vtkSmartPointer<vtkUnstructuredGrid> grid);

    /*!
     * @brief Convert a data::mesh::csptr to a vtkPolyData.
     *
     * @param[in] _mesh data::mesh::csptr.
     * @param[out] _polyData vtkPolyData.
     */
    IO_VTK_API static void toVTKMesh(const data::mesh::csptr& _mesh, vtkSmartPointer<vtkPolyData> _polyData);

    /*!
     * @brief Compute the volume of the mesh using MassProperties vtk class
     * @param[in] mesh current mesh
     * @return volume of the mesh
     */
    IO_VTK_API static double computeVolume(const data::mesh::csptr& mesh);
};

} // namespace sight::io::vtk::helper
