/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <core/location/single_file.hpp>

#include <data/image.hpp>

#include <io/__/reader/generic_object_reader.hpp>

#include <filesystem>

namespace sight::core::jobs
{

class observer;
class base;

} // namespace sight::core::jobs

namespace sight::io::vtk
{

/**
 * @brief   Read an image.
 *
 * Read a VTK image using the VTK lib
 */
class IO_VTK_CLASS_API VtiImageReader : public reader::generic_object_reader<data::image>,
                                        public core::location::single_file
{
public:

    SIGHT_DECLARE_CLASS(
        VtiImageReader,
        io::reader::generic_object_reader<data::image>,
        io::reader::factory::make<VtiImageReader>
    );

    SIGHT_ALLOW_SHARED_FROM_THIS();

    //! @brief Constructor.
    IO_VTK_API VtiImageReader();

    //! @brief Destructor.
    IO_VTK_API ~VtiImageReader() override;

    //! @brief Reading operator.
    IO_VTK_API void read() override;

    /// @return ".vtk"
    IO_VTK_API std::string extension() const override;

    /// @return internal job
    IO_VTK_API SPTR(core::jobs::base) getJob() const override;

private:

    ///Internal job
    SPTR(core::jobs::observer) m_job;
};

} // namespace sight::io::vtk
