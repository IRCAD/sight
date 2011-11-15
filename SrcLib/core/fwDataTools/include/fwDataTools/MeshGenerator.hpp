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

    /**
     * @brief Generate a quad mesh.
     *
     * @param[in]  mesh fwData::Mesh empty mesh structure to fill with quad cells.
     */
    FWDATATOOLS_API void generateQuadMesh(::fwData::Mesh::sptr mesh);

    /**
     * @brief Generate a triangle mesh.
     *
     * @param[in]  mesh fwData::Mesh empty mesh structure to fill with triangle cell.
     */
    FWDATATOOLS_API void generateTriangleMesh(::fwData::Mesh::sptr mesh);

    FWDATATOOLS_API static void toTriangularMesh(::fwData::Mesh::sptr mesh, ::fwData::TriangularMesh::sptr trian);

    FWDATATOOLS_API static void fromTriangularMesh(::fwData::TriangularMesh::sptr trian, ::fwData::Mesh::sptr mesh);

    FWDATATOOLS_API static bool hasUniqueCellType(::fwData::Mesh::sptr mesh, ::fwData::Mesh::CellTypes cell);

protected:

    FWDATATOOLS_API ::fwData::Mesh::Id addPoint(::fwData::Mesh::PointValueType* pt, ::fwData::Mesh::sptr mesh);

    //! @brief Constructor.
    FWDATATOOLS_API MeshGenerator();

    //! @brief Destructor.
    FWDATATOOLS_API virtual ~MeshGenerator();

    typedef std::map<Point, ::fwData::Mesh::Id> PointsMapType;
    PointsMapType m_points;
};

} // namespace fwDataTools

#endif // _FWDATATOOLS_MESHGENERATOR_HPP_
