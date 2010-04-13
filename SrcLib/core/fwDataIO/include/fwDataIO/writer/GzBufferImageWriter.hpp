/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATAIO_WRITER_GZBUFFER_HPP_
#define _FWDATAIO_WRITER_GZBUFFER_HPP_

#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>

#include <fwData/Image.hpp>
#include <fwData/location/SingleFile.hpp>

#include "fwDataIO/config.hpp"
#include "fwDataIO/writer/GenericObjectWriter.hpp"

namespace fwDataIO
{

namespace writer
{

/**
 * @brief   Image Writer. Write file format .raw.gz
 * @class   GzBufferImageWriter.
 * @author  IRCAD (Research and Development Team).
 * @date    2009
 *
 * Ircad writer to write a ::fwData::Image on filesystem which the file format
 * is .raw.gz. This format is basic. The buffer is writen in a file with zlib.
 */
class FWDATAIO_CLASS_API GzBufferImageWriter :  public GenericObjectWriter< ::fwData::Image >,
                                                public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >
{

public :

    fwCoreClassDefinitionsWithFactoryMacro(     (GzBufferImageWriter)(GenericObjectWriter< ::fwData::Image>),
                                                     (()),
                                                     new  GzBufferImageWriter
                                                );

    /// Constructor. Do nothing.
    FWDATAIO_API GzBufferImageWriter( );

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~GzBufferImageWriter();

    /// Read the file with zlib API.
    FWDATAIO_API virtual void write();

    /// Defines extension supported by this writer ".raw.gz"
    FWDATAIO_API virtual std::string  extension();

};

} // namespace writer

} // namespace fwDataIO


#endif // _FWDATAIO_WRITER_GZBUFFER_HPP_
