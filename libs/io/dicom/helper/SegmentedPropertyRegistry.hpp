/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "io/dicom/config.hpp"
#include "io/dicom/container/DicomCodedAttribute.hpp"

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
class IO_DICOM_CLASS_API SegmentedPropertyRegistry
{
public:

    /// Entry containing the 5 attributes of a structure type
    typedef std::array<std::string, 5> entry_t;

    /// Constructor
    IO_DICOM_API SegmentedPropertyRegistry();

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
     * @param[in] filepath Path of the CSV file
     * @param[in] omitFirstLine If set to 'true', the first line of the file is omitted
     * @param[in] logger Logger used to display errors
     */
    IO_DICOM_API bool readSegmentedPropertyRegistryFile(
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
     * @param[in] csvStream CSV stream
     * @param[in] omitFirstLine If set to 'true', the first line of the file is omitted
     * @param[in] logger Logger used to display errors
     */
    IO_DICOM_API bool readSegmentedPropertyRegistryFile(
        std::istream& _csv_stream,
        bool _omit_first_line                  = false,
        const SPTR(core::log::logger)& _logger = nullptr
    );

    /// Returns whether the registry is empty or not
    [[nodiscard]] IO_DICOM_API bool empty() const;

    /// Returns the number of entries
    [[nodiscard]] IO_DICOM_API std::size_t count() const;

    /// Clear the registry
    IO_DICOM_API void clear();

    /**
     * @brief Check if there is an entry for the corresponding structure type
     * @param[in] structureType Structure type
     */
    [[nodiscard]] IO_DICOM_API bool hasEntry(const std::string& _structure_type) const;

    /**
     * @brief Returns matching entry for the corresponding structure type
     * @param[in] structureType Structure type
     */
    [[nodiscard]] IO_DICOM_API entry_t getEntry(const std::string& _structure_type) const;

    /**
     * @brief Getters for entry's attributes
     * @param[in] structureType Structure type
     * @{ */
    [[nodiscard]] IO_DICOM_API std::string get_property_type(const std::string& _structure_type) const;
    [[nodiscard]] IO_DICOM_API std::string getPropertyCategory(const std::string& _structure_type) const;
    [[nodiscard]] IO_DICOM_API std::string getPropertyTypeModifiers(const std::string& _structure_type) const;
    [[nodiscard]] IO_DICOM_API std::string getAnatomicRegion(const std::string& _structure_type) const;
    [[nodiscard]] IO_DICOM_API std::string getAnatomicRegionModifiers(const std::string& _structure_type) const;
    /**  @} */

    /**
     * @brief Returns the structure type associated to the attribute list.
     * If no match is found, it returns an empty string.
     * @param[in] propertyType Property Type
     * @param[in] propertyCategory Property Category
     * @param[in] propertyTypeModifiers Property Modifiers
     * @param[in] anatomicRegion Anatomic Region
     * @param[in] anatomicRegionModifiers Anatomic Region Modifiers
     */
    [[nodiscard]] IO_DICOM_API std::string get_structure_type(
        const std::string& _property_type,
        const std::string& _property_category,
        const std::string& _property_type_modifiers,
        const std::string& _anatomic_region,
        const std::string& _anatomic_region_modifiers
    ) const;

private:

    std::string getEntryValue(std::size_t _index, const std::string& _structure_type) const;

    /// Entry registry type
    typedef std::map<std::string, entry_t> entry_registry_t;

    /// Default entry value
    static const entry_t s_DEFAULT_ENTRY_VALUE;

    /// Segmented Property Registry
    entry_registry_t m_registry;
};

} // namespace sight::io::dicom::helper
