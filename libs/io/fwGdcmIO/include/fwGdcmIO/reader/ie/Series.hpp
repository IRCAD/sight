/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
