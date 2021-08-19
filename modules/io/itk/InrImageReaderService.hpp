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

#include <io/base/service/IReader.hpp>

#include <filesystem>

namespace sight::data
{

class Image;

}

namespace sight::module::io::itk
{

/**
 * @brief Reader for .inr.gz image
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="sight::module::io::itk::InrImageReaderService">
       <inout key="data" uid="..." />
       <file>...</file>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [sight::data::Image]: loaded image.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the image to load, if it is not defined, 'openLocationDialog()' should be called to
 * define the path.
 */
class MODULE_IO_ITK_CLASS_API InrImageReaderService : public sight::io::base::service::IReader
{
public:

    SIGHT_DECLARE_SERVICE(InrImageReaderService, sight::io::base::service::IReader);

    MODULE_IO_ITK_API InrImageReaderService() noexcept;

    MODULE_IO_ITK_API virtual ~InrImageReaderService() noexcept;

protected:

    /// Override
    void starting() override
    {
    }

    /// Override
    void stopping() override
    {
    }

    /// Override
    MODULE_IO_ITK_API void configuring() override;

    /// Override
    MODULE_IO_ITK_API void updating() override;

    /// Override
    MODULE_IO_ITK_API void info(std::ostream& _sstream) override;

    /// Configure using GUI.
    MODULE_IO_ITK_API void openLocationDialog() override;

    /// Return managed file type, here FILE
    MODULE_IO_ITK_API sight::io::base::service::IOPathType getIOPathType() const override;

private:

    void notificationOfDBUpdate();

    bool createImage(const std::filesystem::path& inrFileDir, const SPTR(data::Image)& _pImg);
};

} // namespace sight::module::io::itk
