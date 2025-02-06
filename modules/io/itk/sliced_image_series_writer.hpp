/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <io/__/service/writer.hpp>

#include <filesystem>
#include <string>

namespace sight::module::io::itk
{

/**
 * @brief Write an image to jpg format
 **/

/**
 * @brief Write an image series to jpg format
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::itk::sliced_image_series_writer">
       <in key="data" uid="..." />
       <folder>...</folder>
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [sight::data::image_series]: image series to save.
 * @subsection Configuration Configuration
 * - \b folder (optional): path of the folder, if it is not defined, 'open_location_dialog()' should be called to define
 * the path.
 */
class sliced_image_series_writer : public sight::io::service::writer
{
public:

    SIGHT_DECLARE_SERVICE(sliced_image_series_writer, sight::io::service::writer);

    sliced_image_series_writer() noexcept;

    ~sliced_image_series_writer() noexcept override = default;

protected:

    /// Override
    void starting() override;

    /// Override
    void stopping() override;

    /// Override
    void configuring() override;

    /// Override
    void updating() override;

    /// Override
    void info(std::ostream& _sstream) override;

    /// Configure using GUI.
    void open_location_dialog() override;

    /// Return managed file type, here FOLDER
    sight::io::service::path_type_t get_path_type() const override;
};

} // namespace sight::module::io::itk
