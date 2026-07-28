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

#include <core/location/single_folder.hpp>

#include <io/__/writer/generic_object_writer.hpp>

namespace sight::data
{

class model_series;

} // namespace sight::data

namespace sight::io::vtk
{

/**
 * @brief   Write a data::reconstruction.
 *
 * Write a data::reconstruction as .obj file using the VTK lib and the obj format.
 */
class SIGHT_IO_VTK_CLASS_API model_series_obj_writer : public io::writer::generic_object_writer<data::model_series>,
                                                       public core::location::single_folder
{
public:

    SIGHT_DECLARE_CLASS(model_series_obj_writer, io::writer::generic_object_writer<data::model_series>)

    /// Writing operator.
    SIGHT_IO_VTK_API void write(sight::sptr<sight::core::progress::observer> _progress) override;

    /**
     * @return ".obj"
     */
    SIGHT_IO_VTK_API std::string extension() const override;
};

} // namespace sight::io::vtk
