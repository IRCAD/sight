/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include <fwCore/macros.hpp>

#include <fwIO/IReader.hpp>

#include <boost/filesystem/path.hpp>

namespace fwData
{
class Image;
}

namespace ioITK
{
/**
 * @brief Reader for .inr.gz image
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioITK::InrImageReaderService">
       <inout key="data" uid="..." />
       <file>...</file>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [::fwData::Image]: loaded image.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the image to load, if it is not defined, 'configureWithIHM()' should be called to
 * define the path.
 */
class IOITK_CLASS_API InrImageReaderService : public ::fwIO::IReader
{

public:
    fwCoreServiceClassDefinitionsMacro( (InrImageReaderService)( ::fwIO::IReader) );

    IOITK_API InrImageReaderService() noexcept;

    IOITK_API virtual ~InrImageReaderService() noexcept;

protected:

    /// Override
    virtual void starting() override
    {
    }

    /// Override
    virtual void stopping() override
    {
    }

    /// Override
    IOITK_API virtual void configuring() override;

    /// Override
    IOITK_API virtual void updating() override;

    /// Override
    IOITK_API void info(std::ostream& _sstream ) override;

    /// Override
    IOITK_API virtual void configureWithIHM() override;

    /// Return managed file type, here FILE
    IOITK_API ::fwIO::IOPathType getIOPathType() const override;

private:

    void notificationOfDBUpdate();

    bool createImage( const ::boost::filesystem::path& inrFileDir, const SPTR(::fwData::Image)& _pImg );
};

} // namespace ioITK
