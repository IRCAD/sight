/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/vtk/config.hpp"

#include <core/location/single_folder.hpp>

#include <io/__/writer/generic_object_writer.hpp>

#include <filesystem>

namespace sight::data
{

class Reconstruction;

} // namespace sight::data

namespace sight::data
{

class model_series;

} // namespace sight::data

namespace sight::core::jobs
{

class observer;

} // namespace sight::core::jobs

namespace sight::io::vtk
{

/**
 * @brief   Write a data::reconstruction.
 *
 * Write a data::reconstruction as .obj file using the VTK lib and the obj format.
 */
class model_series_obj_writer : public io::writer::generic_object_writer<data::model_series>,
                                public core::location::single_folder
{
public:

    SIGHT_DECLARE_CLASS(
        model_series_obj_writer,
        io::writer::generic_object_writer<data::model_series>,
        io::writer::factory::make<model_series_obj_writer>
    )

    SIGHT_ALLOW_SHARED_FROM_THIS()

    /// Constructor.
    IO_VTK_API model_series_obj_writer();

    /// Destructor.
    IO_VTK_API ~model_series_obj_writer() override;

    /// Writing operator.
    IO_VTK_API void write() override;

    /**
     * @return ".obj"
     */
    IO_VTK_API std::string extension() const override;

    /// @return internal job
    IO_VTK_API SPTR(core::jobs::base) get_job() const override;

private:

    ///Internal job
    SPTR(core::jobs::observer) m_job;
};

} // namespace sight::io::vtk
