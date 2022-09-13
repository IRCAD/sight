/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include <core/location/MultipleFiles.hpp>

#include <data/Series.hpp>
#include <data/SeriesSet.hpp>

#include <io/base/reader/GenericObjectReader.hpp>

#include <filesystem>

namespace sight::core::jobs
{

class Observer;

}

namespace sight::io::vtk
{

/**
 * @brief   Read a SeriesSet.
 *
 * Read VTK Mesh or Image files using the VTK lib, convert to ModelSeries or ImageSeries and push to SeriesSet.
 */
class IO_VTK_CLASS_API SeriesSetReader :
    public io::base::reader::GenericObjectReader<data::SeriesSet>,
    public core::location::MultipleFiles
{
public:

    SIGHT_DECLARE_CLASS(
        SeriesSetReader,
        io::base::reader::GenericObjectReader<data::SeriesSet>,
        io::base::reader::factory::New<SeriesSetReader>
    )
    SIGHT_ALLOW_SHARED_FROM_THIS()

    /// @brief Constructor.
    IO_VTK_API SeriesSetReader(io::base::reader::IObjectReader::Key key);

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
    IO_VTK_API SPTR(core::jobs::IJob) getJob() const override;

private:

    ///Internal job
    SPTR(core::jobs::Observer) m_job;

    bool m_lazyMode;
};

} // namespace sight::io::vtk
