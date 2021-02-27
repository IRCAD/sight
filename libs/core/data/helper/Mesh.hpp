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

#include "data/config.hpp"
#include "data/helper/Array.hpp"

#include <core/memory/BufferObject.hpp>
#include <core/tools/Type.hpp>

#include <data/Mesh.hpp>

namespace sight::data
{

namespace helper
{

/**
 * @brief   Helper to manage Mesh. Lock the mesh buffer before to modify it.
 * @deprecated The helper is no longer required to access the buffers. It will be removed in sight 22.0
 */
class DATA_DEPRECATED_CLASS_API Mesh
{
public:

    /// Shared pointer type
    typedef std::shared_ptr < data::helper::Mesh > sptr;
    /// Const shared pointer type
    typedef std::shared_ptr < const data::helper::Mesh > csptr;
    /// Unique pointer type
    typedef std::unique_ptr < data::helper::Mesh > uptr;
    /// Const unique pointer type
    typedef std::unique_ptr < const data::helper::Mesh > cuptr;

    //------------------------------------------------------------------------------

    static sptr New(data::Mesh::sptr mesh)
    {
        return std::make_shared< data::helper::Mesh >(mesh);
    }

    DATA_API Mesh( data::Mesh::sptr mesh );

    DATA_API virtual ~Mesh();

    /// Updates locks on all internal data Array of Mesh
    DATA_API void updateLock();

    /**
     * @brief Insert a point into the mesh.
     * Reallocates the point array if needed.
     *
     * @param p point coordinates
     *
     * @return The id of the new point
     *
     * @throw data::Exception
     */
    DATA_API data::Mesh::PointId insertNextPoint(const data::Mesh::PointValueType p[3]);

    /**
     * @brief Insert a point into the mesh.
     * @see insertNextPoint
     */
    DATA_API data::Mesh::PointId insertNextPoint(data::Mesh::PointValueType x,
                                                 data::Mesh::PointValueType y,
                                                 data::Mesh::PointValueType z);

    /**
     * @brief Set a point coordinates.
     * The mesh must be allocated before calling this method.
     *
     * @param id point index
     * @param p point coordinates
     *
     * @throw data::Exception
     */
    DATA_API void setPoint(data::Mesh::PointId id, const data::Mesh::PointValueType p[3]);

    /**
     * @brief Set a point coordinates.
     * @see setPoint
     */
    DATA_API void setPoint(data::Mesh::PointId id,
                           data::Mesh::PointValueType x,
                           data::Mesh::PointValueType y,
                           data::Mesh::PointValueType z);

    /**
     * @brief Set a point color.
     *
     * Depending on the type of allocated point-colors array (RGB or RGBA), 3
     * or 4 points will be read from p.
     * The color array must be allocated before calling this method.
     *
     * @param id point index
     * @param c color
     *
     * @throw data::Exception
     */
    DATA_API void setPointColor(data::Mesh::PointId id, const data::Mesh::ColorValueType c[4]);

    /**
     * @brief Set a cell color.
     *
     * Depending on the type of allocated cell-colors array (RGB or RGBA), 3 or
     * 4 points will be read from p.
     * The color array must be allocated before calling this method.
     *
     * @param id cell index
     * @param c color
     *
     * @throw data::Exception
     */
    DATA_API void setCellColor(data::Mesh::CellId id, const data::Mesh::ColorValueType c[4]);

    /**
     * @brief Set a point normal.
     *
     * The normal array must be allocated before calling this method.
     *
     * @param id point index
     * @param n normal
     *
     * @throw data::Exception
     */
    DATA_API void setPointNormal(data::Mesh::PointId id, const data::Mesh::NormalValueType n[3]);

    /**
     * @brief Set a cell normal.
     *
     * The normal array must be allocated before calling this method.
     *
     * @param id cell index
     * @param n normal
     *
     * @throw data::Exception
     */
    DATA_API void setCellNormal(data::Mesh::CellId id, const data::Mesh::NormalValueType n[3]);

    /**
     * @brief Set a point texCoord.
     *
     * The texCoord array must be allocated before calling this method.
     *
     * @param id point index
     * @param t texCoord
     *
     * @throw data::Exception
     */
    DATA_API void setPointTexCoord(data::Mesh::PointId id, const data::Mesh::TexCoordValueType t[2]);

