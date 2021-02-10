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

#include <io/base/services/IWriter.hpp>

#include <filesystem>

namespace sight::data
{
class Image;
}

namespace sight::modules::io::itk
{

/**
 * @brief Writer for .inr.gz image
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::sight::modules::io::itk::InrImageWriterService">
       <in key="data" uid="..." />
       <file>...</file>
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [sight::data::Image]: image to save.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the file to save, if it not defined, 'openLocationDialog()' should be called to define
 * the path.
 */
class MODULE_IO_ITK_CLASS_API InrImageWriterService : public sight::io::base::services::IWriter
{

public:
    fwCoreServiceMacro(InrImageWriterService, sight::io::base::services::IWriter)

    MODULE_IO_ITK_API InrImageWriterService() noexcept;

    MODULE_IO_ITK_API virtual ~InrImageWriterService() noexcept;

    MODULE_IO_ITK_API static void saveImage( const std::filesystem::path& inrFile, const CSPTR(data::Image)& image );

protected:

    /// Override
    MODULE_IO_ITK_API virtual void starting() override;

    /// Override
    MODULE_IO_ITK_API virtual void stopping() override;

    /// Override
    MODULE_IO_ITK_API virtual void configuring() override;

    /// Override
    MODULE_IO_ITK_API void updating() override;

    /// Override
    MODULE_IO_ITK_API void info(std::ostream& _sstream ) override;

    /**
     * @brief Configure using GUI.
     * @deprecated Will be removed in sight 22.0. Use openLocationDialog() instead.
     */
    [[deprecated("Will be removed in sight 22.0. Use openLocationDialog() instead.")]]
    MODULE_IO_ITK_API virtual void configureWithIHM() override;

    /// configure using GUI.
    MODULE_IO_ITK_API virtual void openLocationDialog() override;

    /// Return managed file type, here FILE
    MODULE_IO_ITK_API sight::io::base::services::IOPathType getIOPathType() const override;

};

} // namespace sight::modules::io::itk
