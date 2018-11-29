/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWDATAIO_READER_GZBUFFERIMAGEREADER_HPP__
#define __FWDATAIO_READER_GZBUFFERIMAGEREADER_HPP__

#include "fwDataIO/config.hpp"
#include "fwDataIO/reader/GenericObjectReader.hpp"

#include <fwData/Image.hpp>
#include <fwData/location/SingleFile.hpp>

#include <boost/filesystem/path.hpp>

namespace fwDataIO
{

namespace reader
{

/**
 * @brief   Image Reader. Read file format .raw.gz
 *
 *
 * Ircad reader to read a ::fwData::Image on filesystem which the file format
 * is .raw.gz. This format is basic. The buffer is writen in a file with zlib.
 */
class FWDATAIO_CLASS_API GzBufferImageReader :  public GenericObjectReader< ::fwData::Image >,
                                                public ::fwData::location::enableSingleFile< IObjectReader >
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((GzBufferImageReader)(GenericObjectReader< ::fwData::Image>),
                                           (()),
                                           ::fwDataIO::reader::factory::New< GzBufferImageReader >
                                           );

    /// Constructor. Do nothing.
    FWDATAIO_API GzBufferImageReader(::fwDataIO::reader::IObjectReader::Key key);

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~GzBufferImageReader();

    /// Read the file with zlib API.
    FWDATAIO_API virtual void read() override;

    /// Defines extension supported by this reader ".raw.gz"
    FWDATAIO_API std::string extension() override;

};

} // namespace reader

} // namespace fwDataIO

#endif // __FWDATAIO_READER_GZBUFFERIMAGEREADER_HPP__
