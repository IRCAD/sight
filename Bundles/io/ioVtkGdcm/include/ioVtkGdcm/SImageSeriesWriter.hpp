/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "ioVtkGdcm/config.hpp"

#include <fwIO/IWriter.hpp>

#include <boost/filesystem/path.hpp>

#include <string>

namespace fwMedData
{
class ImageSeries;
}

namespace fwJobs
{
class IJob;
}

namespace ioVtkGdcm
{

/**
 * @brief Services to write an ImageSeries in DICOM format.
 */
class IOVTKGDCM_CLASS_API SImageSeriesWriter : public ::fwIO::IWriter
{

public:
    fwCoreServiceClassDefinitionsMacro( (SImageSeriesWriter)( ::fwIO::IWriter) );

    typedef ::fwCom::Signal< void ( SPTR(::fwJobs::IJob) ) > JobCreatedSignalType;

    /**
     * @brief   constructor
     */
    IOVTKGDCM_API SImageSeriesWriter() noexcept;

    /**
     * @brief   destructor
     */
    IOVTKGDCM_API virtual ~SImageSeriesWriter() noexcept;

    /// Propose select a directory where to save the DICOM files.
    IOVTKGDCM_API virtual void configureWithIHM() override;

protected:

    /// Does nothing
    IOVTKGDCM_API virtual void starting() override;

    /// Does nothing
    IOVTKGDCM_API virtual void stopping() override;

    /// Does nothing
    IOVTKGDCM_API virtual void configuring() override;

    /// Write the ImageSeries in DICOM format.
    IOVTKGDCM_API void updating() override;

    /// Return path type managed by the service, here FOLDER
    IOVTKGDCM_API ::fwIO::IOPathType getIOPathType() const override;

private:

    void saveImageSeries( const ::boost::filesystem::path folder, SPTR(::fwMedData::ImageSeries) series );

    SPTR(JobCreatedSignalType) m_sigJobCreated;
};

} // namespace ioVtkGdcm
