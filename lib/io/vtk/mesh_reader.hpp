/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <sight/io/vtk/config.hpp>

#include <core/location/single_file.hpp>

#include <data/mesh.hpp>

#include <io/__/reader/generic_object_reader.hpp>

namespace sight::io::vtk
{

/**
 * @brief Reads a Mesh from .vtk (legacy) file using VTK library.
 *
 */
class SIGHT_IO_VTK_CLASS_API mesh_reader :
    public reader::generic_object_reader<data::mesh>,
    public core::location::single_file
{
public:

    SIGHT_DECLARE_CLASS(mesh_reader, io::reader::generic_object_reader<data::mesh>)

    /// @brief Reading operator.
    SIGHT_IO_VTK_API void read(sight::sptr<sight::core::progress::observer> _progress) override;

    /// @return ".vtk"
    SIGHT_IO_VTK_API std::string extension() const override;
};

} // namespace sight::io::vtk
