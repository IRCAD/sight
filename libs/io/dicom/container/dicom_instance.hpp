/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "io/dicom/config.hpp"

#include <core/log/logger.hpp>
#include <core/macros.hpp>

#include <gdcmMediaStorage.h>

#include <memory>

namespace sight::data
{

class dicom_series;
class series;

} // namespace sight::data

namespace sight::io::dicom::container
{

/**
 * @brief This class defines a DICOM SOP instance. It is useful during the whole writing process.
 * This class allows to share data between module writers.
 */
class IO_DICOM_CLASS_API dicom_instance
{
public:

    using sptr = std::shared_ptr<dicom_instance>;

    /// SOP Instance Container Type
    using sop_instance_uid_container_t = std::vector<std::string>;

    /// Constructor
    IO_DICOM_API dicom_instance();

    /**
     * @brief Constructor
     * @param[in] _series Series from which the instance is created
     * @param[in] _logger Logger
     * @param[in] _is_multi_files Set whether the instance must be split in several files or not
     */
    IO_DICOM_API dicom_instance(
        const CSPTR(data::series)& _series,
        SPTR(core::log::logger)_logger = nullptr,
        bool _is_multi_files           = true
    );

    /**
     * @brief Constructor
     * @param[in] _dicom_series DicomSeries from which the instance is created
     * @param[in] _logger Logger
     */
    IO_DICOM_API dicom_instance(
        const CSPTR(data::dicom_series)& _dicom_series,
        SPTR(core::log::logger)_logger = nullptr
    );

    /// Copy constructor
    IO_DICOM_API dicom_instance(const dicom_instance& _dicom_instance);

    /// Destructor
    IO_DICOM_API virtual ~dicom_instance();

    /// Get the flag on multi-files state of an image series
    [[nodiscard]] bool get_is_multi_files() const
    {
        return m_is_multi_files;
    }

    /// Set the flag on multi-files state of an image series
    void set_is_multi_files(bool _is_multi_files)
    {
        m_is_multi_files = _is_multi_files;
    }

    /// Get SOP Class UID
    [[nodiscard]] const std::string& get_sop_class_uid() const
    {
        return m_sop_class_uid;
    }

    /// Set SOP Class UID
    void set_sop_class_uid(const std::string& _sop_class_uid)
    {
        m_sop_class_uid = _sop_class_uid;
    }

    /// Get Series Instance UID
    [[nodiscard]] const std::string& get_series_instance_uid() const
    {
        return m_series_instance_uid;
    }

    /// Set Series Instance UID
    void set_series_instance_uid(const std::string& _series_instance_uid)
    {
        m_series_instance_uid = _series_instance_uid;
    }

    /// Get Study Instance UID
    [[nodiscard]] const std::string& get_study_instance_uid() const
    {
        return m_study_instance_uid;
    }

    /// Set Study Instance UID
    void set_study_instance_uid(const std::string& _study_instance_uid)
    {
        m_study_instance_uid = _study_instance_uid;
    }

    /// Get Study Instance UID
    [[nodiscard]] const std::string& get_frame_of_reference_uid() const
    {
        return m_frame_of_reference_uid;
    }

    /// Set Study Instance UID
    void set_frame_of_reference_uid(const std::string& _frame_of_reference_uid)
    {
        m_frame_of_reference_uid = _frame_of_reference_uid;
    }

    /// Get SOP Instance UID container
    [[nodiscard]] const sop_instance_uid_container_t& get_sop_instance_uid_container() const
    {
        return m_sop_instance_uid_container;
    }

    /// Get SOP Instance UID container
    sop_instance_uid_container_t& get_sop_instance_uid_container()
    {
        return m_sop_instance_uid_container;
    }

    /// Set SOP Instance UID Container
    void set_sop_instance_uid_container(const sop_instance_uid_container_t& _sop_instance_uid_container)
    {
        m_sop_instance_uid_container = _sop_instance_uid_container;
    }

protected:

    /**
     * @brief Compute sop_classUID
     * @param[in] _series Series
     */
    void computesop_class_uid(const CSPTR(data::series)& _series);

    /**
     * @brief Generate SOPInstanceUIDs according to series type and dimension
     * @param[in] _series Series
     */
    void generate_sop_instance_ui_ds(const CSPTR(data::series)& _series);

    /**
     * @brief Extract 'SOP Instance UIDs' and 'Frame of Reference UID' from a DICOM series
     * @param[in] _dicom_series DICOM Series from which the UIDs are extracted
     */
    void read_uid_from_dicom_series(const CSPTR(data::dicom_series)& _dicom_series);

private:

    /// Define if the instance must be split in several files
    bool m_is_multi_files {true};

    /// SOP Class UID
    std::string m_sop_class_uid;

    /// Study Instance UID
    std::string m_study_instance_uid;

    /// Series Instance UID
    std::string m_series_instance_uid;

    /// Frame Of Reference UID
    std::string m_frame_of_reference_uid;

    /// SOP Instance UID container
    sop_instance_uid_container_t m_sop_instance_uid_container;

    /// Logger
    SPTR(core::log::logger) m_logger;
};

} // namespace sight::io::dicom::container
