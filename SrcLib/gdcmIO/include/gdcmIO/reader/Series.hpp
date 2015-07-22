/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_SERIES_HPP__
#define __GDCMIO_READER_SERIES_HPP__

#include "gdcmIO/reader/iod/InformationObjectDefinition.hpp"

namespace gdcmIO
{
namespace reader
{

/**
 * @class Series
 * @brief Series class used to read Series
 */
class GDCMIO_CLASS_API Series
{
public:

    typedef std::map< SPTR(::gdcmIO::container::DicomInstance), ::fwMedData::Series::sptr > SeriesContainerMapType;

    /// Constructor
    GDCMIO_API Series();

    /// Destructor
    GDCMIO_API ~Series();

    //// Read DICOM file
    GDCMIO_API ::fwMedData::Series::sptr read(::fwDicomData::DicomSeries::sptr dicomSeries)
    throw(::gdcmIO::exception::Failed);

protected:

    /// Get referenced series when dealing with Spatial Fiducials
    SPTR(::gdcmIO::container::DicomInstance) getSpatialFiducialsReferencedSeriesInstance(
        ::fwDicomData::DicomSeries::sptr dicomSeries);

    /// Get referenced series when dealing with Structured Report
    SPTR(::gdcmIO::container::DicomInstance) getStructuredReportReferencedSeriesInstance(
        ::fwDicomData::DicomSeries::sptr dicomSeries);

    /// Series Container Map
    SeriesContainerMapType m_seriesContainerMap;


};

}  // namespace reader
}  // namespace gdcmIO

#endif /*__GDCMIO_READER_SERIES_HPP__*/
