/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwVtkIO/config.hpp"

#include <fwData/Mesh.hpp>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkUnstructuredGrid.h>

namespace fwVtkIO
{

namespace helper
{

/**
 * @brief   Helper to manage vtkPolyData and fwData::Mesh.
 */
class FWVTKIO_CLASS_API Mesh
{

public:

    /*!
     * @brief Convert a vtkPolyData to a ::fwData::Mesh::sptr.
     *
     * @param[in] _polyData vtkPolyData.
     * @param[out] _mesh ::fwData::Mesh::sptr.
     */
    FWVTKIO_API static void fromVTKMesh(  vtkSmartPointer<vtkPolyData> _polyData, ::fwData::Mesh::sptr _mesh );
    /*!
     * @brief Convert a vtkUnstructuredGrid to a ::fwData::Mesh::sptr.
     *
     * @param[in] grid vtkUnstructuredGrid.
     * @param[out] mesh ::fwData::Mesh::sptr.
     */

    FWVTKIO_API static void fromVTKGrid(vtkSmartPointer<vtkUnstructuredGrid> grid, ::fwData::Mesh::sptr mesh);

    /*!
     * @brief Convert a ::fwData::Mesh::sptr to a vtkUnstructuredGrid.
     *
     * @param[in] mesh ::fwData::Mesh::sptr.
     * @param[out] grid vtkUnstructuredGrid.
     */
    FWVTKIO_API static void toVTKGrid( const ::fwData::Mesh::csptr& mesh, vtkSmartPointer<vtkUnstructuredGrid> grid);

    /*!
     * @brief Convert a ::fwData::Mesh::sptr to a vtkPolyData.
     *
     * @param[in] _mesh ::fwData::Mesh::sptr.
     * @param[out] _polyData vtkPolyData.
     */
    FWVTKIO_API static void toVTKMesh( const ::fwData::Mesh::csptr& _mesh, vtkSmartPointer<vtkPolyData> _polyData);

    /*!
     * @brief Update a vtkPolyData with ::fwData::Mesh::sptr points
     *
     * @param[out] polyDataDst vtkPolyData
     * @param[in]  meshSrc ::fwData::Mesh::sptr
     * @return vtkPolyData
     *
     * Warning : be carefull with updatePoints : this may change the number of
     * vertex of the polydata, cells will not be updated.
     * Returns the updated vtkPolyPata
     */
    FWVTKIO_API static vtkSmartPointer<vtkPolyData>  updatePolyDataPoints(vtkSmartPointer<vtkPolyData> polyDataDst,
                                                                          const fwData::Mesh::csptr& meshSrc );

    /*!
     * @brief Update a vtkPolyData with point color of fwData::Mesh
     *
     * @param[out] polyDataDst vtkPolyData
     * @param[in]  meshSrc ::fwData::Mesh::sptr
     * @return vtkPolyData
     *
     * Returns the updated vtkPolyPata
     */
    FWVTKIO_API static vtkSmartPointer<vtkPolyData> updatePolyDataPointColor(vtkSmartPointer<vtkPolyData> polyDataDst,
                                                                             const ::fwData::Mesh::csptr& meshSrc);

    /*!
     * @brief Update a vtkPolyData with cell color of fwData::Mesh
     *
     * @param[out] polyDataDst vtkPolyData
     * @param[in]  meshSrc ::fwData::Mesh::sptr
     * @return vtkPolyData
     *
     * Returns the updated vtkPolyPata
     */
    FWVTKIO_API static vtkSmartPointer<vtkPolyData> updatePolyDataCellColor(vtkSmartPointer<vtkPolyData> polyDataDst,
                                                                            const ::fwData::Mesh::csptr& meshSrc );

    /*!
     * @brief Update a vtkPolyData with point normals of fwData::Mesh
     *
     * @param[out] polyDataDst vtkPolyData
     * @param[in]  meshSrc ::fwData::Mesh::sptr
     * @return vtkPolyData
     *
     * Returns the updated vtkPolyPata
     */
    FWVTKIO_API static vtkSmartPointer<vtkPolyData> updatePolyDataPointNormals(vtkSmartPointer<vtkPolyData> polyDataDst,
                                                                               const ::fwData::Mesh::csptr& meshSrc);

    /*!
     * @brief Update a vtkPolyData with cell normals of fwData::Mesh
     *
     * @param[out] polyDataDst vtkPolyData
     * @param[in]  meshSrc ::fwData::Mesh::sptr
     * @return vtkPolyData
     *
     * Returns the updated vtkPolyPata
     */
    FWVTKIO_API static vtkSmartPointer<vtkPolyData> updatePolyDataCellNormals(vtkSmartPointer<vtkPolyData> polyDataDst,
                                                                              const ::fwData::Mesh::csptr& meshSrc);

    /*!
     * @brief Update a vtkPolyData with point texCoords of fwData::Mesh
     *
     * @param[out] polyDataDst vtkPolyData
     * @param[in]  meshSrc ::fwData::Mesh::sptr
     * @return the updated vtkPolyPata
     */
    FWVTKIO_API static vtkSmartPointer<vtkPolyData> updatePolyDataPointTexCoords(
        vtkSmartPointer<vtkPolyData> polyDataDst, const ::fwData::Mesh::csptr& meshSrc);

