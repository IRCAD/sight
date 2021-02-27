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
#include "data/helper/ArrayGetter.hpp"

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
class DATA_DEPRECATED_CLASS_API MeshGetter
{
public:
    [[deprecated("will be removed in sight 22.0, please use data::Mesh")]]
    DATA_API MeshGetter( data::Mesh::csptr mesh );

    DATA_API virtual ~MeshGetter();

    /// Returns the internal corresponding array as a boost::multi_array_ref
    DATA_API data::Mesh::ConstPointsMultiArrayType          getPoints() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    DATA_API data::Mesh::ConstCellTypesMultiArrayType       getCellTypes() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    DATA_API data::Mesh::ConstCellDataMultiArrayType        getCellData() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    DATA_API data::Mesh::ConstCellDataOffsetsMultiArrayType getCellDataOffsets() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    DATA_API data::Mesh::ConstPointColorsMultiArrayType     getPointColors() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    DATA_API data::Mesh::ConstCellColorsMultiArrayType      getCellColors() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    DATA_API data::Mesh::ConstPointNormalsMultiArrayType    getPointNormals() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    DATA_API data::Mesh::ConstCellNormalsMultiArrayType     getCellNormals() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    DATA_API data::Mesh::ConstPointTexCoordsMultiArrayType  getPointTexCoords() const;
    /// Returns the internal corresponding array as a boost::multi_array_ref
    DATA_API data::Mesh::ConstCellTexCoordsMultiArrayType   getCellTexCoords() const;

    /// Returns the internal data::Mesh
    DATA_API data::Mesh::csptr getMesh() const;

    /*!
     * @brief Return true if the mesh is closed.
     * The result is computed for each call.
     */
    [[deprecated("moved to data::tools::Mesh::isClosed()")]] DATA_API bool isClosed();

protected:

    data::Mesh::csptr m_mesh;

    data::helper::ArrayGetter::uptr m_helperPoints;
    data::helper::ArrayGetter::uptr m_helperCellTypes;
    data::helper::ArrayGetter::uptr m_helperCellData;
    data::helper::ArrayGetter::uptr m_helperCellDataOffsets;
    data::helper::ArrayGetter::uptr m_helperPointColors;
    data::helper::ArrayGetter::uptr m_helperCellColors;
    data::helper::ArrayGetter::uptr m_helperPointNormals;
    data::helper::ArrayGetter::uptr m_helperCellNormals;
    data::helper::ArrayGetter::uptr m_helperPointTexCoords;
    data::helper::ArrayGetter::uptr m_helperCellTexCoords;

};

} // namespace helper

} // namespace sight::data
