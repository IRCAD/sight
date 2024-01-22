/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/config.hpp"
#include "data/factory/new.hpp"
#include "data/series.hpp"
#include "data/types.hpp"

#include <core/memory/buffer_object.hpp>

#include <data/factory/new.hpp>

#include <filesystem>

namespace sight::data
{

/**
 * @brief Holds DICOM series.
 */
class DATA_CLASS_API dicom_series final : public series
{
public:

    SIGHT_DECLARE_CLASS(dicom_series, series);

    using dicom_container_t              = std::map<std::size_t, core::memory::buffer_object::sptr>;
    using sop_class_uid_container_t      = std::set<std::string>;
    using computed_tag_value_container_t = std::map<std::string, std::string>;

    /// Destroys the DICOM series.
    DATA_API ~dicom_series() noexcept override = default;

    /**
     * @brief Adds a DICOM path.
     * @param _instance_index index of the instance.
     * @param _path the path.
     */
    DATA_API void add_dicom_path(std::size_t _instance_index, const std::filesystem::path& _path);

    /**
     * @brief Adds a binary buffer.
     * @param _instance_index index of the instance.
     * @param _buffer the buffer.
     */
    DATA_API void add_binary(std::size_t _instance_index, const core::memory::buffer_object::sptr& _buffer);

    /**
     * @brief Gets if the instance is available on the local computer
     * @param _instance_index the instance to check.
     * @return True if the instance is available on the local computer
     */
    DATA_API bool is_instance_available(std::size_t _instance_index) const;

    /**
     * @brief Adds a sop_classUID that is used by this series.
     * @param _sop_class_uid sop_classUID that must be added.
     */
    DATA_API void addsop_class_uid(const std::string& _sop_class_uid);

    /**
     * @brief Adds a computed value to the specified tag.
     * @param _tag_name Name of the tag.
     * @param _value Computed value.
     */
    DATA_API void add_computed_tag_value(const std::string& _tag_name, const std::string& _value);

    /**
     * @brief Gets if there is a computed value for the specified tag.
     * @param _tag_name Name of the tag.
     * @return True if there is a computed value for the tag.
     */
    DATA_API bool has_computed_values(const std::string& _tag_name) const;

    /**
     * @brief Gets the number of instances in the series.
     * @return The number of instances in the series.
     */
    std::size_t num_instances() const
    {
        return m_number_of_instances;
    }

    ///f Sets the number of instances in the series.
    void set_number_of_instances(std::size_t _val)
    {
        m_number_of_instances = _val;
    }

    /// Gets the DICOM container.
    const dicom_container_t& get_dicom_container() const
    {
        return m_dicom_container;
    }

    /// Sets the DICOM container.
    void set_dicom_container(const dicom_container_t& _dicom_container)
    {
        m_dicom_container = _dicom_container;
    }

    /// Clears the DICOM container.
    void clear_dicom_container()
    {
        m_dicom_container.clear();
    }

    /// Gets the SOP Class UID.
    const sop_class_uid_container_t& get_sop_class_ui_ds() const
    {
        return m_sop_class_ui_ds;
    }

    //------------------------------------------------------------------------------

    sop_class_uid_container_t& get_sop_class_ui_ds()
    {
        return m_sop_class_ui_ds;
    }

    /// Sets the SOP Class UID.
    void set_sop_class_ui_ds(const sop_class_uid_container_t& _val)
    {
        m_sop_class_ui_ds = _val;
    }

    /// Gets the computed tag values.
    const computed_tag_value_container_t& get_computed_tag_values() const
    {
        return m_computed_tag_values;
    }

    /// Sets the computed tag values.
    void set_computed_tag_values(const computed_tag_value_container_t& _val)
    {
        m_computed_tag_values = _val;
    }

    /// Gets the first instance number (0 or 1) - Used for PACS preview.
    std::size_t get_first_instance_number() const
    {
        return m_first_instance_number;
    }

    /// Sets the first instance number (0 or 1) - Used for PACS preview.
    void set_first_instance_number(std::size_t _first_instance_number)
    {
        m_first_instance_number = _first_instance_number;
    }

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const dicom_series& _other) const noexcept;
    DATA_API bool operator!=(const dicom_series& _other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    DATA_API void shallow_copy(const object::csptr& _source) override;

    /// Defines deep copy
    /// @throws data::exception if an errors occurs during copy
    /// @param _source source object to copy
    /// @param _cache cache used to deduplicate pointers
    DATA_API void deep_copy(
        const object::csptr& _source,
        const std::unique_ptr<deep_copy_cache_t>& _cache = std::make_unique<deep_copy_cache_t>()
    ) override;

protected:

    /// Defines the number of instances in the series.
    std::size_t m_number_of_instances {0};

    /// Stores DICOM.
    dicom_container_t m_dicom_container;

    /// Stores SOP Class UIDs.
    sop_class_uid_container_t m_sop_class_ui_ds;

    /// Stores computed tag values.
    computed_tag_value_container_t m_computed_tag_values;

    /// Defines the first instance number (0 or 1) - Used for PACS preview.
    std::size_t m_first_instance_number {0};
};

} // Namespace sight::data
