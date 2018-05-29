/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioVTK/config.hpp"

#include <fwData/location/ILocation.hpp>
#include <fwData/Mesh.hpp>

#include <fwIO/IReader.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace fwMedData
{
class SeriesDB;
}

namespace fwJobs
{
class IJob;
}

namespace ioVTK
{

/**
 * @brief   VTK File Reader.
 *
 * Service reading a VTK file (mesh or image) using the fwVtkIO lib.
 *
 * @section Signals Signals
 * - \b jobCreated(SPTR(::fwJobs::IJob)): emitted when the image is loading to display a progress bar.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioVTK::SSeriesDBReader">
      <inout key="data" uid="..." />
      <file>...</file>
      <file>...</file>
      <file>...</file>
   </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b data [::fwMedData::SeriesDB]: seriesDB to load.
 * @subsection Configuration Configuration
 * - \b file (optional): path of the files to load, if it not defined, 'configureWithIHM()' should be called to define
 * the path.
 */
class IOVTK_CLASS_API SSeriesDBReader : public ::fwIO::IReader
{

public:
    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignalType;

    /**
     * @brief Constructor. Do nothing.
     */
    IOVTK_API SSeriesDBReader() noexcept;

    ~SSeriesDBReader() noexcept
    {
    }

    fwCoreServiceClassDefinitionsMacro( (SSeriesDBReader)( ::fwIO::IReader) );

    /**
     * @brief Configure the vtk file path.
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
     * @brief Load VTK mesh or image and push it into SeriesDB.
     * @param[in] vtkFiles vector of boost::filesystem::path.
     * @param[out] seriesDB std::shared_ptr< ::fwMedData::SeriesDB >.
     * @return bool.
     *
     * This method is used to load a mesh using the file path.
     */
    void loadSeriesDB( const ::fwData::location::ILocation::VectPathType& vtkFiles,
                       const SPTR(::fwMedData::SeriesDB)& seriesDB );

    SPTR(JobCreatedSignalType) m_sigJobCreated;

};

} // namespace ioVTK
