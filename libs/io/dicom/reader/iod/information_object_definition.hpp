/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/io/dicom/config.hpp>

#include "io/dicom/container/dicom_instance.hpp"
#include "io/dicom/exception/failed.hpp"

#include <core/log/logger.hpp>

#include <data/dicom_series.hpp>

#include <gdcmReader.h>

#include <cstdint>

namespace sight::io::dicom::reader::iod
{

/**
 * @brief InformationObjectDefinition base class used to read DICOM IODs
 */
class SIGHT_IO_DICOM_CLASS_API information_object_definition
{
public:

    using progress_callback         = std::function<void (std::uint64_t)>;
    using cancel_requested_callback = std::function<bool ()>;

    /**
     * @brief Constructor
     * @param[in] _dicom_series DicomSeries
     * @param[in] _instance Instance
     * @param[in] _logger Logger
     * @param[in] _progress Progress callback
     * @param[in] _cancel Cancel requested callback
     */
    SIGHT_IO_DICOM_API information_object_definition(
        data::dicom_series::csptr _dicom_series,
        SPTR(io::dicom::container::dicom_instance)_instance,
        core::log::logger::sptr _logger   = nullptr,
        progress_callback _progress       = nullptr,
        cancel_requested_callback _cancel = nullptr
    );

    /// Destructor
    SIGHT_IO_DICOM_API virtual ~information_object_definition();

    /**
     * @brief Read a DICOM File
     * @param[in,out] _series Series that must be enriched
     * @throw io::dicom::exception::Failed
     */
    SIGHT_IO_DICOM_API virtual void read(data::series::sptr _series) = 0;

protected:

    /// DICOM Instance
    SPTR(io::dicom::container::dicom_instance) m_instance;

    /// Dicom Series
    data::dicom_series::csptr m_dicom_series;

    ///Logger
    core::log::logger::sptr m_logger;

    /// Progress callback for jobs
    progress_callback m_progress_callback;

    /// Cancel information for jobs
    cancel_requested_callback m_cancel_requested_callback;
};

} // namespace sight::io::dicom::reader::iod
