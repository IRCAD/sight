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

#include "io/dicom/reader/iod/information_object_definition.hpp"

#include <core/log/logger.hpp>

namespace sight::io::dicom::reader
{

/**
 * @brief series class used to read series
 */
class IO_DICOM_CLASS_API series
{
public:

    using series_container_map_t = std::map<std::shared_ptr<io::dicom::container::dicom_instance>, data::series::sptr>;

    using progress_callback         = std::function<void (std::uint64_t)>;
    using cancel_requested_callback = std::function<bool ()>;

    /// Constructor
    IO_DICOM_API series();

    /// Destructor
    IO_DICOM_API ~series();

    /**
     * @brief Read DICOM series
     * @param[in] dicomSeries DICOM series that shall be read
     * @throw io::dicom::exception::Failed
     */
    IO_DICOM_API data::series::sptr read(const data::dicom_series::csptr& _dicom_series);

    /// Get Logger
    [[nodiscard]] const core::log::logger::sptr& get_logger() const
    {
        return m_logger;
    }

    /// Set Logger
    void set_logger(const core::log::logger::sptr& _logger)
    {
        m_logger = _logger;
    }

    /**
     * @brief Set progress callback
     * @param[in] callback Progress callback
     */
    void set_progress_callback(progress_callback _callback)
    {
        m_progress_callback = _callback;
    }

    /**
     * @brief Set cancel callback
     * @param[in] callback Cancel callback
     */
    void set_cancel_requested_callback(cancel_requested_callback _callback)
    {
        m_cancel_requested_callback = _callback;
    }

    /// Enable buffer rotation
    void set_buffer_rotation_enabled(bool _enabled)
    {
        m_enable_buffer_rotation = _enabled;
    }

protected:

    /// Get referenced series when dealing with Spatial Fiducials
    SPTR(io::dicom::container::dicom_instance) get_spatial_fiducials_referenced_series_instance(
        const data::dicom_series::csptr& _dicom_series
    );

    /// Get referenced series when dealing with Structured Report
    SPTR(io::dicom::container::dicom_instance) get_structured_report_referenced_series_instance(
        const data::dicom_series::csptr& _dicom_series
    );

    /// series Container Map
    series_container_map_t m_series_container_map;

    /// Logger
    core::log::logger::sptr m_logger;

    /// Progress callback for jobs
    progress_callback m_progress_callback;

    /// Cancel information for jobs
    cancel_requested_callback m_cancel_requested_callback;

    /// Enable buffer rotation
    bool m_enable_buffer_rotation {true};
};

} // namespace sight::io::dicom::reader
