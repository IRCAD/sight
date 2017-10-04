/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWVTKIO_MESHREADER_HPP__
#define __FWVTKIO_MESHREADER_HPP__

#include "fwVtkIO/config.hpp"

#include <fwData/location/SingleFile.hpp>
#include <fwData/Mesh.hpp>

#include <fwDataIO/reader/GenericObjectReader.hpp>

#include <boost/filesystem/path.hpp>

namespace fwJobs
{
class Observer;
class IJob;
}

namespace fwVtkIO
{

/**
 * @brief   Read a mesh.
 *
 * Read a VTK Mesh using the VTK lib
 */

class MeshReader : public ::fwDataIO::reader::GenericObjectReader< ::fwData::Mesh >,
                   public ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((MeshReader)( ::fwDataIO::reader::GenericObjectReader< ::fwData::Mesh >),
                                           (()),
                                           ::fwDataIO::reader::factory::New< MeshReader >
                                           );
    fwCoreAllowSharedFromThis();

    //! @brief Constructor.
    FWVTKIO_API MeshReader(::fwDataIO::reader::IObjectReader::Key key);

    //! @brief Destructor.
    FWVTKIO_API ~MeshReader();

    //! @brief Reading operator.
    FWVTKIO_API void read() override;

    /// @return ".vtk"
    FWVTKIO_API std::string extension() override;
    /// @return internal job
    FWVTKIO_API SPTR(::fwJobs::IJob) getJob() const override;

private:

    ///Internal job
    SPTR(::fwJobs::Observer) m_job;

};

} // namespace fwVtkIO

#endif // __FWVTKIO_MESHREADER_HPP__
