/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_HELPER_MESH_HPP_
#define _FWCOMED_HELPER_MESH_HPP_

#include <fwTools/Type.hpp>
#include <fwTools/BufferObject.hpp>

#include <fwData/Mesh.hpp>

#include "fwComEd/helper/Array.hpp"
#include "fwComEd/export.hpp"

namespace fwComEd
{

namespace helper
{

/**
 * @class   Mesh
 * @brief   ...
 * @author  IRCAD (Research and Development Team).
 * @date    2012
 */
class FWCOMED_CLASS_API Mesh
{
public :

    FWCOMED_API Mesh( ::fwData::Mesh::sptr mesh );

    FWCOMED_API virtual ~Mesh();

    /**
     * @brief Insert a point into the mesh.
     * Reallocates the point array if needed.
     *
     * @param p[3] point coordinates
     *
     * @return The id of the new point
     *
     * @throw ::fwData::Exception
     */
    FWCOMED_API  ::fwData::Mesh::Id insertNextPoint(const ::fwData::Mesh::PointValueType p[3])
    throw(::fwData::Exception);

    /**
     * @brief Insert a point into the mesh.
     * @see insertNextPoint
     */
    FWCOMED_API ::fwData::Mesh::Id insertNextPoint(::fwData::Mesh::PointValueType x,
                                                   ::fwData::Mesh::PointValueType y,
                                                   ::fwData::Mesh::PointValueType z) throw(::fwData::Exception);

    /**
      * @brief Set a point coordinates.
      * The mesh must be allocated before calling this method.
      *
      * @param id point index
      * @param p[3] point coordinates
      *
      * @throw ::fwData::Exception
      */
     FWCOMED_API void setPoint(::fwData::Mesh::Id id, const ::fwData::Mesh::PointValueType p[3]);

     /**
      * @brief Set a point coordinates.
      * @see setPoint
      */
     FWCOMED_API void setPoint(::fwData::Mesh::Id id,
                               ::fwData::Mesh::PointValueType x,
                               ::fwData::Mesh::PointValueType y,
                               ::fwData::Mesh::PointValueType z);

     /**
      * @brief Set a point color.
      *
      * Depending on the type of allocated point-colors array (RGB or RGBA), 3
      * or 4 points will be read from p.
      * The color array must be allocated before calling this method.
      *
      * @param id point index
      * @param c[3] color
      *
      * @throw ::fwData::Exception
      */
     FWCOMED_API void setPointColor(::fwData::Mesh::Id id, const ::fwData::Mesh::ColorValueType c[4]);

     /**
      * @brief Set a cell color.
      *
      * Depending on the type of allocated cell-colors array (RGB or RGBA), 3 or
      * 4 points will be read from p.
      * The color array must be allocated before calling this method.
      *
      * @param id cell index
      * @param c[3] color
      *
      * @throw ::fwData::Exception
      */
     FWCOMED_API void setCellColor(::fwData::Mesh::Id id, const ::fwData::Mesh::ColorValueType c[4]);

     /**
      * @brief Set a point normal.
      *
      * The normal array must be allocated before calling this method.
      *
      * @param id point index
      * @param n[3] normal
      *
      * @throw ::fwData::Exception
      */
     FWCOMED_API void setPointNormal(::fwData::Mesh::Id id, const ::fwData::Mesh::NormalValueType n[3]);

     /**
      * @brief Set a cell normal.
      *
      * The normal array must be allocated before calling this method.
      *
      * @param id cell index
      * @param n[3] normal
      *
      * @throw ::fwData::Exception
      */
     FWCOMED_API void setCellNormal(::fwData::Mesh::Id id, const ::fwData::Mesh::NormalValueType n[3]);

