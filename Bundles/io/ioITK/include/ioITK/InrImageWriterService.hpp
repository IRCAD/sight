/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioITK/config.hpp"

#include <fwIO/IWriter.hpp>

#include <boost/filesystem/path.hpp>

namespace fwData
{
class Image;
}

namespace ioITK
{

class IOITK_CLASS_API InrImageWriterService : public ::fwIO::IWriter
{

public:
    fwCoreServiceClassDefinitionsMacro( (InrImageWriterService)( ::fwIO::IWriter) );

    IOITK_API InrImageWriterService() noexcept;

    IOITK_API virtual ~InrImageWriterService() noexcept;

    IOITK_API static void saveImage( const ::boost::filesystem::path& inrFile, const SPTR(::fwData::Image)& image );

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

    /// Return managed file type, here FILE
    IOITK_API ::fwIO::IOPathType getIOPathType() const override;

};

} // namespace ioITK
