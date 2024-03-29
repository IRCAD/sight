/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "io/vtk/config.hpp"

#include <core/location/single_file.hpp>

#include <data/mesh.hpp>

#include <io/__/writer/generic_object_writer.hpp>

#include <filesystem>

namespace sight::core::jobs
{

class observer;
class base;

} // namespace sight::core::jobs

namespace sight::io::vtk
{

/**
 * @brief Writes a Mesh in .ply format using VTK library.
 */

class IO_VTK_CLASS_API ply_mesh_writer :
    public writer::generic_object_writer<data::mesh>,
    public core::location::single_file
{
public:

    SIGHT_DECLARE_CLASS(
        ply_mesh_writer,
        io::writer::generic_object_writer<data::mesh>,
        io::writer::factory::make<ply_mesh_writer>
    )
    SIGHT_ALLOW_SHARED_FROM_THIS()

    /// @brief Constructor.
    IO_VTK_API ply_mesh_writer();

    /// @brief Destructor.
    IO_VTK_API ~ply_mesh_writer() override;

    /// @brief Reading operator.
    IO_VTK_API void write() override;

    /// @return ".ply"
    IO_VTK_API std::string extension() const override;

    /// @return internal job
    IO_VTK_API SPTR(core::jobs::base) get_job() const override;

private:

    ///Internal job
    SPTR(core::jobs::observer) m_job;
};

} // namespace sight::io::vtk
