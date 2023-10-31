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
#include "io/igtl/detail/exception/conversion.hpp"

#include <data/mesh.hpp>

#include <igtlPolyDataMessage.h>

namespace sight::io::igtl::detail::converter
{

/**
 *
 * @brief class to manage conversion between data::mesh and igtl::PolyDataMessage
 */
class IO_IGTL_CLASS_API mesh_converter :
    public base
{
public:

    /// Constructor
    IO_IGTL_API mesh_converter();

    /// Destructor
    IO_IGTL_API ~mesh_converter() override;

    /**
     * @brief convert a igtl::PositionMessage to a data::line
     *
     * @return an data::image converted from an ::igtl::ImageMessage
     */
    [[nodiscard]] IO_IGTL_API data::object::sptr from_igtl_message(::igtl::MessageBase::Pointer _src) const override;

    /**
     * @brief convert a data::line to an igtl::PositionMessage
     *
     * @return an  ::igtl::ImageMessage converted from an data::image
     */
    [[nodiscard]] IO_IGTL_API ::igtl::MessageBase::Pointer from_fw_data_object(data::object::csptr _src) const override;

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
    [[nodiscard]] IO_IGTL_API std::string const& get_fw_data_object_type() const override;

private:

    /// copy cells from a fwMesh to polydata message
    static void copy_cells_from_fw_mesh(data::mesh::csptr _mesh_src, ::igtl::PolyDataMessage::Pointer _dest);

    /// copy points from fwMesh to polydata message
    static void copy_points_from_fw_mesh(data::mesh::csptr _mesh_src, ::igtl::PolyDataMessage::Pointer _dest);

    /// copy attributes from fwMesh(Normals, Colors) to polydata message
    static void copy_attributes_from_fw_mesh(
        data::mesh::csptr _mesh_src,
        ::igtl::PolyDataMessage::Pointer _dest
    );

    /// copy attribute from polydata message to fwMesh
    static void copy_attribute_from_poly_data(::igtl::PolyDataMessage::Pointer _src, data::mesh::sptr _dest);

private:

    /// igtl type supported for conversion
    static const std::string IGTL_TYPE;

    /// fwData type supported for conversion
    static const std::string FWDATA_OBJECT_TYPE;
};

} // namespace sight::io::igtl::detail::converter
