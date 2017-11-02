/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWITKIO_JPGIMAGEWRITER_HPP__
#define __FWITKIO_JPGIMAGEWRITER_HPP__

#include "fwItkIO/config.hpp"

#include <fwData/Image.hpp>
#include <fwData/location/Folder.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include <fwTools/ProgressAdviser.hpp>

#include <boost/filesystem/path.hpp>

namespace fwItkIO
{

class JpgImageWriter :  public ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image >,
                        public ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >,
                        public ::fwTools::ProgressAdviser
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((JpgImageWriter)( ::fwDataIO::writer::GenericObjectWriter< ::fwData::Image>),
                                           (()),
                                           ::fwDataIO::writer::factory::New< JpgImageWriter >
                                           );
    fwCoreAllowSharedFromThis();

    FWITKIO_API JpgImageWriter(::fwDataIO::writer::IObjectWriter::Key key);

    FWITKIO_API ~JpgImageWriter();

    FWITKIO_API void write() override;

    FWITKIO_API std::string extension() override;
};

} // namespace fwItkIO

#endif // __FWITKIO_JPGIMAGEWRITER_HPP__
