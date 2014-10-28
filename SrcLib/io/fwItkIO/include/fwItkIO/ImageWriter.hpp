/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWITKIO_IMAGEWRITER_HPP_
#define _FWITKIO_IMAGEWRITER_HPP_

#include <fwDataIO/writer/GenericObjectWriter.hpp>
#include <fwData/location/SingleFile.hpp>
#include <fwTools/ProgressAdviser.hpp>
#include <fwData/Image.hpp>

#include "fwItkIO/config.hpp"

namespace fwItkIO
{

class ImageWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image >,
                    public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >,
                    public ::fwTools::ProgressAdviser
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((ImageWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image>),
                                            (()),
                                            ::fwDataIO::writer::factory::New< ImageWriter >
                                           );
    fwCoreAllowSharedFromThis();

    FWITKIO_API ImageWriter(::fwDataIO::writer::IObjectWriter::Key key);

    FWITKIO_API ~ImageWriter();

    FWITKIO_API void write();

    FWITKIO_API std::string extension();
};

} // namespace fwItkIO

#endif // _FWITKIO_IMAGEWRITER_HPP_
