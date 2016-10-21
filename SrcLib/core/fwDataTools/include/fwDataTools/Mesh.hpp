/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATATOOLS_MESH_HPP__
#define __FWDATATOOLS_MESH_HPP__

#include "fwDataTools/Vector.hxx"
#include "fwDataTools/config.hpp"
#include <fwDataTools/helper/Mesh.hpp>

#include <fwCore/base.hpp>

#include <fwData/Mesh.hpp>
#include <fwData/TransformationMatrix3D.hpp>

namespace fwDataTools
{


/**
 * @brief   This helper generates a mesh using specified structure (quad or triangle).
 */
class Mesh
{

public:

    typedef std::map<Point, ::fwData::Mesh::Id> PointsMapType;


    /**
     * @brief Initialize 'rand' seed
     */
    FWDATATOOLS_API static void initRand();

    /**
     * @brief Generate cell normals for the mesh.
     *
     * @param[out]  mesh fwData::Mesh structure to fill with cell normals.
     */
    FWDATATOOLS_API static void generateCellNormals(::fwData::Mesh::sptr mesh);

    /**
     * @brief Generate point normals for the mesh.
     *
     * @param[out]  mesh fwData::Mesh structure to fill with cell normals.
     */
    FWDATATOOLS_API static void generatePointNormals(::fwData::Mesh::sptr mesh);

    /**
     * @brief Shake Array of Normals.
     *
     * Array must have 3 float components, 1 dimension. Otherwise, shakeNormals
     * will do nothing.
     *
     * @param[out]  array mesh's array containing normals.
     */
    FWDATATOOLS_API static void shakeNormals(::fwData::Array::sptr array);

    /**
     * @brief Shake point Normals.
     *
     * @param[out]  mesh fwData::Mesh structure to shake normals.
     */
    FWDATATOOLS_API static void shakePointNormals(::fwData::Mesh::sptr mesh);

    /**
     * @brief Shake cell Normals.
     *
     * @param[out]  mesh fwData::Mesh structure to shake normals.
     */
    FWDATATOOLS_API static void shakeCellNormals(::fwData::Mesh::sptr mesh);

    /**
     * @brief Shake points of the mesh.
     *
     * @param[out]  mesh fwData::Mesh structure to shake.
     */
    FWDATATOOLS_API static void shakePoint(::fwData::Mesh::sptr mesh);

    /**
     * @brief Colorize mesh (vertex point color).
     *
     * @param[in]  mesh fwData::Mesh mesh structure to colorize.
     */
    FWDATATOOLS_API static void colorizeMeshPoints(::fwData::Mesh::sptr mesh);

    /**
     * @brief Colorize mesh (cell color).
     *
     * @param[in]  mesh fwData::Mesh mesh structure to colorize.
     */
    FWDATATOOLS_API static void colorizeMeshCells(::fwData::Mesh::sptr mesh);

    /**
     * @brief
     *
     * @param[in]  mesh fwData::Mesh mesh structure to find cell type.
     * @param[in]  cell CellTypes to find in mesh.
     */
    FWDATATOOLS_API static bool hasUniqueCellType(::fwData::Mesh::sptr mesh, ::fwData::Mesh::CellTypes cell);

    /// Applied a transformation 4x4 on mesh
    FWDATATOOLS_API static void transform( ::fwData::Mesh::sptr mesh, ::fwData::TransformationMatrix3D::csptr t );

};

} // namespace fwDataTools

#endif // __FWDATATOOLS_MESH_HPP__
