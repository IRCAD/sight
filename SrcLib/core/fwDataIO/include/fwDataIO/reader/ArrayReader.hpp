/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATAIO_READER_ARRAYREADER_HPP__
#define __FWDATAIO_READER_ARRAYREADER_HPP__

#include "fwDataIO/config.hpp"
#include "fwDataIO/reader/GenericObjectReader.hpp"

#include <fwData/Array.hpp>
#include <fwData/location/SingleFile.hpp>

#include <boost/filesystem/path.hpp>

namespace fwDataIO
{
namespace reader
{

/**
 * @brief   Array Reader. Read file format .raw
 *
 *
 * Ircad reader to read a ::fwData::Array on filesystem in a raw format.
 */
class FWDATAIO_CLASS_API ArrayReader :  public GenericObjectReader< ::fwData::Array >,
                                        public ::fwData::location::enableSingleFile< IObjectReader >
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((ArrayReader)(GenericObjectReader< ::fwData::Array>),
                                           (()),
                                           ::fwDataIO::reader::factory::New<ArrayReader>
                                           );

    /// Constructor. Do nothing.
    FWDATAIO_API ArrayReader(::fwDataIO::reader::IObjectReader::Key key);

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~ArrayReader();

    /// Read the file with zlib API.
    FWDATAIO_API virtual void read() override;

    /// Defines extension supported by this reader ".raw"
    FWDATAIO_API std::string extension() override;

};

} // namespace reader
} // namespace fwDataIO

#endif // __FWDATAIO_READER_ARRAYREADER_HPP__
