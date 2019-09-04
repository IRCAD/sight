/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

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
