/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
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

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomCodedAttribute.hpp"

#include <fwData/Object.hpp>

#include <filesystem>

#include <array>
#include <string>

namespace fwLog
{
class Logger;
}

namespace fwGdcmIO
{
namespace helper
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
class FWGDCMIO_CLASS_API SegmentedPropertyRegistry
{
public:

    /// Entry containing the 5 attributes of a structure type
    typedef std::array<std::string, 5> EntryType;

    /// Constructor
    FWGDCMIO_API SegmentedPropertyRegistry();

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
    FWGDCMIO_API bool readSegmentedPropertyRegistryFile(const std::filesystem::path& filepath,
                                                        bool omitFirstLine                  = false,
                                                        const SPTR(::fwLog::Logger)& logger = 0);

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
    FWGDCMIO_API bool readSegmentedPropertyRegistryFile(std::istream& csvStream,
                                                        bool omitFirstLine                  = false,
                                                        const SPTR(::fwLog::Logger)& logger = 0);

    /// Returns whether the registry is empty or not
    FWGDCMIO_API bool empty() const;

    /// Returns the number of entries
    FWGDCMIO_API std::size_t count() const;

    /// Clear the registry
    FWGDCMIO_API void clear();

    /**
     * @brief Check if there is an entry for the corresponding structure type
     * @param[in] structureType Structure type
     */
    FWGDCMIO_API bool hasEntry(const std::string& structureType) const;

    /**
     * @brief Returns matching entry for the corresponding structure type
     * @param[in] structureType Structure type
     */
    FWGDCMIO_API EntryType getEntry(const std::string& structureType) const;

    /**
     * @brief Getters for entry's attributes
     * @param[in] structureType Structure type
     * @{ */
    FWGDCMIO_API std::string getPropertyType(const std::string& structureType) const;
    FWGDCMIO_API std::string getPropertyCategory(const std::string& structureType) const;
    FWGDCMIO_API std::string getPropertyTypeModifiers(const std::string& structureType) const;
    FWGDCMIO_API std::string getAnatomicRegion(const std::string& structureType) const;
    FWGDCMIO_API std::string getAnatomicRegionModifiers(const std::string& structureType) const;
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
    FWGDCMIO_API std::string getStructureType(const std::string& propertyType,
                                              const std::string& propertyCategory,
                                              const std::string& propertyTypeModifiers,
                                              const std::string& anatomicRegion,
                                              const std::string& anatomicRegionModifiers) const;

private:

    /// Entry registry type
    typedef std::map<std::string, EntryType> EntryRegistryType;

    /// Default entry value
    static const EntryType s_DEFAULT_ENTRY_VALUE;

    /// Segmented Property Registry
    EntryRegistryType m_registry;

};

} //namespace helper
} //namespace fwGdcmIO
