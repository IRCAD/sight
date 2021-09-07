/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include <core/location/SingleFile.hpp>

#include <data/Image.hpp>

#include <io/base/reader/GenericObjectReader.hpp>

#include <vector>

namespace sight::core::jobs
{

class Observer;
class IJob;

}

namespace sight::io::vtk
{

/**
 * @brief   Read a bitmap image.
 *
 * Read a Bitmap Image using the VTK library
 */
class BitmapImageReader : public base::reader::GenericObjectReader<data::Image>,
                          public core::location::SingleFile
{
public:

    SIGHT_DECLARE_CLASS(
        BitmapImageReader,
        io::base::reader::GenericObjectReader<data::Image>,
        io::base::reader::factory::New<BitmapImageReader>
    );

    SIGHT_ALLOW_SHARED_FROM_THIS();

    //! @brief Constructor.
    IO_VTK_API BitmapImageReader(io::base::reader::IObjectReader::Key key);

    //! @brief Destructor.
    IO_VTK_API ~BitmapImageReader();

    //! @brief Reading operator.
    IO_VTK_API void read() override;

    /// @return The available file extensions for loading bitmap images.
    IO_VTK_API std::string extension() override;

    /// @return internal job
    IO_VTK_API SPTR(core::jobs::IJob) getJob() const override;

    /// @return A vector of the available bitmap extensions for the vtkImageReader2 class
    IO_VTK_API static void getAvailableExtensions(std::vector<std::string>& ext);

private:

    ///Internal job
    SPTR(core::jobs::Observer) m_job;

    /// Available file extensions for bitmap files
    std::string m_availableExtensions;
};

} // namespace sight::io::vtk
