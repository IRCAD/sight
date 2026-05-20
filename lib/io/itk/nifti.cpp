/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include "nifti.hpp"

#include "nifti_image_reader.hpp"
#include "nifti_image_writer.hpp"

#include <core/progress/observer.hpp>

namespace sight::io::itk
{

//------------------------------------------------------------------------------

bool read_nifti_image(const std::filesystem::path& _path, const sight::data::image::sptr& _image)
{
    auto nifti_reader = std::make_shared<sight::io::itk::nifti_image_reader>();
    nifti_reader->set_file(_path);
    nifti_reader->set_object(_image);

    try
    {
        auto progress = std::make_shared<sight::core::progress::observer>("Read Nifti image");
        nifti_reader->read(progress);
    }
    catch(const std::exception&)
    {
        SIGHT_ERROR("Cannot read image " << std::quoted(_path.string()));
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------

bool write_nifti_image(const std::filesystem::path& _path, const sight::data::image::csptr& _image)
{
    auto writer = std::make_shared<sight::io::itk::nifti_image_writer>();
    writer->set_file(_path);
    writer->set_object(_image);

    try
    {
        // Ignore progress for now
        const auto progress = std::make_shared<sight::core::progress::observer>("Write Nifti image");
        writer->write(progress);
    }
    catch(const std::exception& e)
    {
        SIGHT_ERROR("Error during saving : " << e.what());
        return false;
    }
    catch(...)
    {
        SIGHT_ERROR("Error during saving");
        return false;
    }

    return true;
}

} // namespace sight::io::itk
