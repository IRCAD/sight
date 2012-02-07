/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATAIO_READER_ARRAYREADER_HPP_
#define _FWDATAIO_READER_ARRAYREADER_HPP_

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
 * @brief   Array Reader. Read file format .raw
 * @class   ArrayReader.
 * @author  IRCAD (Research and Development Team).
 * @date    2011
 *
 * Ircad reader to read a ::fwData::Array on filesystem in a raw format.
 */
class FWDATAIO_CLASS_API ArrayReader :  public GenericObjectReader< ::fwData::Array >,
                                                public ::fwData::location::enableSingleFile< IObjectReader >
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((ArrayReader)(GenericObjectReader< ::fwData::Array>),
                                           (()),
                                           new  ArrayReader
                                           );

    /// Constructor. Do nothing.
    FWDATAIO_API ArrayReader();

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~ArrayReader();

    /// Read the file with zlib API.
    FWDATAIO_API virtual void read();

    /// Defines extension supported by this reader ".raw"
    FWDATAIO_API std::string extension();

};

} // namespace reader
} // namespace fwDataIO


#endif // _FWDATAIO_READER_ARRAYREADER_HPP_
