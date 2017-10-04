/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATAIO_WRITER_GZBUFFERIMAGEWRITER_HPP__
#define __FWDATAIO_WRITER_GZBUFFERIMAGEWRITER_HPP__

#include "fwDataIO/config.hpp"
#include "fwDataIO/writer/GenericObjectWriter.hpp"

#include <fwData/Image.hpp>
#include <fwData/location/SingleFile.hpp>

#include <boost/filesystem/path.hpp>

namespace fwDataIO
{

namespace writer
{

/**
 * @brief   Image Writer. Write file format .raw.gz
 *
 *
 * Ircad writer to write a ::fwData::Image on filesystem which the file format
 * is .raw.gz. This format is basic. The buffer is writen in a file with zlib.
 */
class FWDATAIO_CLASS_API GzBufferImageWriter :  public GenericObjectWriter< ::fwData::Image >,
                                                public ::fwData::location::enableSingleFile< ::fwDataIO::writer::
                                                                                             IObjectWriter >
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((GzBufferImageWriter)(GenericObjectWriter< ::fwData::Image>),
                                           (()),
                                           ::fwDataIO::writer::factory::New< GzBufferImageWriter >
                                           );

    /// Constructor. Do nothing.
    FWDATAIO_API GzBufferImageWriter(::fwDataIO::writer::IObjectWriter::Key key);

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~GzBufferImageWriter();

    /// Read the file with zlib API.
    FWDATAIO_API virtual void write() override;

    /// Defines extension supported by this writer ".raw.gz"
    FWDATAIO_API virtual std::string extension() override;

};

} // namespace writer

} // namespace fwDataIO

#endif // __FWDATAIO_WRITER_GZBUFFERIMAGEWRITER_HPP__
