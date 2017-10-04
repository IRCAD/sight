/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWITKIO_IMAGEWRITER_HPP__
#define __FWITKIO_IMAGEWRITER_HPP__

#include "fwItkIO/config.hpp"

#include <fwData/Image.hpp>
#include <fwData/location/SingleFile.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include <fwTools/ProgressAdviser.hpp>

namespace fwItkIO
{

class ImageWriter : public ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image >,
                    public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >,
                    public ::fwTools::ProgressAdviser
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((ImageWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image>),
                                           (()),
                                           ::fwDataIO::writer::factory::New< ImageWriter >
                                           );
    fwCoreAllowSharedFromThis();

    FWITKIO_API ImageWriter(::fwDataIO::writer::IObjectWriter::Key key);

    FWITKIO_API ~ImageWriter();

    FWITKIO_API void write() override;

    FWITKIO_API std::string extension() override;
};

} // namespace fwItkIO

#endif // __FWITKIO_IMAGEWRITER_HPP__
