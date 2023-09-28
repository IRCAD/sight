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

#include <core/location/multiple_files.hpp>

#include <data/Series.hpp>
#include <data/SeriesSet.hpp>

#include <io/__/reader/GenericObjectReader.hpp>

#include <filesystem>

namespace sight::core::jobs
{

class observer;

}

namespace sight::io::vtk
{

/**
 * @brief   Read a SeriesSet.
 *
 * Read VTK Mesh or Image files using the VTK lib, convert to ModelSeries or ImageSeries and push to SeriesSet.
 */
class IO_VTK_CLASS_API SeriesSetReader :
    public io::reader::GenericObjectReader<data::SeriesSet>,
    public core::location::multiple_files
{
public:

    SIGHT_DECLARE_CLASS(
        SeriesSetReader,
        io::reader::GenericObjectReader<data::SeriesSet>,
        io::reader::factory::make<SeriesSetReader>
    )
    SIGHT_ALLOW_SHARED_FROM_THIS()

    /// @brief Constructor.
    IO_VTK_API SeriesSetReader();

    /// @brief Destructor.
    IO_VTK_API ~SeriesSetReader() override = default;

    /// @brief Reading operator.
    IO_VTK_API void read() override;

    /// @return ".vtk"
    IO_VTK_API std::string extension() const override;

    //------------------------------------------------------------------------------

    void setLazyMode(bool lazyMode)
    {
        m_lazyMode = lazyMode;
    }

    /// @return internal job
    IO_VTK_API SPTR(core::jobs::base) getJob() const override;

private:

    ///Internal job
    SPTR(core::jobs::observer) m_job;

    bool m_lazyMode;
};

} // namespace sight::io::vtk
