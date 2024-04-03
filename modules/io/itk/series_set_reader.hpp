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

#include <core/macros.hpp>

#include <io/__/service/reader.hpp>

#include <filesystem>
#include <string>

namespace sight::data
{

class image;

}

namespace sight::data
{

class series;

}

namespace sight::module::io::itk
{

/**
 * @brief Reads inr files and pushes them into series_set.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::itk::series_set_reader">
       <inout key="data" uid="..." />
       <file>...</file>
       <file>...</file>
       <file>...</file>
       <file>...</file>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [sight::data::series_set]: store the loaded images.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the images to load, if it not defined, 'open_location_dialog()' should be called to
 * define the path.
 */
class series_set_reader : public sight::io::service::reader
{
public:

    SIGHT_DECLARE_SERVICE(series_set_reader, sight::io::service::reader);

    series_set_reader() noexcept = default;

    ~series_set_reader() noexcept override = default;

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

    /// Reads inr files specified by user (configure or open_location_dialog) and pushes them into series_set.
    void updating() override;

    /**
     * @brief Configure the inr files path.
     *
     * This method is used to find the inr files path using a files selector.
     */
    void open_location_dialog() override;

    /// Returns managed file type, here FILES
    sight::io::service::path_type_t get_path_type() const override;

private:

    /// Initializes Series with dummy values and Study with specified instanceUID.
    static void init_series(SPTR(data::series) _series, const std::string& _instance_uid);
};

} // namespace sight::module::io::itk
