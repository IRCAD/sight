/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWITKIO_IMAGEREADER_HPP_
#define _FWITKIO_IMAGEREADER_HPP_

#include <boost/filesystem/path.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwData/Image.hpp>

#include <fwTools/ProgressAdviser.hpp>

#include "itkIO/config.hpp"

namespace itkIO
{

class ImageReader : public ::fwDataIO::reader::GenericObjectReader< ::fwData::Image >,
                    public ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >,
                    public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((ImageReader)(::fwDataIO::reader::GenericObjectReader< ::fwData::Image>),
                                            (()),
                                            new  ImageReader
                                           );
    fwCoreAllowSharedFromThis();

    FWITKIO_API void read();

protected :

    FWITKIO_API ImageReader();

    FWITKIO_API ~ImageReader();
};

} // namespace itkIO

#endif // _FWITKIO_IMAGEREADER_HPP_