     /**
      * @brief Insert a cell into the mesh.
      *
      * Reallocates the mesh concerned arrays if needed.
      *
      * @return The id of the new cell
      *
      * @throw ::fwData::Exception
      */
     FWCOMED_API ::fwData::Mesh::Id insertNextCell(::fwData::Mesh::CellTypesEnum type,
                                                   const ::fwData::Mesh::CellValueType *cell,
                                                   size_t nb ) throw(::fwData::Exception);
     /**
      * @brief Insert a cell into the mesh.
      * @see insertNextCell
      */
     FWCOMED_API ::fwData::Mesh::Id insertNextCell(::fwData::Mesh::CellValueType p)  throw(::fwData::Exception);
     /**
      * @brief Insert a cell into the mesh.
      * @see insertNextCell
      */
     FWCOMED_API ::fwData::Mesh::Id insertNextCell(::fwData::Mesh::CellValueType p1,
                                                   ::fwData::Mesh::CellValueType p2 ) throw(::fwData::Exception);
     /**
      * @brief Insert a cell into the mesh.
      * @see insertNextCell
      */
     FWCOMED_API ::fwData::Mesh::Id insertNextCell(::fwData::Mesh::CellValueType p1,
                                                   ::fwData::Mesh::CellValueType p2,
                                                   ::fwData::Mesh::CellValueType p3 ) throw(::fwData::Exception);
     /**
      * @brief Insert a cell into the mesh.
      * @see insertNextCell
      */
     FWCOMED_API ::fwData::Mesh::Id insertNextCell(::fwData::Mesh::CellValueType p1,
                                                   ::fwData::Mesh::CellValueType p2,
                                                   ::fwData::Mesh::CellValueType p3,
                                                   ::fwData::Mesh::CellValueType p4 ) throw(::fwData::Exception);
     /**
      * @brief Insert a cell into the mesh.
      * @see insertNextCell
      */
     template <typename T> ::fwData::Mesh::Id insertNextCell(::fwData::Mesh::CellTypesEnum type,
                                                             const T &pointsBegin,
                                                             const T &pointsEnd ) throw(::fwData::Exception);



     /// Returns the internal corresponding array as a boost::multi_array_ref
     FWCOMED_API ::fwData::Mesh::PointsMultiArrayType          getPoints() const;
     /// Returns the internal corresponding array as a boost::multi_array_ref
     FWCOMED_API ::fwData::Mesh::CellTypesMultiArrayType       getCellTypes() const;
     /// Returns the internal corresponding array as a boost::multi_array_ref
     FWCOMED_API ::fwData::Mesh::CellDataMultiArrayType        getCellData() const;
     /// Returns the internal corresponding array as a boost::multi_array_ref
     FWCOMED_API ::fwData::Mesh::CellDataOffsetsMultiArrayType getCellDataOffsets() const;
     /// Returns the internal corresponding array as a boost::multi_array_ref
     FWCOMED_API ::fwData::Mesh::PointColorsMultiArrayType     getPointColors() const;
     /// Returns the internal corresponding array as a boost::multi_array_ref
     FWCOMED_API ::fwData::Mesh::CellColorsMultiArrayType      getCellColors() const;
     /// Returns the internal corresponding array as a boost::multi_array_ref
     FWCOMED_API ::fwData::Mesh::PointNormalsMultiArrayType    getPointNormals() const;
     /// Returns the internal corresponding array as a boost::multi_array_ref
     FWCOMED_API ::fwData::Mesh::CellNormalsMultiArrayType     getCellNormals() const;


protected:

    ::fwData::Mesh::sptr m_mesh;

    ::fwComEd::helper::Array::sptr m_helperPoints;
    ::fwComEd::helper::Array::sptr m_helperCellTypes;
    ::fwComEd::helper::Array::sptr m_helperCellData;
    ::fwComEd::helper::Array::sptr m_helperCellDataOffsets;
    ::fwComEd::helper::Array::sptr m_helperPointColors;
    ::fwComEd::helper::Array::sptr m_helperCellColors;
    ::fwComEd::helper::Array::sptr m_helperPointNormals;
    ::fwComEd::helper::Array::sptr m_helperCellNormals;

};


} // namespace helper

} // namespace fwData

#endif // _FWCOMED_HELPER_MESH_HPP_
