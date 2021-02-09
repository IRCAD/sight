/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <io/base/services/IReader.hpp>

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

namespace sight::modules::io::itk
{

/**
 * @brief Reads inr files and pushes them into SeriesDB.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::sight::modules::io::itk::SInrSeriesDBReader">
       <inout key="data" uid="..." />
       <file>...</file>
       <file>...</file>
       <file>...</file>
       <file>...</file>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [data::SeriesDB]: store the loaded images.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the images to load, if it not defined, 'openLocationDialog()' should be called to
 * define the path.
 */
class MODULE_IO_ITK_CLASS_API SInrSeriesDBReader : public sight::io::base::services::IReader
{

public:
    fwCoreServiceMacro(SInrSeriesDBReader, sight::io::base::services::IReader)

    MODULE_IO_ITK_API SInrSeriesDBReader() noexcept;

    MODULE_IO_ITK_API virtual ~SInrSeriesDBReader() noexcept;

protected:

    /// Does nothing.
    virtual void starting() override
    {
    }

    /// Does nothing.
    virtual void stopping() override
    {
    }

    /// Calls base class implementation
    virtual void configuring() override;

    /// Reads inr files specified by user (configure or openLocationDialog) and pushes them into SeriesDB.
    MODULE_IO_ITK_API virtual void updating() override;

    /**
     * @brief Configure the inr files path.
     *
     * This method is used to find the inr files path using a files selector.
     * @deprecated Will be removed in sight 22.0. Use openLocationDialog() instead.
     */
    [[deprecated("Will be removed in sight 22.0. Use openLocationDialog() instead.")]]
    MODULE_IO_ITK_API virtual void configureWithIHM() override;

    /**
     * @brief Configure the inr files path.
     *
     * This method is used to find the inr files path using a files selector.
     */
    MODULE_IO_ITK_API virtual void openLocationDialog() override;

    /// Returns managed file type, here FILES
    MODULE_IO_ITK_API sight::io::base::services::IOPathType getIOPathType() const override;

private:

    /// Initializes Series with dummy values and Study with specified instanceUID.
    void initSeries(SPTR(data::Series) series, const std::string& instanceUID);

    /// Reads specified inr file in image.
    bool createImage( const std::filesystem::path inrFile, SPTR(data::Image) image );
};

} // namespace sight::modules::io::itk
