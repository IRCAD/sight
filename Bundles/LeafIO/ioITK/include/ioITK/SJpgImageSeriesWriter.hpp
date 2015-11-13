/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOITK_SJPGIMAGESERIESWRITER_HPP__
#define __IOITK_SJPGIMAGESERIESWRITER_HPP__

#include <string>
#include <boost/filesystem/path.hpp>

#include <io/IWriter.hpp>

#include "ioITK/config.hpp"

namespace ioITK
{

/**
 * @brief Write an image to jpg format
 **/
class IOITK_CLASS_API SJpgImageSeriesWriter : public ::io::IWriter
{

public:
    fwCoreServiceClassDefinitionsMacro ( (SJpgImageSeriesWriter)( ::io::IWriter) );

    IOITK_API SJpgImageSeriesWriter() throw();

    IOITK_API virtual ~SJpgImageSeriesWriter() throw();

protected:

    /// Override
    IOITK_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOITK_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOITK_API void updating() throw(::fwTools::Failed);

    /// Override
    IOITK_API void info(std::ostream &_sstream );

    /// Override
    IOITK_API virtual void configureWithIHM();

    /// Return managed file type, here FOLDER
    IOITK_API ::io::IOPathType getIOPathType() const;

};

} // namespace ioITK

#endif //__IOITK_SJPGIMAGESERIESWRITER_HPP__

