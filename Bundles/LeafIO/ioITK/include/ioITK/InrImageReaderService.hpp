/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOITK_INRIMAGEREADERSERVICE_HPP__
#define __IOITK_INRIMAGEREADERSERVICE_HPP__

#include "ioITK/config.hpp"

#include <fwCore/macros.hpp>

#include <io/IReader.hpp>

#include <boost/filesystem/path.hpp>

namespace fwData
{
class Image;
}

namespace ioITK
{

class IOITK_CLASS_API InrImageReaderService : public ::io::IReader
{

public:
    fwCoreServiceClassDefinitionsMacro( (InrImageReaderService)( ::io::IReader) );

    IOITK_API InrImageReaderService() noexcept;

    IOITK_API virtual ~InrImageReaderService() noexcept;

protected:

    /// Override
    virtual void starting()
    {
    }

    /// Override
    virtual void stopping()
    {
    }

    /// Override
    IOITK_API virtual void configuring();

    /// Override
    IOITK_API virtual void updating();

    /// Override
    IOITK_API void info(std::ostream& _sstream );

    /// Override
    IOITK_API virtual void configureWithIHM();

    /// Return managed file type, here FILE
    IOITK_API ::io::IOPathType getIOPathType() const;

private:

    void notificationOfDBUpdate();

    bool createImage( const ::boost::filesystem::path& inrFileDir, const SPTR(::fwData::Image)& _pImg );
};

} // namespace ioITK

#endif //__IOITK_INRIMAGEREADERSERVICE_HPP__

