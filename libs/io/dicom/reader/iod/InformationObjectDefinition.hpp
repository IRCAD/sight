/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <core/log/logger.hpp>

#include <data/dicom_series.hpp>

#include <gdcmReader.h>

#include <cstdint>

namespace sight::io::dicom::reader::iod
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
        data::dicom_series::csptr _dicom_series,
        SPTR(io::dicom::container::DicomInstance)_instance,
        core::log::logger::sptr _logger = nullptr,
        ProgressCallback _progress      = nullptr,
        CancelRequestedCallback _cancel = nullptr
    );

    /// Destructor
    IO_DICOM_API virtual ~InformationObjectDefinition();

    /**
     * @brief Read a DICOM File
     * @param[in,out] series Series that must be enriched
     * @throw io::dicom::exception::Failed
     */
    IO_DICOM_API virtual void read(data::series::sptr _series) = 0;

protected:

    /// DICOM Instance
    SPTR(io::dicom::container::DicomInstance) m_instance;

    /// Dicom Series
    data::dicom_series::csptr m_dicomSeries;

    ///Logger
    core::log::logger::sptr m_logger;

    /// Progress callback for jobs
    ProgressCallback m_progressCallback;

    /// Cancel information for jobs
    CancelRequestedCallback m_cancelRequestedCallback;
};

} // namespace sight::io::dicom::reader::iod
