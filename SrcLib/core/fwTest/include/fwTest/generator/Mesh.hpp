/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTEST_GENERATOR_MESH_HPP__
#define __FWTEST_GENERATOR_MESH_HPP__

#include "fwTest/config.hpp"

#include <fwCore/base.hpp>

#include <fwData/Mesh.hpp>

#include <fwDataTools/Vector.hxx>
#include <fwDataTools/helper/Mesh.hpp>


namespace fwTest
{
namespace generator
{


/**
 * @brief   This helper generates a mesh using specified structure (quad or triangle).
 */
class Mesh
{

public:

    typedef std::map< ::fwDataTools::Point, ::fwData::Mesh::Id> PointsMapType;


    /**
     * @brief Initialize 'rand' seed
     */
    FWTEST_API static void initRand();

    /**
     * @brief Generate a mesh.
     * @param mesh mesh to generate
     */
    FWTEST_API static void generateMesh( ::fwData::Mesh::sptr mesh );


    /**
     * @brief Add quad cells in mesh, this method generates synthetic data (two face of a cube).
     *
     * @param[out]  mesh fwData::Mesh Mesh structure to fill with quad cells.
     * @param[in]   points PointsMapType Point registration map used to not create point which already in mesh buffer.
     * @param[in]   nbPointsByEdge size_t Number of points by edge.
     * @param[in]   edgeDim float Edge dimension in 3D world.
     */
    FWTEST_API static void addQuadMesh(::fwData::Mesh::sptr mesh,
                                       PointsMapType& points,
                                       size_t nbPointsByEdge = 10,
                                       float edgeDim = 100.);

    /**
     * @brief Add triangle cells in mesh, this method generates synthetic data (two face of a cube).
     *
     * @param[out]  mesh fwData::Mesh Mesh structure to fill with triangle cells.
     * @param[in]   points PointsMapType Point registration map used to not create point which already in mesh buffer.
     * @param[in]   nbPointsByEdge size_t Number of points by edge.
     * @param[in]   edgeDim float Edge dimension in 3D world.
     */
    FWTEST_API static void addTriangleMesh(::fwData::Mesh::sptr mesh,
                                           PointsMapType& points,
                                           size_t nbPointsByEdge = 10,
                                           float edgeDim = 100.);

    /**
     * @brief Generate a quad mesh.
     * @see addQuadMesh
     * @param[out]  mesh fwData::Mesh empty mesh structure to fill with quad cells.
     */
    FWTEST_API static void generateQuadMesh(::fwData::Mesh::sptr mesh);

    /**
     * @brief Generate a triangle mesh.
     * @see addTriangleMesh
     * @param[out]  mesh fwData::Mesh empty mesh structure to fill with triangle cell.
     */
    FWTEST_API static void generateTriangleMesh(::fwData::Mesh::sptr mesh);

    /**
     * @brief Generate a mesh with quad and triangle cells.
     * @see addQuadMesh
     * @see addTriangleMesh
     * @param[out]  mesh fwData::Mesh empty mesh structure to fill with quad and triangle cells.
     */
    FWTEST_API static void generateTriangleQuadMesh(::fwData::Mesh::sptr mesh);


protected:

    FWTEST_API static ::fwData::Mesh::Id addPoint(::fwData::Mesh::PointValueType* pt,
                                                  ::fwDataTools::helper::Mesh::sptr meshHelper,
                                                  PointsMapType& points);


};

} // namespace generator
} // namespace fwTest

#endif // __FWTEST_GENERATOR_MESH_HPP__
