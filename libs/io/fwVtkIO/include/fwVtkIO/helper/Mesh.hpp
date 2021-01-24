/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "fwVtkIO/config.hpp"

#include <data/Mesh.hpp>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

namespace fwVtkIO
{

namespace helper
{

/**
 * @brief   Helper to manage vtkPolyData and data::Mesh.
 */
class FWVTKIO_CLASS_API Mesh
{

public:

    /*!
     * @brief Convert a vtkPolyData to a data::Mesh::sptr.
     *
     * @param[in] _polyData vtkPolyData.
     * @param[out] _mesh data::Mesh::sptr.
     */
    FWVTKIO_API static void fromVTKMesh(  vtkSmartPointer<vtkPolyData> _polyData, data::Mesh::sptr _mesh );
    /*!
     * @brief Convert a vtkUnstructuredGrid to a data::Mesh::sptr.
     *
     * @param[in] grid vtkUnstructuredGrid.
     * @param[out] mesh data::Mesh::sptr.
     */

    FWVTKIO_API static void fromVTKGrid(vtkSmartPointer<vtkUnstructuredGrid> grid, data::Mesh::sptr mesh);

    /*!
     * @brief Convert a data::Mesh::csptr to a vtkUnstructuredGrid.
     *
     * @param[in] mesh data::Mesh::csptr.
     * @param[out] grid vtkUnstructuredGrid.
     */
    FWVTKIO_API static void toVTKGrid( const data::Mesh::csptr& mesh, vtkSmartPointer<vtkUnstructuredGrid> grid);

    /*!
     * @brief Convert a data::Mesh::csptr to a vtkPolyData.
     *
     * @param[in] _mesh data::Mesh::csptr.
     * @param[out] _polyData vtkPolyData.
     */
    FWVTKIO_API static void toVTKMesh( const data::Mesh::csptr& _mesh, vtkSmartPointer<vtkPolyData> _polyData);

    /*!
     * @brief Update a vtkPolyData with data::Mesh::sptr points positions, colors, normals and/or textures
     *
     * @param[out] polyDataDst vtkPolyData
     * @param[in]  meshSrc data::Mesh::sptr
     * @return vtkPolyData
     *
     * Warning : be careful with updatePoints : this may change the number of
     * vertex of the polydata, cells will not be updated.
     * Returns the updated vtkPolyPata
     */
    FWVTKIO_API static void  updatePolyDataPointsAndAttributes(vtkSmartPointer<vtkPolyData> polyDataDst,
                                                               const data::Mesh::csptr& meshSrc );

    /*!
     * @brief Update a vtkPolyData with data::Mesh::sptr points
     *
     * @param[out] polyDataDst vtkPolyData
     * @param[in]  meshSrc data::Mesh::sptr
     * @return vtkPolyData
     *
     * Warning : be careful with updatePoints : this may change the number of
     * vertex of the polydata, cells will not be updated.
     * Returns the updated vtkPolyPata
     */
    FWVTKIO_API static void  updatePolyDataPoints(vtkSmartPointer<vtkPolyData> polyDataDst,
                                                  const data::Mesh::csptr& meshSrc );

    /*!
     * @brief Update a vtkPolyData with point color of data::Mesh
     *
     * @param[out] polyDataDst vtkPolyData
     * @param[in]  meshSrc data::Mesh::sptr
     * @return vtkPolyData
     *
     * Returns the updated vtkPolyPata
     */
    FWVTKIO_API static void updatePolyDataPointColor(vtkSmartPointer<vtkPolyData> polyDataDst,
                                                     const data::Mesh::csptr& meshSrc);

    /*!
     * @brief Update a vtkPolyData with cell color of data::Mesh
     *
     * @param[out] polyDataDst vtkPolyData
     * @param[in]  meshSrc data::Mesh::sptr
     * @return vtkPolyData
     *
     * Returns the updated vtkPolyPata
     */
    FWVTKIO_API static void updatePolyDataCellColor(vtkSmartPointer<vtkPolyData> polyDataDst,
                                                    const data::Mesh::csptr& meshSrc );

    /*!
     * @brief Update a vtkPolyData with point normals of data::Mesh
     *
     * @param[out] polyDataDst vtkPolyData
     * @param[in]  meshSrc data::Mesh::sptr
     * @return vtkPolyData
     *
     * Returns the updated vtkPolyPata
     */
    FWVTKIO_API static void updatePolyDataPointNormals(vtkSmartPointer<vtkPolyData> polyDataDst,
                                                       const data::Mesh::csptr& meshSrc);

    /*!
     * @brief Update a vtkPolyData with cell normals of data::Mesh
     *
     * @param[out] polyDataDst vtkPolyData
     * @param[in]  meshSrc data::Mesh::sptr
     * @return vtkPolyData
     *
     * Returns the updated vtkPolyPata
     */
    FWVTKIO_API static void updatePolyDataCellNormals(vtkSmartPointer<vtkPolyData> polyDataDst,
                                                      const data::Mesh::csptr& meshSrc);

    /*!
     * @brief Update a vtkPolyData with point texCoords of data::Mesh
     *
     * @param[out] polyDataDst vtkPolyData
     * @param[in]  meshSrc data::Mesh::sptr
     * @return the updated vtkPolyPata
     */
    FWVTKIO_API static void updatePolyDataPointTexCoords(
        vtkSmartPointer<vtkPolyData> polyDataDst, const data::Mesh::csptr& meshSrc);

