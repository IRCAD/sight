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

#include <io/base/service/IWriter.hpp>

#include <filesystem>
#include <string>

namespace sight::data
{
class Image;
}

namespace sight::module::io::itk
{

/**
 * @brief Write an image to jpg format
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::itk::JpgImageWriterService">
       <in key="data" uid="..." />
       <folder>...</folder>
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [sight::data::Image]: image to save.
 * @subsection Configuration Configuration
 * - \b folder (optional): path of the folder, if it is not defined, 'openLocationDialog()' should be called to define
 * the path.
 */
class MODULE_IO_ITK_CLASS_API JpgImageWriterService : public sight::io::base::service::IWriter
{

public:
    SIGHT_DECLARE_SERVICE(JpgImageWriterService, sight::io::base::service::IWriter)

    MODULE_IO_ITK_API JpgImageWriterService() noexcept;

    MODULE_IO_ITK_API virtual ~JpgImageWriterService() noexcept;

    MODULE_IO_ITK_API static void saveImage(
        const std::filesystem::path& imgPath,
        const CSPTR(data::Image)& img);

protected:

    /// Override
    MODULE_IO_ITK_API void starting() override;

    /// Override
    MODULE_IO_ITK_API void stopping() override;

    /// Override
    MODULE_IO_ITK_API void configuring() override;

    /// Override
    MODULE_IO_ITK_API void updating() override;

    /// Override
    MODULE_IO_ITK_API void info(std::ostream& _sstream ) override;

    /**
     * @brief Configure using GUI.
     * @deprecated Will be removed in sight 22.0. Use openLocationDialog() instead.
     */
    [[deprecated("Will be removed in sight 22.0. Use openLocationDialog() instead.")]]
    MODULE_IO_ITK_API void configureWithIHM() override;

    /// Configure using GUI.
    MODULE_IO_ITK_API void openLocationDialog() override;

    /// Return managed file type, here FOLDER
    MODULE_IO_ITK_API sight::io::base::service::IOPathType getIOPathType() const override;

};

} // namespace sight::module::io::itk
