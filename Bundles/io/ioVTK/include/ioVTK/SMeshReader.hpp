/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioVTK/config.hpp"

#include <fwData/Mesh.hpp>

#include <fwIO/IReader.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace fwJobs
{
class IJob;
}

namespace ioVTK
{

/**
 * @brief   VTK Mesh Reader.
 *
 * Service reading a VTK mesh using the fwVtkIO lib.
 * @section XML Configuration
 * @code{.xml}
    <service uid="..." type="::ioVTK::SMeshReader">
        <inout key="output" uid="mesh" />
        <file>@path/to/file</file>
    </service>
 * @endcode
 *
 * @subsection In-Out In-Out
 * - \b seriesDB [::fwMedData::SeriesDB]: SeriesDB
 *
 */
class IOVTK_CLASS_API SMeshReader : public ::fwIO::IReader
{

public:
    virtual ~SMeshReader() noexcept
    {
    }

    fwCoreServiceClassDefinitionsMacro( (SMeshReader)( ::fwIO::IReader) );

    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignalType;

    /// Constructor
    IOVTK_API SMeshReader() noexcept;

    /**
     * @brief Configure the image path.
     *
     * This method is used to find
     * the file path  using a file selector.
     */
    IOVTK_API virtual void configureWithIHM() override;

protected:

    IOVTK_API virtual ::fwIO::IOPathType getIOPathType() const override;

    /**
     * @brief Starting method.
     *
     * This method is used to initialize the service.
     */
    IOVTK_API virtual void starting() override;

    /**
     * @brief Stopping method.
     *
     * The stopping method is empty for this service.
     */
    IOVTK_API virtual void stopping() override;

    /**
     * @brief Configuring method.
     *
     * The configuring method only calls the configuring method from the base class
     */
    IOVTK_API virtual void configuring() override;

    /**
     * @brief Updating method.
     *
     * This method is used to update the service.
     * The image is read.
     */
    IOVTK_API void updating() override;

    /**
     * @brief Info method.
     *
     * This method is used to give
     * informations about the service.
     */
    IOVTK_API void info(std::ostream& _sstream ) override;

private:

    /**
     * @brief Load a VTK mesh.
     * @param[in] _vtkFile ::boost::filesystem::path.
     * @param[out] _pMesh std::shared_ptr< ::fwData::Mesh >.
     * @return bool.
     *
     * This method is used to load a mesh using the file path.
     */
    void loadMesh( const ::boost::filesystem::path _vtkFile, ::fwData::Mesh::sptr _pMesh );

    /**
     * @brief Notification method.
     *
     * This method is used to notify
     * the mesh services.
     */
    void notificationOfUpdate();

    /**
     * @brief the m_bServiceIsConfigured value is \b true
     * if the mesh path is known.
     */
    bool m_bServiceIsConfigured;

    /**
     * @brief Mesh path.
     */
    ::boost::filesystem::path m_fsMeshPath;

    /// Signal triggered when job created
    SPTR(JobCreatedSignalType) m_sigJobCreated;
};

} // namespace ioVTK