    /*!
     * @brief Update a vtkPolyData with cell texCoords of fwData::Mesh
     *
     * @param[out] polyDataDst vtkPolyData
     * @param[in]  meshSrc ::fwData::Mesh::sptr
     * @return the updated vtkPolyPata
     */
    FWVTKIO_API static vtkSmartPointer<vtkPolyData> updatePolyDataCellTexCoords(
        vtkSmartPointer<vtkPolyData> polyDataDst, const ::fwData::Mesh::csptr& meshSrc);

    /*!
     * @brief Update a vtkUnstructuredGrid with ::fwData::Mesh::sptr points
     *
     * @param[out] gridDataDst vtkUnstructuredGrid
     * @param[in]  meshSrc ::fwData::Mesh::sptr
     * @return vtkUnstructuredGrid
     *
     * Warning : be carefull with updatePoints : this may change the number of
     * vertex of the vtkUnstructuredGrid, cells will not be updated.
     * Returns the updated vtkUnstructuredGrid
     */
    FWVTKIO_API static vtkSmartPointer<vtkUnstructuredGrid>  updateGridPoints(
        vtkSmartPointer<vtkUnstructuredGrid> gridDst,
        const fwData::Mesh::csptr& meshSrc );

    /*!
     * @brief Update a vtkUnstructuredGrid with point color of fwData::Mesh
     *
     * @param[out] gridDataDst vtkUnstructuredGrid
     * @param[in]  meshSrc ::fwData::Mesh::sptr
     * @return vtkUnstructuredGrid
     *
     * Returns the updated vtkUnstructuredGrid
     */
    FWVTKIO_API static vtkSmartPointer<vtkUnstructuredGrid> updateGridPointColor(
        vtkSmartPointer<vtkUnstructuredGrid> gridDst,
        const ::fwData::Mesh::csptr& meshSrc);

    /*!
     * @brief Update a vtkUnstructuredGrid with cell color of fwData::Mesh
     *
     * @param[out] gridDataDst vtkUnstructuredGrid
     * @param[in]  meshSrc ::fwData::Mesh::sptr
     * @return vtkUnstructuredGrid
     *
     * Returns the updated vtkUnstructuredGrid
     */
    FWVTKIO_API static vtkSmartPointer<vtkUnstructuredGrid> updateGridCellColor(
        vtkSmartPointer<vtkUnstructuredGrid> gridDst,
        const ::fwData::Mesh::csptr& meshSrc );

    /*!
     * @brief Update a vtkUnstructuredGrid with point normals of fwData::Mesh
     *
     * @param[out] gridDataDst vtkUnstructuredGrid
     * @param[in]  meshSrc ::fwData::Mesh::sptr
     * @return vtkUnstructuredGrid
     *
     * Returns the updated vtkUnstructuredGrid
     */
    FWVTKIO_API static vtkSmartPointer<vtkUnstructuredGrid> updateGridPointNormals(
        vtkSmartPointer<vtkUnstructuredGrid> gridDst,
        const ::fwData::Mesh::csptr& meshSrc);

    /*!
     * @brief Update a vtkUnstructuredGrid with cell normals of fwData::Mesh
     *
     * @param[out] gridDataDst vtkUnstructuredGrid
     * @param[in]  meshSrc ::fwData::Mesh::sptr
     * @return vtkUnstructuredGrid
     *
     * Returns the updated vtkUnstructuredGrid
     */
    FWVTKIO_API static vtkSmartPointer<vtkUnstructuredGrid> updateGridCellNormals(
        vtkSmartPointer<vtkUnstructuredGrid> gridDst,
        const ::fwData::Mesh::csptr& meshSrc);

    /*!
     * @brief Update a vtkUnstructuredGrid with point texCoords of fwData::Mesh
     *
     * @param[out] gridDataDst vtkUnstructuredGrid
     * @param[in]  meshSrc ::fwData::Mesh::sptr
     * @return vtkUnstructuredGrid
     *
     * Returns the updated vtkUnstructuredGrid
     */
    FWVTKIO_API static vtkSmartPointer<vtkUnstructuredGrid> updateGridPointTexCoords(
        vtkSmartPointer<vtkUnstructuredGrid> gridDst, const ::fwData::Mesh::csptr& meshSrc);

    /*!
     * @brief Update a vtkUnstructuredGrid with cell texCoords of fwData::Mesh
     *
     * @param[out] gridDataDst vtkUnstructuredGrid
     * @param[in]  meshSrc ::fwData::Mesh::sptr
     * @return vtkUnstructuredGrid
     *
     * Returns the updated vtkUnstructuredGrid
     */
    FWVTKIO_API static vtkSmartPointer<vtkUnstructuredGrid> updateGridCellTexCoords(
        vtkSmartPointer<vtkUnstructuredGrid> gridDst, const ::fwData::Mesh::csptr& meshSrc);

    /*!
     * @brief Compute the volume of the mesh using MassProperties vtk class
     * @param[in] mesh current mesh
     * @return volume of the mesh
     */
    FWVTKIO_API static double computeVolume( const ::fwData::Mesh::csptr& mesh );

};

} // namespace helper
} // namespace fwVtkIO
