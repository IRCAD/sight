/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include <data/image.hpp>

#include <io/__/reader/generic_object_reader.hpp>

#include <vector>

namespace sight::core::jobs
{

class observer;
class base;

} // namespace sight::core::jobs

namespace sight::io::vtk
{

/**
 * @brief   Read a bitmap image.
 *
 * Read a Bitmap image using the VTK library
 */
class bitmap_image_reader : public reader::generic_object_reader<data::image>,
                            public core::location::single_file
{
public:

    SIGHT_DECLARE_CLASS(
        bitmap_image_reader,
        io::reader::generic_object_reader<data::image>,
        io::reader::factory::make<bitmap_image_reader>
    );

    SIGHT_ALLOW_SHARED_FROM_THIS();

    //! @brief Constructor.
    SIGHT_IO_VTK_API bitmap_image_reader();

    //! @brief Destructor.
    SIGHT_IO_VTK_API ~bitmap_image_reader() override;

    //! @brief Reading operator.
    SIGHT_IO_VTK_API void read() override;

    /// @return The available file extensions for loading bitmap images.
    SIGHT_IO_VTK_API std::string extension() const override;

    /// @return internal job
    SIGHT_IO_VTK_API SPTR(core::jobs::base) get_job() const override;

    /// @return A vector of the available bitmap extensions for the vtkImageReader2 class
    SIGHT_IO_VTK_API static void get_available_extensions(std::vector<std::string>& _ext);

private:

    ///Internal job
    SPTR(core::jobs::observer) m_job;

    /// Available file extensions for bitmap files
    std::string m_available_extensions;
};

} // namespace sight::io::vtk
