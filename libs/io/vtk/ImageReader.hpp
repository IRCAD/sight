/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/vtk/config.hpp"

#include <core/location/SingleFile.hpp>

#include <data/Image.hpp>

#include <io/base/reader/GenericObjectReader.hpp>

#include <filesystem>

namespace sight::core::jobs
{

class Observer;

}

namespace sight::io::vtk
{

/**
 * @brief   Read an image.
 *
 * Read a VTK Image using the VTK lib
 */
class ImageReader : public base::reader::GenericObjectReader<data::Image>,
                    public core::location::SingleFile
{
public:

    SIGHT_DECLARE_CLASS(
        ImageReader,
        io::base::reader::GenericObjectReader<data::Image>,
        io::base::reader::factory::New<ImageReader>
    );
    SIGHT_ALLOW_SHARED_FROM_THIS();

    //! @brief Constructor.
    IO_VTK_API ImageReader(io::base::reader::IObjectReader::Key key);

    //! @brief Destructor.
    IO_VTK_API ~ImageReader();

    //! @brief Reading operator.
    IO_VTK_API void read() override;

    /// @return ".vtk"
    IO_VTK_API std::string extension() override;

    /// @return internal job
    IO_VTK_API SPTR(core::jobs::IJob) getJob() const override;

private:

    ///Internal job
    SPTR(core::jobs::Observer) m_job;
};

} // namespace sight::io::vtk
