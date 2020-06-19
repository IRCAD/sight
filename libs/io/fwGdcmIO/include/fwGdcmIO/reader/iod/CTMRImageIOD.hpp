/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "fwGdcmIO/reader/iod/InformationObjectDefinition.hpp"

namespace fwGdcmIO
{
namespace reader
{
namespace iod
{

/**
 * @brief CTMRImageIOD class used to read CT & MR Image IODs
 */
class FWGDCMIO_CLASS_API CTMRImageIOD : public ::fwGdcmIO::reader::iod::InformationObjectDefinition
{

public:

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries
     * @param[in] instance Instance
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    FWGDCMIO_API CTMRImageIOD(const ::fwMedData::DicomSeries::csptr& dicomSeries,
                              const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                              const ::fwLog::Logger::sptr& logger = nullptr,
                              ProgressCallback progress           = nullptr,
                              CancelRequestedCallback cancel      = nullptr);

    /// Destructor
    FWGDCMIO_API ~CTMRImageIOD();

    /**
     * @brief Read a DICOM File
     * @param[in,out] series Series that must be enriched
     * @throw(::fwGdcmIO::exception::Failed)
     */
    FWGDCMIO_API void read(::fwMedData::Series::sptr series);

    /// Enable buffer rotation
    void setBufferRotationEnabled(bool enabled)
    {
        m_enableBufferRotation = enabled;
    }

protected:
    /// Enable buffer rotation
    bool m_enableBufferRotation;
};

}  // namespace iod
}  // namespace reader
}  // namespace fwGdcmIO
