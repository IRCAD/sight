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

#ifndef __FWDATAIO_READER_MESHREADER_HPP__
#define __FWDATAIO_READER_MESHREADER_HPP__

#include "fwDataIO/config.hpp"
#include "fwDataIO/reader/GenericObjectReader.hpp"

#include <fwData/location/SingleFile.hpp>
#include <fwData/Mesh.hpp>

#include <boost/filesystem/path.hpp>

namespace fwData
{
class Mesh;
}

namespace fwDataIO
{

namespace reader
{

/**
 * @brief   Mesh reader. Read file format .trian
 *
 *
 * Load trian file into fwData::Mesh
 */
class FWDATAIO_CLASS_API MeshReader :
    public GenericObjectReader< ::fwData::Mesh>,
    public ::fwData::location::enableSingleFile< IObjectReader >
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (MeshReader)(GenericObjectReader< ::fwData::Mesh >),
                                            (()),
                                            ::fwDataIO::reader::factory::New< MeshReader >
                                            );

    /// Constructor. Do nothing.
    FWDATAIO_API MeshReader(::fwDataIO::reader::IObjectReader::Key key);

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~MeshReader();

    /// Read the file with standard iostream API.
    FWDATAIO_API virtual void read() override;

    FWDATAIO_API std::string extension() override;

};

} // namespace reader

} // namespace fwDataIO

#endif // __FWDATAIO_READER_MESHREADER_HPP__
