/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATAIO_WRITER_ARRAYWRITER_HPP_
#define _FWDATAIO_WRITER_ARRAYWRITER_HPP_

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
 * @brief   Array Writer. Write file format .raw
 * @class   ArrayWriter.
 * @author  IRCAD (Research and Development Team).
 * @date    2011
 *
 * Ircad writer to write a ::fwData::Array on filesystem in a raw format
 */
class FWDATAIO_CLASS_API ArrayWriter :  public GenericObjectWriter< ::fwData::Array >,
                                                public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((ArrayWriter)(GenericObjectWriter< ::fwData::Array>),
                                           (()),
                                           new  ArrayWriter
                                           );

    /// Constructor. Do nothing.
    FWDATAIO_API ArrayWriter( );

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~ArrayWriter();

    /// Read the file with zlib API.
    FWDATAIO_API virtual void write();

    /// Defines extension supported by this writer ".raw"
    FWDATAIO_API virtual std::string  extension();

};

} // namespace writer
} // namespace fwDataIO


#endif // _FWDATAIO_WRITER_ARRAYWRITER_HPP_
