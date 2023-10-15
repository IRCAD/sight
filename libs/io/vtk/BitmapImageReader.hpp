/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "io/vtk/config.hpp"

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
class BitmapImageReader : public reader::generic_object_reader<data::image>,
                          public core::location::single_file
{
public:

    SIGHT_DECLARE_CLASS(
        BitmapImageReader,
        io::reader::generic_object_reader<data::image>,
        io::reader::factory::make<BitmapImageReader>
    );

    SIGHT_ALLOW_SHARED_FROM_THIS();

    //! @brief Constructor.
    IO_VTK_API BitmapImageReader();

    //! @brief Destructor.
    IO_VTK_API ~BitmapImageReader() override;

    //! @brief Reading operator.
    IO_VTK_API void read() override;

    /// @return The available file extensions for loading bitmap images.
    IO_VTK_API std::string extension() const override;

    /// @return internal job
    IO_VTK_API SPTR(core::jobs::base) getJob() const override;

    /// @return A vector of the available bitmap extensions for the vtkImageReader2 class
    IO_VTK_API static void getAvailableExtensions(std::vector<std::string>& ext);

private:

    ///Internal job
    SPTR(core::jobs::observer) m_job;

    /// Available file extensions for bitmap files
    std::string m_availableExtensions;
};

} // namespace sight::io::vtk
