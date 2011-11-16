/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATATOOLS_MESHGENERATOR_HPP_
#define _FWDATATOOLS_MESHGENERATOR_HPP_

#include <fwCore/base.hpp>

#include <fwData/TriangularMesh.hpp>
#include <fwData/Mesh.hpp>

#include "fwDataTools/export.hpp"

namespace fwDataTools
{

class Point
{
public:

    float x;
    float y;
    float z;

    Point() : x(0.), y(0.), z(0.)
    {}
    Point(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
    {}

    bool operator<(const Point& pt) const
    {
        return (   this->x < pt.x
                ||(this->x == pt.x && this->y < pt.y)
                ||(this->x == pt.x && this->y == pt.y && this->z < pt.z) );
    }
};

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
     * @param[out]  mesh fwData::Mesh empty mesh structure to fill with cell normals.
     */
    FWDATATOOLS_API static void generateCellNormals(::fwData::Mesh::sptr mesh);

    /**
     * @brief Shake points of the mesh.
     *
     * @param[out]  mesh fwData::Mesh empty mesh structure to shake.
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
    FWDATATOOLS_API static bool hasUniqueCellType(::fwData::Mesh::sptr mesh, ::fwData::Mesh::CellTypes cell);

protected:

    FWDATATOOLS_API static ::fwData::Mesh::Id addPoint(::fwData::Mesh::PointValueType* pt, ::fwData::Mesh::sptr mesh, PointsMapType& points);

    //! @brief Constructor.
    FWDATATOOLS_API MeshGenerator();

    //! @brief Destructor.
    FWDATATOOLS_API virtual ~MeshGenerator();

};

} // namespace fwDataTools

#endif // _FWDATATOOLS_MESHGENERATOR_HPP_
