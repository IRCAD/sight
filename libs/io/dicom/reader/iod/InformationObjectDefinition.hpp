/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/dicom/config.hpp"
#include "io/dicom/container/DicomInstance.hpp"
#include "io/dicom/exception/Failed.hpp"

#include <core/log/Logger.hpp>

#include <data/DicomSeries.hpp>

#include <gdcmReader.h>

#include <cstdint>

namespace sight::io::dicom
{

namespace reader
{

namespace iod
{

/**
 * @brief InformationObjectDefinition base class used to read DICOM IODs
 */
class IO_DICOM_CLASS_API InformationObjectDefinition
{
public:

    typedef std::function<void (std::uint64_t)> ProgressCallback;
    typedef std::function<bool ()> CancelRequestedCallback;

    /**
     * @brief Constructor
     * @param[in] dicomSeries DicomSeries
     * @param[in] instance Instance
     * @param[in] logger Logger
     * @param[in] progress Progress callback
     * @param[in] cancel Cancel requested callback
     */
    IO_DICOM_API InformationObjectDefinition(
        const data::DicomSeries::csptr& dicomSeries,
        const SPTR(io::dicom::container::DicomInstance)& instance,
        const core::log::Logger::sptr& logger = nullptr,
        ProgressCallback progress             = nullptr,
        CancelRequestedCallback cancel        = nullptr
    );

    /// Destructor
    IO_DICOM_API virtual ~InformationObjectDefinition();

    /**
     * @brief Read a DICOM File
     * @param[in,out] series Series that must be enriched
     * @throw io::dicom::exception::Failed
     */
    IO_DICOM_API virtual void read(data::Series::sptr series) = 0;

protected:

    /// DICOM Instance
    SPTR(io::dicom::container::DicomInstance) m_instance;

    /// Dicom Series
    data::DicomSeries::csptr m_dicomSeries;

    ///Logger
    core::log::Logger::sptr m_logger;

    /// Progress callback for jobs
    ProgressCallback m_progressCallback;

    /// Cancel information for jobs
    CancelRequestedCallback m_cancelRequestedCallback;
};

} // namespace iod

} // namespace reader

} // namespace sight::io::dicom