    /**
     * @brief Set a cell texCoord.
     *
     * The texCoord array must be allocated before calling this method.
     *
     * @param id cell index
     * @param t texCoord
     *
     * @throw data::Exception
     */
    DATA_API void setCellTexCoord(data::Mesh::CellId id, const data::Mesh::TexCoordValueType t[2]);

    /**
     * @brief Insert a cell into the mesh.
     *
     * Reallocates the mesh concerned arrays if needed.
     *
     * @return The id of the new cell
     *
     * @throw data::Exception
     */
    DATA_API data::Mesh::CellId insertNextCell(data::Mesh::CellTypesEnum type,
                                               const data::Mesh::CellId* cell,
                                               size_t nb );
    /**
     * @brief Insert a cell into the mesh.
     * @see insertNextCell
     */
    DATA_API data::Mesh::CellId insertNextCell(data::Mesh::PointId p);
    /**
     * @brief Insert a cell into the mesh.
     * @see insertNextCell
     */
    DATA_API data::Mesh::CellId insertNextCell(data::Mesh::PointId p1,
                                               data::Mesh::PointId p2 );
    /**
     * @brief Insert a cell into the mesh.
     * @see insertNextCell
     */
    DATA_API data::Mesh::CellId insertNextCell(data::Mesh::PointId p1,
                                               data::Mesh::PointId p2,
                                               data::Mesh::PointId p3 );
    /**
     * @brief Insert a cell into the mesh.
     * @see insertNextCell
     */
    DATA_API data::Mesh::CellId insertNextCell(data::Mesh::PointId p1,
                                               data::Mesh::PointId p2,
                                               data::Mesh::PointId p3,
                                               data::Mesh::PointId p4,
                                               data::Mesh::CellTypesEnum type = data::Mesh::QUAD);
    /**
     * @brief Insert a cell into the mesh.
     * @see insertNextCell
     */
    template <typename T> data::Mesh::CellId insertNextCell(data::Mesh::CellTypesEnum type,
                                                            const T& pointsBegin,
                                                            const T& pointsEnd );

    /// Returns the internal corresponding array as a boost::multi_array_ref
    DATA_API data::Mesh::PointsMultiArrayType          getPoints() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    DATA_API data::Mesh::CellTypesMultiArrayType       getCellTypes() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    DATA_API data::Mesh::CellDataMultiArrayType        getCellData() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    DATA_API data::Mesh::CellDataOffsetsMultiArrayType getCellDataOffsets() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    DATA_API data::Mesh::PointColorsMultiArrayType     getPointColors() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    DATA_API data::Mesh::CellColorsMultiArrayType      getCellColors() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    DATA_API data::Mesh::PointNormalsMultiArrayType    getPointNormals() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    DATA_API data::Mesh::CellNormalsMultiArrayType     getCellNormals() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    DATA_API data::Mesh::PointTexCoordsMultiArrayType  getPointTexCoords() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    DATA_API data::Mesh::CellTexCoordsMultiArrayType   getCellTexCoords() const;

    /// Returns the internal data::Mesh
    DATA_API data::Mesh::csptr getMesh() const;

    /*!
     * @brief Return true if the mesh is closed.
     * The result is computed for each call.
     */
    [[deprecated("moved to data::tools::Mesh::isClosed()")]] DATA_API bool isClosed();

protected:

    data::Mesh::sptr m_mesh;

    data::helper::Array::sptr m_helperPoints;
    data::helper::Array::sptr m_helperCellTypes;
    data::helper::Array::sptr m_helperCellData;
    data::helper::Array::sptr m_helperCellDataOffsets;
    data::helper::Array::sptr m_helperPointColors;
    data::helper::Array::sptr m_helperCellColors;
    data::helper::Array::sptr m_helperPointNormals;
    data::helper::Array::sptr m_helperCellNormals;
    data::helper::Array::sptr m_helperPointTexCoords;
    data::helper::Array::sptr m_helperCellTexCoords;

};

} // namespace helper

} // namespace sight::data
