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

namespace sight::data
{

class image;

} // namespace sight::data

namespace sight::module::io::itk
{

/**
 * @brief Writer for .inr.gz image
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::itk::image_writer">
       <in key="data" uid="..." />
       <file>...</file>
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [sight::data::image]: image to save.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the file to save, if it not defined, 'open_location_dialog()' should be called to
 * define
 * the path.
 */
class image_writer : public sight::io::service::writer
{
public:

    SIGHT_DECLARE_SERVICE(image_writer, sight::io::service::writer);

    image_writer() noexcept;

    ~image_writer() noexcept override = default;

    static bool save_image(const std::filesystem::path& _img_save_path, const CSPTR(data::image)& _image);

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

    /// configure using GUI.
    void open_location_dialog() override;

    /// Return managed file type, here FILE
    sight::io::service::path_type_t get_path_type() const override;
};

} // namespace sight::module::io::itk
