/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_READER_IE_SERIES_HPP__
#define __GDCMIO_READER_IE_SERIES_HPP__

#include <fwMedData/Series.hpp>

#include "gdcmIO/reader/ie/InformationEntity.hpp"

namespace gdcmIO
{
namespace reader
{
namespace ie
{

/**
 * @class Series
 * @brief Series Information Entity class
 */
class GDCMIO_CLASS_API Series : public ::gdcmIO::reader::ie::InformationEntity< ::fwMedData::Series >
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read series.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] series Series data
     */
    GDCMIO_API Series(SPTR(::fwDicomData::DicomSeries) dicomSeries,
            SPTR(::gdcm::Reader) reader,
            SPTR(::gdcmIO::container::DicomInstance) instance,
            ::fwMedData::Series::sptr series);

    /// Destructor
    GDCMIO_API virtual ~Series();

    /**
     * @brief Read General Series Module tags
     * @see PS 3.3 C.7.3.1
     */
    GDCMIO_API virtual void readGeneralSeriesModule();

};

} // namespace ie
} // namespace reader
} // namespace gdcmIO

#endif /* __GDCMIO_READER_IE_SERIES_HPP__ */