    /*!
     * @brief Update a vtkPolyData with cell texCoords of data::Mesh
     *
     * @param[out] polyDataDst vtkPolyData
     * @param[in]  meshSrc data::Mesh::sptr
     * @return the updated vtkPolyPata
     */
    FWVTKIO_API static void updatePolyDataCellTexCoords(
        vtkSmartPointer<vtkPolyData> polyDataDst, const data::Mesh::csptr& meshSrc);

    /*!
     * @brief Update a vtkUnstructuredGrid with data::Mesh::sptr points positions, colors, normals and/or textures
     *
     * @param[out] gridDataDst vtkUnstructuredGrid
     * @param[in]  meshSrc data::Mesh::sptr
     * @return vtkUnstructuredGrid
     *
     * Warning : be careful with updatePoints : this may change the number of
     * vertex of the vtkUnstructuredGrid, cells will not be updated.
     * Returns the updated vtkUnstructuredGrid
     */
    FWVTKIO_API static void updateGridPointsAndAttributes(vtkSmartPointer<vtkUnstructuredGrid> gridDst,
                                                          const data::Mesh::csptr& meshSrc );

    /*!
     * @brief Update a vtkUnstructuredGrid with data::Mesh::sptr points
     *
     * @param[out] gridDataDst vtkUnstructuredGrid
     * @param[in]  meshSrc data::Mesh::sptr
     * @return vtkUnstructuredGrid
     *
     * Warning : be careful with updatePoints : this may change the number of
     * vertex of the vtkUnstructuredGrid, cells will not be updated.
     * Returns the updated vtkUnstructuredGrid
     */
    FWVTKIO_API static void updateGridPoints(
        vtkSmartPointer<vtkUnstructuredGrid> gridDst,
        const data::Mesh::csptr& meshSrc );

    /*!
     * @brief Update a vtkUnstructuredGrid with point color of data::Mesh
     *
     * @param[out] gridDataDst vtkUnstructuredGrid
     * @param[in]  meshSrc data::Mesh::sptr
     * @return vtkUnstructuredGrid
     *
     * Returns the updated vtkUnstructuredGrid
     */
    FWVTKIO_API static void updateGridPointColor(
        vtkSmartPointer<vtkUnstructuredGrid> gridDst,
        const data::Mesh::csptr& meshSrc);

    /*!
     * @brief Update a vtkUnstructuredGrid with cell color of data::Mesh
     *
     * @param[out] gridDataDst vtkUnstructuredGrid
     * @param[in]  meshSrc data::Mesh::sptr
     * @return vtkUnstructuredGrid
     *
     * Returns the updated vtkUnstructuredGrid
     */
    FWVTKIO_API static void updateGridCellColor(
        vtkSmartPointer<vtkUnstructuredGrid> gridDst,
        const data::Mesh::csptr& meshSrc );

    /*!
     * @brief Update a vtkUnstructuredGrid with point normals of data::Mesh
     *
     * @param[out] gridDataDst vtkUnstructuredGrid
     * @param[in]  meshSrc data::Mesh::sptr
     * @return vtkUnstructuredGrid
     *
     * Returns the updated vtkUnstructuredGrid
     */
    FWVTKIO_API static void updateGridPointNormals(
        vtkSmartPointer<vtkUnstructuredGrid> gridDst,
        const data::Mesh::csptr& meshSrc);

    /*!
     * @brief Update a vtkUnstructuredGrid with cell normals of data::Mesh
     *
     * @param[out] gridDataDst vtkUnstructuredGrid
     * @param[in]  meshSrc data::Mesh::sptr
     * @return vtkUnstructuredGrid
     *
     * Returns the updated vtkUnstructuredGrid
     */
    FWVTKIO_API static void updateGridCellNormals(
        vtkSmartPointer<vtkUnstructuredGrid> gridDst,
        const data::Mesh::csptr& meshSrc);

    /*!
     * @brief Update a vtkUnstructuredGrid with point texCoords of data::Mesh
     *
     * @param[out] gridDataDst vtkUnstructuredGrid
     * @param[in]  meshSrc data::Mesh::sptr
     * @return vtkUnstructuredGrid
     *
     * Returns the updated vtkUnstructuredGrid
     */
    FWVTKIO_API static void updateGridPointTexCoords(
        vtkSmartPointer<vtkUnstructuredGrid> gridDst, const data::Mesh::csptr& meshSrc);

    /*!
     * @brief Update a vtkUnstructuredGrid with cell texCoords of data::Mesh
     *
     * @param[out] gridDataDst vtkUnstructuredGrid
     * @param[in]  meshSrc data::Mesh::sptr
     * @return vtkUnstructuredGrid
     *
     * Returns the updated vtkUnstructuredGrid
     */
    FWVTKIO_API static void updateGridCellTexCoords(
        vtkSmartPointer<vtkUnstructuredGrid> gridDst, const data::Mesh::csptr& meshSrc);

    /*!
     * @brief Compute the volume of the mesh using MassProperties vtk class
     * @param[in] mesh current mesh
     * @return volume of the mesh
     */
    FWVTKIO_API static double computeVolume( const data::Mesh::csptr& mesh );

};

} // namespace helper
} // namespace fwVtkIO
