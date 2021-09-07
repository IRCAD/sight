/************************************************************************
 *
 * Copyright (C) 2017-2021 IRCAD France
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

#include "io/dicom/helper/SegmentedPropertyRegistry.hpp"

#include "io/dicom/helper/CsvIO.hpp"
#include "io/dicom/helper/DicomCodedAttribute.hpp"

#include <core/log/Logger.hpp>

#include <filesystem>

namespace sight::io::dicom
{

namespace helper
{

//------------------------------------------------------------------------------

const SegmentedPropertyRegistry::EntryType SegmentedPropertyRegistry::s_DEFAULT_ENTRY_VALUE =
{{"(111176;DCM;Unspecified)", "(T-D000A;SRT;Anatomical Structure)", "", "", ""}};

//------------------------------------------------------------------------------

SegmentedPropertyRegistry::SegmentedPropertyRegistry()
{
}

//------------------------------------------------------------------------------

bool checkAndFormatEntry(
    const std::string& structureType,
    SegmentedPropertyRegistry::EntryType& entry,
    const core::log::Logger::sptr& logger
)
{
    bool result = true;

    // Check condition and add critical log on failure
    auto checkCondition = [&](bool condition, const std::string& msg)
                          {
                              result &= condition;

                              SIGHT_ERROR_IF(msg, !condition && !logger);
                              if(!condition && logger)
                              {
                                  logger->critical(msg);
                              }
                          };

    // Structure type
    checkCondition(!structureType.empty(), "Structure type of registry entries shall not be empty.");

    // Property type
    std::string& propertyType = entry[0];
    checkCondition(!propertyType.empty(), "Property Type shall not be empty for '" + structureType + "'.");
    checkCondition(
        propertyType.empty()
        || io::dicom::helper::DicomCodedAttribute::checkAndFormatEntry(propertyType),
        "Coded entry is badly formatted : '" + propertyType + "'. Please check registry."
    );

    // Property category
    std::string& propertyCategory = entry[1];
    checkCondition(!propertyCategory.empty(), "Property Category shall not be empty for '" + structureType + "'.");
    checkCondition(
        propertyCategory.empty()
        || io::dicom::helper::DicomCodedAttribute::checkAndFormatEntry(propertyCategory),
        "Coded entry is badly formatted : '" + propertyCategory + "'. Please check registry."
    );

    // Property type modifiers (may be empty)
    std::string& propertyTypeModifiers = entry[2];
    checkCondition(
        propertyTypeModifiers.empty()
        || io::dicom::helper::DicomCodedAttribute::checkAndFormatEntry(propertyTypeModifiers, true),
        "Coded entry is badly formatted : '" + propertyTypeModifiers + "'. Please check registry."
    );

    // Anatomic region
    std::string& anatomicRegion = entry[3];
    checkCondition(
        propertyCategory != "(M-01000;SRT;Morphologically Altered Structure)" || !anatomicRegion.empty(),
        "Anatomic Region shall not be empty for altered structures. See '" + structureType + "'."
    );
    checkCondition(
        anatomicRegion.empty()
        || io::dicom::helper::DicomCodedAttribute::checkAndFormatEntry(anatomicRegion),
        "Coded entry is badly formatted : '" + anatomicRegion + "'. Please check registry."
    );

    // Anatomic region modifiers (may be empty)
    std::string& anatomicRegionModifiers = entry[4];
    checkCondition(
        anatomicRegionModifiers.empty()
        || io::dicom::helper::DicomCodedAttribute::checkAndFormatEntry(anatomicRegionModifiers, true),
        "Coded entry is badly formatted : '" + anatomicRegionModifiers + "'. Please check registry."
    );

    return result;
}

//------------------------------------------------------------------------------

bool SegmentedPropertyRegistry::readSegmentedPropertyRegistryFile(
    const std::filesystem::path& filepath,
    bool omitFirstLine,
    const core::log::Logger::sptr& logger
)
{
    const std::string filepathStr = filepath.string();
    if(std::filesystem::exists(filepath))
    {
        std::ifstream csvStream(filepathStr);
        return readSegmentedPropertyRegistryFile(csvStream, omitFirstLine, logger);
    }

    return false;
}

//------------------------------------------------------------------------------

bool SegmentedPropertyRegistry::readSegmentedPropertyRegistryFile(
    std::istream& csvStream,
    bool omitFirstLine,
    const core::log::Logger::sptr& logger
)
{
    bool result = true;

    // Read CSV
    io::dicom::helper::CsvIO reader(csvStream);

    const std::string separator = "|";
    auto tokens                 = reader.getLine(separator);

    if(omitFirstLine)
    {
        // First line is omitted (titles)
        tokens = reader.getLine(separator);
    }

    while(!tokens.empty())
    {
        const std::string structureType = tokens[0];

        if(tokens.size() >= 6)
        {
            EntryType entry;
            std::copy_n(tokens.begin() + 1, 5, entry.begin());
            if(checkAndFormatEntry(structureType, entry, logger))
            {
                m_registry[structureType] = entry;
            }
            else
            {
                result = false;
            }
        }
        else if(logger)
        {
            logger->critical("Entry badly formatted for structure '" + structureType + "'.");
            result = false;
        }

        tokens = reader.getLine(separator);
    }

    // Check uniqueness of entries
    std::set<EntryType> uniquenessSet;
    for(const auto& entry : m_registry)
    {
        if(uniquenessSet.find(entry.second) != uniquenessSet.end())
        {
            const std::string msg = "Several structure types have the same attribute combination : {"
                                    + entry.second[0] + ";"
                                    + entry.second[1] + ";"
                                    + entry.second[2] + ";"
                                    + entry.second[3] + ";"
                                    + entry.second[4] + "}";

            SIGHT_ERROR_IF(msg, !logger);
            if(logger)
            {
                logger->critical(msg);
            }

            result = false;
        }

        uniquenessSet.insert(entry.second);
    }

    return result;
}

//------------------------------------------------------------------------------

bool SegmentedPropertyRegistry::empty() const
{
    return m_registry.empty();
}

//------------------------------------------------------------------------------

std::size_t SegmentedPropertyRegistry::count() const
{
    return m_registry.size();
}

//------------------------------------------------------------------------------

void SegmentedPropertyRegistry::clear()
{
    m_registry.clear();
}

//------------------------------------------------------------------------------

bool SegmentedPropertyRegistry::hasEntry(const std::string& structureType) const
{
    return m_registry.find(structureType) != m_registry.end();
}

//------------------------------------------------------------------------------

SegmentedPropertyRegistry::EntryType SegmentedPropertyRegistry::getEntry(const std::string& structureType) const
{
    const auto it = m_registry.find(structureType);
    if(it != m_registry.end())
    {
        return it->second;
    }

    return s_DEFAULT_ENTRY_VALUE;
}

//------------------------------------------------------------------------------

#define GET_ENTRY_VALUE(index) \
    const auto it = m_registry.find(structureType); \
    if(it != m_registry.end()) \
    { \
        return it->second[index]; \
    } \
    return s_DEFAULT_ENTRY_VALUE[index];

//------------------------------------------------------------------------------

std::string SegmentedPropertyRegistry::getPropertyType(const std::string& structureType) const
{
    GET_ENTRY_VALUE(0);
}

//------------------------------------------------------------------------------

std::string SegmentedPropertyRegistry::getPropertyCategory(const std::string& structureType) const
{
    GET_ENTRY_VALUE(1);
}

//------------------------------------------------------------------------------

std::string SegmentedPropertyRegistry::getPropertyTypeModifiers(const std::string& structureType) const
{
    GET_ENTRY_VALUE(2);
}

//------------------------------------------------------------------------------

std::string SegmentedPropertyRegistry::getAnatomicRegion(const std::string& structureType) const
{
    GET_ENTRY_VALUE(3);
}

//------------------------------------------------------------------------------

std::string SegmentedPropertyRegistry::getAnatomicRegionModifiers(const std::string& structureType) const
{
    GET_ENTRY_VALUE(4);
}

#undef GET_ENTRY_VALUE

//------------------------------------------------------------------------------

std::string SegmentedPropertyRegistry::getStructureType(
    const std::string& propertyType,
    const std::string& propertyCategory,
    const std::string& propertyTypeModifiers,
    const std::string& anatomicRegion,
    const std::string& anatomicRegionModifiers
) const
{
    // Entry that we are looking for
    const EntryType entry = {{propertyType,
        propertyCategory,
        propertyTypeModifiers,
        anatomicRegion,
        anatomicRegionModifiers
    }
    };

    // Search predicate
    const auto predicate = [&](const EntryRegistryType::value_type& value) -> bool
                           {
                               return value.second == entry;
                           };

    // Search for entry
    const auto it = std::find_if(m_registry.begin(), m_registry.end(), predicate);

    return (it != m_registry.end()) ? it->first : "";
}

//------------------------------------------------------------------------------

} //namespace helper

} //namespace sight::io::dicom
