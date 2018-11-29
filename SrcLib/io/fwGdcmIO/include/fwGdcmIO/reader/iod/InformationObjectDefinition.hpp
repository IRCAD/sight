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

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomInstance.hpp"
#include "fwGdcmIO/exception/Failed.hpp"

#include <fwLog/Logger.hpp>

#include <fwMedData/DicomSeries.hpp>

#include <gdcmReader.h>

#include <cstdint>

namespace fwGdcmIO
{
namespace reader
{
namespace iod
{

/**
 * @brief InformationObjectDefinition base class used to read DICOM IODs
 */
class FWGDCMIO_CLASS_API InformationObjectDefinition
{

public:

    typedef std::function< void (std::uint64_t) > ProgressCallback;
    typedef std::function< bool () > CancelRequestedCallback;

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries
     * @param[in] instance Instance
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    FWGDCMIO_API InformationObjectDefinition(const ::fwMedData::DicomSeries::csptr& dicomSeries,
                                             const SPTR(::fwGdcmIO::container::DicomInstance)& instance,
                                             const ::fwLog::Logger::sptr& logger = nullptr,
                                             ProgressCallback progress           = nullptr,
                                             CancelRequestedCallback cancel      = nullptr);

    /// Destructor
    FWGDCMIO_API virtual ~InformationObjectDefinition();

    /**
     * @brief Read a DICOM File
     * @param[in,out] series Series that must be enriched
     */
    FWGDCMIO_API virtual void read(::fwMedData::Series::sptr series) throw(::fwGdcmIO::exception::Failed) = 0;

protected:

    /// DICOM Instance
    SPTR(::fwGdcmIO::container::DicomInstance) m_instance;

    /// Dicom Series
    ::fwMedData::DicomSeries::csptr m_dicomSeries;

    ///Logger
    ::fwLog::Logger::sptr m_logger;

    /// Progress callback for jobs
    ProgressCallback m_progressCallback;

    /// Cancel information for jobs
    CancelRequestedCallback m_cancelRequestedCallback;

};

} // namespace iod
} // namespace reader
} // namespace fwGdcmIO
