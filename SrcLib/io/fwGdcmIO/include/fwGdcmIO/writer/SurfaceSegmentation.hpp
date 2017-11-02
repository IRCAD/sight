/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_WRITER_SURFACESEGMENTATION_HPP__
#define __FWGDCMIO_WRITER_SURFACESEGMENTATION_HPP__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomInstance.hpp"
#include "fwGdcmIO/exception/Failed.hpp"

#include <fwData/location/SingleFile.hpp>

#include <fwDataIO/writer/GenericObjectWriter.hpp>

#include <fwLog/Logger.hpp>

#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>

namespace fwDicomData
{
class DicomSeries;
}

namespace fwJobs
{
class Aggregator;
class Observer;
}

namespace fwGdcmIO
{
namespace writer
{

/**
 * @brief This class handles DICOM Surface Segmentation files writing.
 */
class FWGDCMIO_CLASS_API SurfaceSegmentation :
    public ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::ModelSeries >,
    public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >
{

public:

    fwCoreClassDefinitionsWithFactoryMacro(
        (SurfaceSegmentation)( ::fwDataIO::writer::GenericObjectWriter< ::fwMedData::ModelSeries >), (()),
        ::fwDataIO::writer::factory::New< SurfaceSegmentation >);

    /// Constructor
    FWGDCMIO_API SurfaceSegmentation(::fwDataIO::writer::IObjectWriter::Key key);

    /// Destructor
    FWGDCMIO_API ~SurfaceSegmentation();

    /// Load and start appropriate writing tools.
    FWGDCMIO_API void write() override;

    /// Does nothing
    FWGDCMIO_API std::string extension() override;

    /**
     * @brief Set associated DICOM series (Associated image instances)
     * @param[in] dicomSeries Associated DICOM series
     */
    FWGDCMIO_API void setAssociatedDicomSeries(const SPTR(::fwMedData::DicomSeries)& dicomSeries);

    /// Returns logger
    FWGDCMIO_API SPTR(::fwLog::Logger) getLogger() const;

    /// Getter for writer's job
    FWGDCMIO_API SPTR(::fwJobs::IJob) getJob() const override;

private:

    /// Associated DICOM series (Associated image instances)
    SPTR(::fwMedData::DicomSeries) m_associatedDicomSeries;

    /// Logger
    ::fwLog::Logger::sptr m_logger;

    /// Writer job
    SPTR(::fwJobs::Observer) m_writerJob;

};

} // namespace writer
} // namespace fwGdcmIO

#endif // __FWGDCMIO_WRITER_SURFACESEGMENTATION_HPP__

