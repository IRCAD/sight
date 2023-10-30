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

#include "io/dicom/helper/SegmentedPropertyRegistry.hpp"

#include "io/__/reader/csv_reader.hpp"
#include "io/dicom/helper/DicomCodedAttribute.hpp"

#include <core/log/logger.hpp>

#include <filesystem>

namespace sight::io::dicom::helper
{

//------------------------------------------------------------------------------

const segmented_property_registry::entry_t segmented_property_registry::DEFAULT_ENTRY_VALUE =
{{"(111176;DCM;Unspecified)", "(T-D000A;SRT;Anatomical Structure)", "", "", ""}};

//------------------------------------------------------------------------------

segmented_property_registry::segmented_property_registry()
= default;

//------------------------------------------------------------------------------

bool check_and_format_entry(
    const std::string& _structure_type,
    segmented_property_registry::entry_t& _entry,
    const core::log::logger::sptr& _logger
)
{
    bool result = true;

    // Check condition and add critical log on failure
    auto check_condition = [&](bool _condition, const std::string& _msg)
                           {
                               result &= _condition;

                               SIGHT_ERROR_IF(_msg, !_condition && !_logger);
                               if(!_condition && _logger)
                               {
                                   _logger->critical(_msg);
                               }
                           };

    // Structure type
    check_condition(!_structure_type.empty(), "Structure type of registry entries shall not be empty.");

    // Property type
    std::string& property_type = _entry[0];
    check_condition(!property_type.empty(), "Property Type shall not be empty for '" + _structure_type + "'.");
    check_condition(
        property_type.empty()
        || io::dicom::helper::dicom_coded_attribute::check_and_format_entry(property_type),
        "Coded entry is badly formatted : '" + property_type + "'. Please check registry."
    );

    // Property category
    std::string& property_category = _entry[1];
    check_condition(!property_category.empty(), "Property Category shall not be empty for '" + _structure_type + "'.");
    check_condition(
        property_category.empty()
        || io::dicom::helper::dicom_coded_attribute::check_and_format_entry(property_category),
        "Coded entry is badly formatted : '" + property_category + "'. Please check registry."
    );

    // Property type modifiers (may be empty)
    std::string& property_type_modifiers = _entry[2];
    check_condition(
        property_type_modifiers.empty()
        || io::dicom::helper::dicom_coded_attribute::check_and_format_entry(property_type_modifiers, true),
        "Coded entry is badly formatted : '" + property_type_modifiers + "'. Please check registry."
    );

    // Anatomic region
    std::string& anatomic_region = _entry[3];
    check_condition(
        property_category != "(M-01000;SRT;Morphologically Altered Structure)" || !anatomic_region.empty(),
        "Anatomic Region shall not be empty for altered structures. See '" + _structure_type + "'."
    );
    check_condition(
        anatomic_region.empty()
        || io::dicom::helper::dicom_coded_attribute::check_and_format_entry(anatomic_region),
        "Coded entry is badly formatted : '" + anatomic_region + "'. Please check registry."
    );

    // Anatomic region modifiers (may be empty)
    std::string& anatomic_region_modifiers = _entry[4];
    check_condition(
        anatomic_region_modifiers.empty()
        || io::dicom::helper::dicom_coded_attribute::check_and_format_entry(anatomic_region_modifiers, true),
        "Coded entry is badly formatted : '" + anatomic_region_modifiers + "'. Please check registry."
    );

    return result;
}

//------------------------------------------------------------------------------

bool segmented_property_registry::read_segmented_property_registry_file(
    const std::filesystem::path& _filepath,
    bool _omit_first_line,
    const core::log::logger::sptr& _logger
)
{
    if(std::filesystem::exists(_filepath))
    {
        bool result = true;

        // Read CSV
        io::reader::csv_reader reader(_filepath);

        const std::string separator = "|";
        auto tokens                 = reader.get_line(separator);

        if(_omit_first_line)
        {
            // First line is omitted (titles)
            tokens = reader.get_line(separator);
        }

        while(!tokens.empty())
        {
            const std::string& structure_type = tokens[0];

            // Each lines of valid csv file shall contain at least 6 elements.
            if(tokens.size() >= 6)
            {
                entry_t entry;

                // First element is skipped because it contains the title of csv file.
                std::copy_n(tokens.begin() + 1, 5, entry.begin());

                if(check_and_format_entry(structure_type, entry, _logger))
                {
                    m_registry[structure_type] = entry;
                }
                else
                {
                    result = false;
                }
            }
            else if(_logger)
            {
                _logger->critical("Entry badly formatted for structure '" + structure_type + "'.");
                result = false;
            }

            tokens = reader.get_line(separator);
        }

        // Check uniqueness of entries
        std::set<entry_t> uniqueness_set;
        for(const auto& entry : m_registry)
        {
            if(uniqueness_set.find(entry.second) != uniqueness_set.end())
            {
                const std::string msg = "Several structure types have the same attribute combination : {"
                                        + entry.second[0] + ";"
                                        + entry.second[1] + ";"
                                        + entry.second[2] + ";"
                                        + entry.second[3] + ";"
                                        + entry.second[4] + "}";

                SIGHT_ERROR_IF(msg, !_logger);
                if(_logger)
                {
                    _logger->critical(msg);
                }

                result = false;
            }

            uniqueness_set.insert(entry.second);
        }

        return result;
    }

    return false;
}

//------------------------------------------------------------------------------

bool segmented_property_registry::empty() const
{
    return m_registry.empty();
}

//------------------------------------------------------------------------------

std::size_t segmented_property_registry::count() const
{
    return m_registry.size();
}

//------------------------------------------------------------------------------

void segmented_property_registry::clear()
{
    m_registry.clear();
}

//------------------------------------------------------------------------------

bool segmented_property_registry::has_entry(const std::string& _structure_type) const
{
    return m_registry.find(_structure_type) != m_registry.end();
}

//------------------------------------------------------------------------------

segmented_property_registry::entry_t segmented_property_registry::get_entry(const std::string& _structure_type) const
{
    const auto it = m_registry.find(_structure_type);
    if(it != m_registry.end())
    {
        return it->second;
    }

    return DEFAULT_ENTRY_VALUE;
}

//------------------------------------------------------------------------------

std::string segmented_property_registry::get_entry_value(std::size_t _index, const std::string& _structure_type) const
{
    const auto it = m_registry.find(_structure_type);
    if(it != m_registry.end())
    {
        return it->second[_index];
    }

    return DEFAULT_ENTRY_VALUE[_index];
}

//------------------------------------------------------------------------------

std::string segmented_property_registry::get_property_type(const std::string& _structure_type) const
{
    return get_entry_value(0, _structure_type);
}

//------------------------------------------------------------------------------

std::string segmented_property_registry::get_property_category(const std::string& _structure_type) const
{
    return get_entry_value(1, _structure_type);
}

//------------------------------------------------------------------------------

std::string segmented_property_registry::get_property_type_modifiers(const std::string& _structure_type) const
{
    return get_entry_value(2, _structure_type);
}

//------------------------------------------------------------------------------

std::string segmented_property_registry::get_anatomic_region(const std::string& _structure_type) const
{
    return get_entry_value(3, _structure_type);
}

//------------------------------------------------------------------------------

std::string segmented_property_registry::get_anatomic_region_modifiers(const std::string& _structure_type) const
{
    return get_entry_value(4, _structure_type);
}

//------------------------------------------------------------------------------

std::string segmented_property_registry::get_structure_type(
    const std::string& _property_type,
    const std::string& _property_category,
    const std::string& _property_type_modifiers,
    const std::string& _anatomic_region,
    const std::string& _anatomic_region_modifiers
) const
{
    // Entry that we are looking for
    const entry_t entry = {{_property_type,
        _property_category,
        _property_type_modifiers,
        _anatomic_region,
        _anatomic_region_modifiers
    }
    };

    // Search predicate
    const auto predicate = [&](const entry_registry_t::value_type& _value) -> bool
                           {
                               return _value.second == entry;
                           };

    // Search for entry
    const auto it = std::find_if(m_registry.begin(), m_registry.end(), predicate);

    return (it != m_registry.end()) ? it->first : "";
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::helper
