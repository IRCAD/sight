/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioVTK/config.hpp"

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
 * @brief   VTK ImageSeries Reader.
 *
 * Service reading an image series using the fwVtkIO lib.
 *
 * @section XML Configuration
 * @code{.xml}
    <service uid="..." type="::ioVTK::SImageSeriesReader">
        <inout key="data" uid="imageSeries" />
        <file>@path/to/file</file>
    </service>
 * @endcode
 *
 * @subsection In-Out In-Out
 * - \b data [::fwMedData::ImageSeries]: ImageSeries containing read image
 *
 */
class IOVTK_CLASS_API SImageSeriesReader : public ::fwIO::IReader
{

public:

    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignalType;

    /**
     * @brief Constructor. Do nothing.
     */
    IOVTK_API SImageSeriesReader() noexcept;

    ~SImageSeriesReader() noexcept
    {
    }

    fwCoreServiceClassDefinitionsMacro( (SImageSeriesReader)( ::fwIO::IReader) );

    /**
     * @brief Configure the image path.
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
     * This method is used to give informations about the service.
     */
    IOVTK_API void info(std::ostream& _sstream ) override;

private:

    /// This method notifies other ImageSeries services that a new image has been loaded.
    void notificationOfDBUpdate();

    /// Image path.
    ::boost::filesystem::path m_fsImgPath;

    SPTR(JobCreatedSignalType) m_sigJobCreated;
};

} // namespace ioVTK
