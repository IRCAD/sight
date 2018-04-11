/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwGdcmIO/reader/ie/InformationEntity.hpp"

#include <fwMedData/Series.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

/**
 * @brief Series Information Entity class
 */
class FWGDCMIO_CLASS_API Series : public ::fwGdcmIO::reader::ie::InformationEntity< ::fwMedData::Series >
{

public:
    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries used to access computed tag values.
     * @param[in] reader GDCM reader that must be used to read series.
     * @param[in] instance DICOM instance used to share informations between modules
     * @param[in] series Series data
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    FWGDCMIO_API Series(const CSPTR(::fwMedData::DicomSeries)& dicomSeries,
                        const SPTR(::gdcm::Reader)& reader,
                        const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                        const ::fwMedData::Series::sptr& series,
                        const ::fwLog::Logger::sptr& logger = nullptr,
                        ProgressCallback progress           = nullptr,
                        CancelRequestedCallback cancel      = nullptr);

    /// Destructor
    FWGDCMIO_API virtual ~Series();

    /**
     * @brief Read General Series Module tags
     * @see PS 3.3 C.7.3.1
     */
    FWGDCMIO_API virtual void readGeneralSeriesModule();

};

} // namespace ie
} // namespace reader
} // namespace fwGdcmIO
