/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <core/location/multiple_files.hpp>

#include <data/series.hpp>
#include <data/series_set.hpp>

#include <io/__/reader/generic_object_reader.hpp>

#include <filesystem>

namespace sight::core::jobs
{

class aggregator;

}

namespace sight::io::vtk
{

/**
 * @brief   Read a series_set.
 *
 * Read VTK Mesh or image files using the VTK lib, convert to ModelSeries or ImageSeries and push to series_set.
 */
class SIGHT_IO_VTK_CLASS_API series_set_reader :
    public io::reader::generic_object_reader<data::series_set>,
    public core::location::multiple_files
{
public:

    SIGHT_DECLARE_CLASS(series_set_reader, io::reader::generic_object_reader<data::series_set>)

    /// @brief Constructor.
    SIGHT_IO_VTK_API series_set_reader();

    /// @brief Destructor.
    SIGHT_IO_VTK_API ~series_set_reader() override = default;

    /// @brief Reading operator.
    SIGHT_IO_VTK_API void read() override;

    /// @return ".vtk"
    SIGHT_IO_VTK_API std::string extension() const override;

    //------------------------------------------------------------------------------

    void set_lazy_mode(bool _lazy_mode)
    {
        m_lazy_mode = _lazy_mode;
    }

    /// @return internal job
    SIGHT_IO_VTK_API SPTR(core::jobs::base) get_job() const override;

private:

    ///Internal job
    SPTR(core::jobs::aggregator) m_job;

    bool m_lazy_mode;
};

} // namespace sight::io::vtk
