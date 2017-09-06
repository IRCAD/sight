/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOITK_JPGIMAGEWRITERSERVICE_HPP__
#define __IOITK_JPGIMAGEWRITERSERVICE_HPP__

#include "ioITK/config.hpp"

#include <io/IWriter.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace fwData
{
class Image;
}

namespace ioITK
{

class IOITK_CLASS_API JpgImageWriterService : public ::io::IWriter
{

public:
    fwCoreServiceClassDefinitionsMacro( (JpgImageWriterService)( ::io::IWriter) );

    IOITK_API JpgImageWriterService() noexcept;

    IOITK_API virtual ~JpgImageWriterService() noexcept;

    IOITK_API static void saveImage(
        const ::boost::filesystem::path& imgPath,
        const SPTR(::fwData::Image)& img);

protected:

    /// Override
    IOITK_API virtual void starting();

    /// Override
    IOITK_API virtual void stopping();

    /// Override
    IOITK_API virtual void configuring();

    /// Override
    IOITK_API void updating();

    /// Override
    IOITK_API void info(std::ostream& _sstream );

    /// Override
    IOITK_API virtual void configureWithIHM();

    /// Return managed file type, here FOLDER
    IOITK_API ::io::IOPathType getIOPathType() const;

};

} // namespace ioITK

#endif //__IOITK_JPGIMAGEWRITERSERVICE_HPP__

