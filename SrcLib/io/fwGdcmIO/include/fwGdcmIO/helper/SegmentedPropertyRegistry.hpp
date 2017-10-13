/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_HELPER_SEGMENTEDPROPERTYREGISTRY_HPP__
#define __FWGDCMIO_HELPER_SEGMENTEDPROPERTYREGISTRY_HPP__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomCodedAttribute.hpp"

#include <fwData/Object.hpp>

#include <boost/filesystem/path.hpp>

#include <array>
#include <iostream>
#include <map>
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
    FWGDCMIO_API bool readSegmentedPropertyRegistryFile(const ::boost::filesystem::path& filepath,
                                                        bool omitFirstLine = false,
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
                                                        bool omitFirstLine = false,
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


protected:

    /// Entry registry type
    typedef std::map<std::string, EntryType> EntryRegistryType;

    /// Default entry value
    static const EntryType s_DEFAULT_ENTRY_VALUE;

    /// Segmented Property Registry
    EntryRegistryType m_registry;

};

} //namespace helper
} //namespace fwGdcmIO

#endif // __FWGDCMIO_HELPER_SEGMENTEDPROPERTYREGISTRY_HPP__
