/************************************************************************
 *
 * Copyright (C) 2017-2024 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include "io/dicom/container/dicom_coded_attribute.hpp"

#include <data/object.hpp>

#include <array>
#include <filesystem>
#include <string>

namespace sight::core::log
{

class logger;

} // namespace sight::core::log

namespace sight::io::dicom::helper
{

/**
 * @brief Registry of Segment Properties.
 * This class defines a registry which links a structure type to an entry.
 * Each entry is composed of 5 attributes :
 * - Property Type
 * - Property Category
 * - Property Type Modifiers
 * - Anatomic Region
 * - Anatomic Region Modifiers
 */
class SIGHT_IO_DICOM_CLASS_API segmented_property_registry
{
public:

    /// Entry containing the 5 attributes of a structure type
    using entry_t = std::array<std::string, 5>;

    /// Constructor
    SIGHT_IO_DICOM_API segmented_property_registry();

    /**
     * @brief Read an extract registry values from a CSV file
     * Each lines shall contain at least 6 elements :
     * - Structure Type
     * - Property Type (With unique (AAA;BBB;CCC) value) [MANDATORY]
     * - Property Category (With unique (AAA;BBB;CCC) value) [MANDATORY]
     * - Property Type Modifiers (With one or more (AAA;BBB;CCC) values)
     * - Anatomic Region (With unique (AAA;BBB;CCC) value) [MANDATORY]
     * - Anatomic Region Modifiers (With one or more (AAA;BBB;CCC) values)
     * Each of those elements shall be separated using the '|' separator
     * @param[in] _filepath Path of the CSV file
     * @param[in] _omit_first_line If set to 'true', the first line of the file is omitted
     * @param[in] _logger Logger used to display errors
     */
    SIGHT_IO_DICOM_API bool read_segmented_property_registry_file(
        const std::filesystem::path& _filepath,
        bool _omit_first_line                  = false,
        const SPTR(core::log::logger)& _logger = nullptr
    );

    /**
     * @brief Read an extract registry values from a CSV stream
     * Each lines shall contain at least 6 elements :
     * - Structure Type
     * - Property Type (With unique (AAA;BBB;CCC) value) [MANDATORY]
     * - Property Category (With unique (AAA;BBB;CCC) value) [MANDATORY]
     * - Property Type Modifiers (With one or more (AAA;BBB;CCC) values)
     * - Anatomic Region (With unique (AAA;BBB;CCC) value)
     * - Anatomic Region Modifiers (With one or more (AAA;BBB;CCC) values)
     * Each of those elements shall be separated using the '|' separator
     * @param[in] _csv_stream CSV stream
     * @param[in] _omit_first_line If set to 'true', the first line of the file is omitted
     * @param[in] _logger Logger used to display errors
     */
    SIGHT_IO_DICOM_API bool read_segmented_property_registry_file(
        std::istream& _csv_stream,
        bool _omit_first_line                  = false,
        const SPTR(core::log::logger)& _logger = nullptr
    );

    /// Returns whether the registry is empty or not
    [[nodiscard]] SIGHT_IO_DICOM_API bool empty() const;

    /// Returns the number of entries
    [[nodiscard]] SIGHT_IO_DICOM_API std::size_t count() const;

    /// Clear the registry
    SIGHT_IO_DICOM_API void clear();

    /**
     * @brief Check if there is an entry for the corresponding structure type
     * @param[in] _structure_type Structure type
     */
    [[nodiscard]] SIGHT_IO_DICOM_API bool has_entry(const std::string& _structure_type) const;

    /**
     * @brief Returns matching entry for the corresponding structure type
     * @param[in] _structure_type Structure type
     */
    [[nodiscard]] SIGHT_IO_DICOM_API entry_t get_entry(const std::string& _structure_type) const;

    /**
     * @brief Getters for entry's attributes
     * @param[in] _structure_type Structure type
     * @{ */
    [[nodiscard]] SIGHT_IO_DICOM_API std::string get_property_type(const std::string& _structure_type) const;
    [[nodiscard]] SIGHT_IO_DICOM_API std::string get_property_category(const std::string& _structure_type) const;
    [[nodiscard]] SIGHT_IO_DICOM_API std::string get_property_type_modifiers(const std::string& _structure_type) const;
    [[nodiscard]] SIGHT_IO_DICOM_API std::string get_anatomic_region(const std::string& _structure_type) const;
    [[nodiscard]] SIGHT_IO_DICOM_API std::string get_anatomic_region_modifiers(
        const std::string& _structure_type
    ) const;
    /**  @} */

    /**
     * @brief Returns the structure type associated to the attribute list.
     * If no match is found, it returns an empty string.
     * @param[in] _property_type Property Type
     * @param[in] _property_category Property Category
     * @param[in] _property_type_modifiers Property Modifiers
     * @param[in] _anatomic_region Anatomic Region
     * @param[in] _anatomic_region_modifiers Anatomic Region Modifiers
     */
    [[nodiscard]] SIGHT_IO_DICOM_API std::string get_structure_type(
        const std::string& _property_type,
        const std::string& _property_category,
        const std::string& _property_type_modifiers,
        const std::string& _anatomic_region,
        const std::string& _anatomic_region_modifiers
    ) const;

private:

    std::string get_entry_value(std::size_t _index, const std::string& _structure_type) const;

    /// Entry registry type
    using entry_registry_t = std::map<std::string, entry_t>;

    /// Default entry value
    static const entry_t DEFAULT_ENTRY_VALUE;

    /// Segmented Property Registry
    entry_registry_t m_registry;
};

} // namespace sight::io::dicom::helper
