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

#include "io/dicom/exception/failed.hpp"

#include <core/location/multiple_files.hpp>
#include <core/location/single_folder.hpp>
#include <core/log/logger.hpp>

#include <data/dicom_series.hpp>
#include <data/series_set.hpp>

#include <io/__/reader/generic_object_reader.hpp>

#include <service/base.hpp>

namespace sight::core::jobs
{

class aggregator;
class base;
class observer;

}

namespace sight::io::dicom
{

namespace reader
{

/**
 * @brief This class adds patient(s) from DICOM file(s) to data::series_set.
 */
class SIGHT_IO_DICOM_CLASS_API series_set : public sight::io::reader::generic_object_reader<data::series_set>,
                                            public core::location::single_folder,
                                            public core::location::multiple_files,
                                            public core::com::has_signals
{
public:

    SIGHT_DECLARE_CLASS(series_set, io::reader::generic_object_reader<data::series_set>);

    using dicom_series_container_t        = std::vector<std::shared_ptr<data::dicom_series> >;
    using filename_container_t            = std::vector<std::string>;
    using supported_sop_class_container_t = std::vector<std::string>;

    /// Constructor
    SIGHT_IO_DICOM_API series_set();

    /// Destructor
    SIGHT_IO_DICOM_API ~series_set() override = default;

    /// Reads DICOM data from configured path and fills series_set object
    SIGHT_IO_DICOM_API void read() override;

    /**
     * @brief Reads DICOM data from DicomSeries and fills series_set object
     * @param[in] _dicom_series_set series_set containing DicomSeries that must be read
     * @param[in] _notifier Service used to notify changes in series_set
     */
    SIGHT_IO_DICOM_API void read_from_dicom_series_set(
        const data::series_set::csptr& _dicom_series_set,
        const sight::service::base::sptr& _notifier = sight::service::base::sptr()
    );

    /**
     * @brief Reads DICOM data from configured path and fills series_set object with DicomSeries
     */
    SIGHT_IO_DICOM_API void read_dicom_series();

    /// Return true if a dicomdir file can be read.
    SIGHT_IO_DICOM_API bool is_dicom_dir_available();

    /// Return DicomSeries container
    SIGHT_IO_DICOM_API dicom_series_container_t& get_dicom_series();

    /// Get Set whether the reader must use the dicomdir file or not
    const bool& get_dicomdir_activated() const
    {
        return m_is_dicomdir_activated;
    }

    ///Set Set whether the reader must use the dicomdir file or not
    void set_dicomdir_activated(const bool& _is_dicomdir_activated)
    {
        m_is_dicomdir_activated = _is_dicomdir_activated;
    }

    /// Get Dicom filter type that must be applied prior to the reading process
    const std::string& get_dicom_filter_type() const
    {
        return m_dicom_filter_type;
    }

    ///Set Dicom filter type that must be applied prior to the reading process
    void set_dicom_filter_type(const std::string& _dicom_filter_type)
    {
        m_dicom_filter_type = _dicom_filter_type;
    }

    /// Get Supported SOP Class
    const supported_sop_class_container_t& getsupported_sop_class_container() const
    {
        return m_supported_sop_class_container;
    }

    ///Set Supported SOP Class
    void setsupported_sop_class_container(const supported_sop_class_container_t& _supported_sop_class_container)
    {
        m_supported_sop_class_container = _supported_sop_class_container;
    }

    /// Get Logger
    const core::log::logger::sptr& get_logger() const
    {
        return m_logger;
    }

    ///Set Logger
    void set_logger(const core::log::logger::sptr& _logger)
    {
        m_logger = _logger;
    }

    /// Getter for reader's job
    SIGHT_IO_DICOM_API SPTR(core::jobs::base) get_job() const override;

    /// Enable buffer rotation
    void set_buffer_rotation_enabled(bool _enabled)
    {
        m_enable_buffer_rotation = _enabled;
    }

private:

    /**
     * @brief Read DICOM series
     */
    void read_dicom();

    /**
     * @brief Convert DicomSeries to Image or Model Series
     */
    void convert_dicom_series(const sight::service::base::sptr& _notifier = sight::service::base::sptr());

    /**
     * @brief Function used to sort DicomSeries
     * @param[in] _a First DicomSeries
     * @param[in] _b Second DicomSeries
     */
    static bool dicom_series_comparator(
        const SPTR(data::dicom_series)& _a,
        const SPTR(data::dicom_series)& _b
    );

    /// Object Reader Map
    dicom_series_container_t m_dicom_series_container;

    /// True if the reader can use the dicomdir file.
    bool m_is_dicomdir_activated;

    /// Dicom filter type that must be applied prior to the reading process
    std::string m_dicom_filter_type;

    /// Supported SOP Class container
    supported_sop_class_container_t m_supported_sop_class_container;

    /// Logger
    core::log::logger::sptr m_logger;

    /// Observer managing all subjobs
    SPTR(core::jobs::aggregator) m_job;

    /// Enable buffer rotation
    bool m_enable_buffer_rotation;

    SPTR(core::jobs::observer) m_dicomdir_file_lookup_job;
    SPTR(core::jobs::observer) m_regular_file_lookup_job;
    SPTR(core::jobs::observer) m_reader_job;
    SPTR(core::jobs::observer) m_complete_dicom_series_job;
    SPTR(core::jobs::observer) m_converter_job;
};

} // namespace reader

} // namespace sight::io::dicom
