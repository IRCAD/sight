/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "io/igtl/detail/converter/base.hpp"
#include "io/igtl/detail/exception/Conversion.hpp"

#include <data/mesh.hpp>

#include <igtlPolyDataMessage.h>

namespace sight::io::igtl::detail::converter
{

/**
 *
 * @brief class to manage conversion between data::mesh and igtl::PolyDataMessage
 */
class IO_IGTL_CLASS_API MeshConverter :
    public base
{
public:

    /// Constructor
    IO_IGTL_API MeshConverter();

    /// Destructor
    IO_IGTL_API ~MeshConverter() override;

    /**
     * @brief convert a igtl::PositionMessage to a data::line
     *
     * @return an data::image converted from an ::igtl::ImageMessage
     */
    [[nodiscard]] IO_IGTL_API data::object::sptr fromIgtlMessage(::igtl::MessageBase::Pointer _src) const override;

    /**
     * @brief convert a data::line to an igtl::PositionMessage
     *
     * @return an  ::igtl::ImageMessage converted from an data::image
     */
    [[nodiscard]] IO_IGTL_API ::igtl::MessageBase::Pointer fromFwDataObject(data::object::csptr _src) const override;

    /**
     * @brief create a new LineConverter smart pointer
     *
     * @return a smart pointer to an ImageConverter
     */
    IO_IGTL_API static base::sptr New();

    /**
     * @brief get the igtlType supported for conversion
     *
     * @return the igtlType supported for conversion
     */
    [[nodiscard]] IO_IGTL_API std::string const& get_igtl_type() const override;

    /**
     * @brief get the fwData object type supported for conversion
     *
     * @return the fwData Object type supported for conversion
     */
    [[nodiscard]] IO_IGTL_API std::string const& getFwDataObjectType() const override;

private:

    /// copy cells from a fwMesh to polydata message
    static void copyCellsFromFwMesh(data::mesh::csptr _mesh_src, ::igtl::PolyDataMessage::Pointer _dest);

    /// copy points from fwMesh to polydata message
    static void copyPointsFromFwMesh(data::mesh::csptr _mesh_src, ::igtl::PolyDataMessage::Pointer _dest);

    /// copy attributes from fwMesh(Normals, Colors) to polydata message
    static void copyAttributesFromFwMesh(
        data::mesh::csptr _mesh_src,
        ::igtl::PolyDataMessage::Pointer _dest
    );

    /// copy attribute from polydata message to fwMesh
    static void copyAttributeFromPolyData(::igtl::PolyDataMessage::Pointer _src, data::mesh::sptr _dest);

private:

    /// igtl type supported for conversion
    static const std::string s_IGTL_TYPE;

    /// fwData type supported for conversion
    static const std::string s_FWDATA_OBJECT_TYPE;
};

} // namespace sight::io::igtl::detail::converter
