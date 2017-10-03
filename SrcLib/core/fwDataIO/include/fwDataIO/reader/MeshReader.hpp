/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
