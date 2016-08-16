/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_READER_SERIES_HPP__
#define __FWGDCMIO_READER_SERIES_HPP__

#include "fwGdcmIO/reader/iod/InformationObjectDefinition.hpp"

#include <fwLog/Logger.hpp>

#include <functional>

namespace fwGdcmIO
{
namespace reader
{

/**
 * @brief Series class used to read Series
 */
class FWGDCMIO_CLASS_API Series
{
public:

    typedef std::map< SPTR(::fwGdcmIO::container::DicomInstance), ::fwMedData::Series::sptr > SeriesContainerMapType;

    typedef std::function< void (unsigned int&) > ProgressCallback;

    /// Constructor
    FWGDCMIO_API Series();

    /// Destructor
    FWGDCMIO_API ~Series();

    //// Read DICOM file
    FWGDCMIO_API ::fwMedData::Series::sptr read(::fwMedData::DicomSeries::sptr dicomSeries)
    throw(::fwGdcmIO::exception::Failed);

    /// Get Logger
    const ::fwLog::Logger::sptr& getLogger() const
    {
        return m_logger;
    }

    /// Set Logger
    void setLogger(const ::fwLog::Logger::sptr& logger)
    {
        m_logger = logger;
    }

    void addCallback(std::function< void(unsigned int&) > c)
    {
        m_callback = c;
    }
    void cancel()
    {
        m_cancelled = true;
    }

    /// Enable buffer rotation
    void setBufferRotationEnabled(bool enabled)
    {
        m_enableBufferRotation = enabled;
    }

protected:

    /// Get referenced series when dealing with Spatial Fiducials
    SPTR(::fwGdcmIO::container::DicomInstance) getSpatialFiducialsReferencedSeriesInstance(
        ::fwMedData::DicomSeries::sptr dicomSeries);

    /// Get referenced series when dealing with Structured Report
    SPTR(::fwGdcmIO::container::DicomInstance) getStructuredReportReferencedSeriesInstance(
        ::fwMedData::DicomSeries::sptr dicomSeries);

    /// Series Container Map
    SeriesContainerMapType m_seriesContainerMap;

    ///Logger
    ::fwLog::Logger::sptr m_logger;

    /// Progress callback for jobs
    ProgressCallback m_callback;

    /// Cancel information for jobs
    bool m_cancelled;

    /// Enable buffer rotation
    bool m_enableBufferRotation;

};

}  // namespace reader
}  // namespace fwGdcmIO

#endif /*__FWGDCMIO_READER_SERIES_HPP__*/
