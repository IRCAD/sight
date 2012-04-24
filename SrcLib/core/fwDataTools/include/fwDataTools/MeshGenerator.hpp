/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATATOOLS_MESHGENERATOR_HPP_
#define _FWDATATOOLS_MESHGENERATOR_HPP_

#include <fwCore/base.hpp>

#include <fwData/TriangularMesh.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/Mesh.hpp>

#include "fwDataTools/Vector.hxx"
#include "fwDataTools/export.hpp"

namespace fwDataTools
{


/**
 * @brief   This helper generates a mesh using specified structure (quad or triangle).
 * @class   MeshGenerator.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 *
 */
class MeshGenerator : public ::fwCore::BaseObject
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((MeshGenerator)( ::fwCore::BaseObject ),
                                           (()),
                                           new MeshGenerator
                                          );
    fwCoreAllowSharedFromThis();

    typedef std::map<Point, ::fwData::Mesh::Id> PointsMapType;


    /**
     * @brief Initialize 'rand' seed
     */
    FWDATATOOLS_API static void initRand();

    /**
     * @brief Add quad cells in mesh, this method generates synthetic data (two face of a cube).
     *
     * @param[out]  mesh fwData::Mesh Mesh structure to fill with quad cells.
     * @param[in]   points PointsMapType Point registration map used to not create point which already in mesh buffer.
     * @param[in]   nbPointsByEdge size_t Number of points by edge.
     * @param[in]   edgeDim float Edge dimension in 3D world.
     */
    FWDATATOOLS_API static void addQuadMesh(::fwData::Mesh::sptr mesh, PointsMapType& points, size_t nbPointsByEdge = 10, float edgeDim = 100.);

    /**
     * @brief Add triangle cells in mesh, this method generates synthetic data (two face of a cube).
     *
     * @param[out]  mesh fwData::Mesh Mesh structure to fill with triangle cells.
     * @param[in]   points PointsMapType Point registration map used to not create point which already in mesh buffer.
     * @param[in]   nbPointsByEdge size_t Number of points by edge.
     * @param[in]   edgeDim float Edge dimension in 3D world.
     */
    FWDATATOOLS_API static void addTriangleMesh(::fwData::Mesh::sptr mesh, PointsMapType& points, size_t nbPointsByEdge = 10, float edgeDim = 100.);

    /**
     * @brief Generate a quad mesh.
     * @see addQuadMesh
     * @param[out]  mesh fwData::Mesh empty mesh structure to fill with quad cells.
     */
    FWDATATOOLS_API static void generateQuadMesh(::fwData::Mesh::sptr mesh);

    /**
     * @brief Generate a triangle mesh.
     * @see addTriangleMesh
     * @param[out]  mesh fwData::Mesh empty mesh structure to fill with triangle cell.
     */
    FWDATATOOLS_API static void generateTriangleMesh(::fwData::Mesh::sptr mesh);

    /**
     * @brief Generate a mesh with quad and triangle cells.
     * @see addQuadMesh
     * @see addTriangleMesh
     * @param[out]  mesh fwData::Mesh empty mesh structure to fill with quad and triangle cells.
     */
    FWDATATOOLS_API static void generateTriangleQuadMesh(::fwData::Mesh::sptr mesh);


    /**
     * @brief Generate cell normals for the mesh.
     *
     * @param[out]  mesh fwData::Mesh structure to fill with cell normals.
     */
    FWDATATOOLS_API static void generateCellNormals(::fwData::Mesh::sptr mesh);

    /**
     * @brief Generate cell normals for the mesh.
     *
     * @param[out]  mesh fwData::Mesh structure to fill with cell normals.
     */
    FWDATATOOLS_API static void generatePointNormals(::fwData::Mesh::sptr mesh);

    /**
     * @brief Shake Array of Normals.
     *
     * Array must have 3 float components, 1 dimention. Otherwise, shakeNormals
     * will do nothing.
     *
     * @param[out]  mesh fwData::Array structure to shake normals.
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
     * @brief Convert fwData::Mesh to fwData::TriangularMesh structure.
     *
     * @param[in]   mesh fwData::Mesh mesh to convert.
     * @param[out]  trian fwData::TriangularMesh destination for the converted mesh.
     *
     * @pre Mesh must contains only triangular cells.
     */
    FWDATATOOLS_API static void toTriangularMesh(::fwData::Mesh::sptr mesh, ::fwData::TriangularMesh::sptr trian);

    /**
     * @brief Convert fwData::TriangularMesh to fwData::Mesh structure.
     *
     * @param[in]  trian fwData::TriangularMesh triangular mesh to convert.
     * @param[out] mesh fwData::Mesh destination for the converted triangular mesh.
     */
    FWDATATOOLS_API static void fromTriangularMesh(::fwData::TriangularMesh::sptr trian, ::fwData::Mesh::sptr mesh);

    /**
     * @brief
     *
     * @param[in]  mesh fwData::Mesh mesh structure to find cell type.
     * @param[in]  cell CellTypes to find in mesh.
     */
    FWDATATOOLS_API static bool hasUniqueCellType(::fwData::Mesh::csptr mesh, ::fwData::Mesh::CellTypes cell);

    /// Applied a transformation 4x4 on mesh
    FWDATATOOLS_API static void transform( ::fwData::Mesh::sptr mesh, ::fwData::TransformationMatrix3D::sptr t );

protected:

    FWDATATOOLS_API static ::fwData::Mesh::Id addPoint(::fwData::Mesh::PointValueType* pt, ::fwData::Mesh::sptr mesh, PointsMapType& points);

    //! @brief Constructor.
    FWDATATOOLS_API MeshGenerator();

    //! @brief Destructor.
    FWDATATOOLS_API virtual ~MeshGenerator();

};

} // namespace fwDataTools

#endif // _FWDATATOOLS_MESHGENERATOR_HPP_
