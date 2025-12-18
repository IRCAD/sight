/************************************************************************
 *
 * Copyright (C) 2020-2025 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include <sight/io/vtk/config.hpp>

#include <core/location/single_file.hpp>

#include <data/mesh.hpp>

#include <io/__/writer/generic_object_writer.hpp>

#include <filesystem>

namespace sight::io::vtk
{

/**
 * @brief Writes a Mesh in .vtp format using VTK library.
 */

class SIGHT_IO_VTK_CLASS_API vtp_mesh_writer :
    public writer::generic_object_writer<data::mesh>,
    public core::location::single_file
{
public:

    SIGHT_DECLARE_CLASS(vtp_mesh_writer, io::writer::generic_object_writer<data::mesh>)

    /// @brief Reading operator.
    SIGHT_IO_VTK_API void write(SPTR(sight::core::progress::observer) _progress) override;

    /// @return ".vtp"
    SIGHT_IO_VTK_API std::string extension() const override;
};

} // namespace sight::io::vtk
