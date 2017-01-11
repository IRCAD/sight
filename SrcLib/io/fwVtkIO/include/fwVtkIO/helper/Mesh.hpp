/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWVTKIO_HELPER_MESH_HPP__
#define __FWVTKIO_HELPER_MESH_HPP__

#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>

#include <fwData/Mesh.hpp>

#include "fwVtkIO/config.hpp"

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
                                                                          const fwData::Mesh::csptr &meshSrc );

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
     * @brief Compute the volume of the mesh using MassProperties vtk class
     * @param[in] mesh current mesh
     * @return volume of the mesh
     */
    FWVTKIO_API static double computeVolume( const ::fwData::Mesh::csptr& mesh );

};

} // namespace helper
} // namespace fwVtkIO

#endif // __FWVTKIO_HELPER_MESH_HPP__
