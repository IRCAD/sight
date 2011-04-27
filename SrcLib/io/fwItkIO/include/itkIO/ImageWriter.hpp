/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWITKIO_IMAGEWRITER_HPP_
#define _FWITKIO_IMAGEWRITER_HPP_

#include <boost/filesystem/path.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwTools/ProgressAdviser.hpp>
#include <fwData/Image.hpp>

#include "itkIO/config.hpp"

namespace itkIO
{

class ImageWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image >,
                    public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >,
                    public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((ImageWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image>),
                                            (()),
                                            new  ImageWriter
                                           );
    fwCoreAllowSharedFromThis();

    FWITKIO_API void write();

    FWITKIO_API std::string extension();

protected:

    FWITKIO_API ImageWriter();

    FWITKIO_API ~ImageWriter();
};

} // namespace itkIO

#endif // _FWITKIO_IMAGEWRITER_HPP_
