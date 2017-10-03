/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATAIO_READER_TAGREADER_HPP__
#define __FWDATAIO_READER_TAGREADER_HPP__

#include "fwDataIO/config.hpp"
#include "fwDataIO/reader/GenericObjectReader.hpp"

#include <fwData/location/SingleFile.hpp>

#include <boost/filesystem/path.hpp>

namespace fwData
{
class Tag;
}

namespace fwDataIO
{

namespace reader
{

/**
 * @brief   Tag Reader. Read file format .tag
 *
 *
 * Ircad reader to read a ::fwData::Tag on filesystem which the file format
 * is .tag.
 */
class FWDATAIO_CLASS_API TagReader :
    public GenericObjectReader< ::fwData::Tag >,
    public ::fwData::location::enableSingleFile< IObjectReader >
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (TagReader)(GenericObjectReader< ::fwData::Tag >),
                                            (()),
                                            ::fwDataIO::reader::factory::New< TagReader >
                                            );
    /// Constructor. Do nothing.
    FWDATAIO_API TagReader(::fwDataIO::reader::IObjectReader::Key key);

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~TagReader();

    /// Read the file .tag.
    FWDATAIO_API virtual void read() override;

    /// Defines extension supported by this reader ".tag"
    FWDATAIO_API std::string extension() override;

};

} // namespace reader

} // namespace fwDataIO

#endif // __FWDATAIO_READER_TAGREADER_HPP__
