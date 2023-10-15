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

#include "modules/io/itk/config.hpp"

#include <io/__/service/writer.hpp>

#include <filesystem>
#include <string>

namespace sight::module::io::itk
{

/**
 * @brief Writer for .inr.gz image series
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::itk::image_series_writer">
       <in key="data" uid="..." />
       <file>...</file>
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [sight::data::image_series]: image series to save.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the file to save, if it not defined, 'openLocationDialog()' should be called to define
 * the path.
 */
class MODULE_IO_ITK_CLASS_API image_series_writer : public sight::io::service::writer
{
public:

    SIGHT_DECLARE_SERVICE(image_series_writer, sight::io::service::writer);

    MODULE_IO_ITK_API image_series_writer() noexcept;

    MODULE_IO_ITK_API ~image_series_writer() noexcept override;

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
    MODULE_IO_ITK_API void info(std::ostream& _sstream) override;

    /// Configure using GUI.
    MODULE_IO_ITK_API void openLocationDialog() override;

    /// Return managed file type, here FILE
    MODULE_IO_ITK_API sight::io::service::IOPathType getIOPathType() const override;
};

} // namespace sight::module::io::itk
