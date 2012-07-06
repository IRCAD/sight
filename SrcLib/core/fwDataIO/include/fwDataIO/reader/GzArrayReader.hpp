/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATAIO_READER_GZARRAYREADER_HPP_
#define _FWDATAIO_READER_GZARRAYREADER_HPP_

#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>

#include <fwData/Array.hpp>
#include <fwData/location/SingleFile.hpp>

#include "fwDataIO/config.hpp"
#include "fwDataIO/reader/GenericObjectReader.hpp"

namespace fwDataIO
{
namespace reader
{

/**
 * @brief   Array Reader. Read file format .raw.gz
 * @class   GzArrayReader.
 * @author  IRCAD (Research and Development Team).
 * @date    2011
 *
 * Ircad reader to read a ::fwData::Array on filesystem which the file format
 * is .raw.gz. This format is basic. The buffer is writen in a file with zlib.
 */
class FWDATAIO_CLASS_API GzArrayReader :  public GenericObjectReader< ::fwData::Array >,
                                                public ::fwData::location::enableSingleFile< IObjectReader >
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((GzArrayReader)(GenericObjectReader< ::fwData::Array>),
                                           (()),
                                           new  GzArrayReader
                                           );

    /// Constructor. Do nothing.
    FWDATAIO_API GzArrayReader();

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~GzArrayReader();

    /// Read the file with zlib API.
    FWDATAIO_API virtual void read();

    /// Defines extension supported by this reader ".raw.gz"
    FWDATAIO_API std::string extension();

};

} // namespace reader
} // namespace fwDataIO


#endif // _FWDATAIO_READER_GZARRAYREADER_HPP_
