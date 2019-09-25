/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "ioITK/config.hpp"

#include <fwIO/IWriter.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace fwData
{
class Image;
}

namespace ioITK
{

/**
 * @brief Write an image to jpg format
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioITK::JpgImageWriterService">
       <in key="data" uid="..." />
       <folder>...</folder>
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [::fwData::Image]: image to save.
 * @subsection Configuration Configuration
 * - \b folder (optional): path of the folder, if it is not defined, 'configureWithIHM()' should be called to define
 * the path.
 */
class IOITK_CLASS_API JpgImageWriterService : public ::fwIO::IWriter
{

public:
    fwCoreServiceMacro(JpgImageWriterService,  ::fwIO::IWriter);

    IOITK_API JpgImageWriterService() noexcept;

    IOITK_API virtual ~JpgImageWriterService() noexcept;

    IOITK_API static void saveImage(
        const ::boost::filesystem::path& imgPath,
        const CSPTR(::fwData::Image)& img);

protected:

    /// Override
    IOITK_API virtual void starting() override;

    /// Override
    IOITK_API virtual void stopping() override;

    /// Override
    IOITK_API virtual void configuring() override;

    /// Override
    IOITK_API void updating() override;

    /// Override
    IOITK_API void info(std::ostream& _sstream ) override;

    /// Override
    IOITK_API virtual void configureWithIHM() override;

    /// Return managed file type, here FOLDER
    IOITK_API ::fwIO::IOPathType getIOPathType() const override;

};

} // namespace ioITK
