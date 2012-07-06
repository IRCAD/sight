/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATAIO_WRITER_GZARRAYWRITER_HPP_
#define _FWDATAIO_WRITER_GZARRAYWRITER_HPP_

#include <boost/shared_ptr.hpp>

#include <fwData/Array.hpp>
#include <fwData/location/SingleFile.hpp>

#include "fwDataIO/config.hpp"
#include "fwDataIO/writer/GenericObjectWriter.hpp"

namespace fwDataIO
{
namespace writer
{

/**
 * @brief   Array Writer. Write file format .raw.gz
 * @class   GzArrayWriter.
 * @author  IRCAD (Research and Development Team).
 * @date    2011
 *
 * Ircad writer to write a ::fwData::Array on filesystem which the file format
 * is .raw.gz. This format is basic. The buffer is written in a file with zlib.
 */
class FWDATAIO_CLASS_API GzArrayWriter :  public GenericObjectWriter< ::fwData::Array >,
                                                public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((GzArrayWriter)(GenericObjectWriter< ::fwData::Array>),
                                           (()),
                                           new  GzArrayWriter
                                           );

    /// Constructor. Do nothing.
    FWDATAIO_API GzArrayWriter( );

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~GzArrayWriter();

    /// Read the file with zlib API.
    FWDATAIO_API virtual void write();

    /// Defines extension supported by this writer ".raw.gz"
    FWDATAIO_API virtual std::string  extension();

};

} // namespace writer
} // namespace fwDataIO


#endif // _FWDATAIO_WRITER_GZARRAYWRITER_HPP_
