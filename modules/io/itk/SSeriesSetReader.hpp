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

#include "modules/io/itk/config.hpp"

#include <core/macros.hpp>

#include <io/base/service/IReader.hpp>

#include <filesystem>
#include <string>

namespace sight::data
{

class Image;

}

namespace sight::data
{

class Series;

}

namespace sight::module::io::itk
{

/**
 * @brief Reads inr files and pushes them into SeriesSet.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::itk::SSeriesSetReader">
       <inout key="data" uid="..." />
       <file>...</file>
       <file>...</file>
       <file>...</file>
       <file>...</file>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [sight::data::SeriesSet]: store the loaded images.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the images to load, if it not defined, 'openLocationDialog()' should be called to
 * define the path.
 */
class MODULE_IO_ITK_CLASS_API SSeriesSetReader : public sight::io::base::service::IReader
{
public:

    SIGHT_DECLARE_SERVICE(SSeriesSetReader, sight::io::base::service::IReader);

    MODULE_IO_ITK_API SSeriesSetReader() noexcept = default;

    MODULE_IO_ITK_API ~SSeriesSetReader() noexcept override = default;

protected:

    /// Does nothing.
    void starting() override
    {
    }

    /// Does nothing.
    void stopping() override
    {
    }

    /// Calls base class implementation
    void configuring() override;

    /// Reads inr files specified by user (configure or openLocationDialog) and pushes them into SeriesSet.
    MODULE_IO_ITK_API void updating() override;

    /**
     * @brief Configure the inr files path.
     *
     * This method is used to find the inr files path using a files selector.
     */
    MODULE_IO_ITK_API void openLocationDialog() override;

    /// Returns managed file type, here FILES
    MODULE_IO_ITK_API sight::io::base::service::IOPathType getIOPathType() const override;

private:

    /// Initializes Series with dummy values and Study with specified instanceUID.
    static void initSeries(SPTR(data::Series) series, const std::string& instanceUID);
};

} // namespace sight::module::io::itk
