/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "io/dicom/helper/tags.hpp"

#include "io/__/reader/csv_reader.hpp"
#include "io/dicom/exception/InvalidTag.hpp"

#include <core/exceptionmacros.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <filesystem>
#include <fstream>

namespace sight::io::dicom::helper
{

//------------------------------------------------------------------------------

gdcm::Tag get_gdcm_tag(const std::string& _group, const std::string& _element)
{
    SIGHT_ASSERT("Group and element can not be empty", !_group.empty() && !_element.empty());

    using dest_t = std::uint16_t;
    dest_t group_dest   = 0;
    dest_t element_dest = 0;

    try
    {
        const std::uint64_t group_l   = std::stoul(_group, nullptr, 16);
        const std::uint64_t element_l = std::stoul(_element, nullptr, 16);

        group_dest   = boost::numeric_cast<dest_t>(group_l);
        element_dest = boost::numeric_cast<dest_t>(element_l);
    }
    catch(std::out_of_range& e)
    {
        SIGHT_THROW_EXCEPTION(
            io::dicom::exception::invalid_tag(
                std::string("Unable to read DICOM tag from '") + _group + "," + _element + "' : " + e.what()
            )

        );
    }
    catch(std::invalid_argument& e)
    {
        SIGHT_THROW_EXCEPTION(
            io::dicom::exception::invalid_tag(
                std::string("Unable to read DICOM tag from '") + _group + "," + _element + "' : " + e.what()
            )

        );
    }
    catch(boost::bad_numeric_cast& e)
    {
        SIGHT_THROW_EXCEPTION(
            io::dicom::exception::invalid_tag(
                std::string("Unable to read DICOM tag from '") + _group + "," + _element + "' : " + e.what()
            )

        );
    }

    return {group_dest, element_dest};
}

//------------------------------------------------------------------------------

private_tag_vec_t load_private_tags(const std::filesystem::path& _tags_path)
{
    SIGHT_ASSERT(
        "File '" + _tags_path.string() + "' must exists",
        std::filesystem::exists(_tags_path) && std::filesystem::is_regular_file(_tags_path)
    );

    private_tag_vec_t private_tags;
    io::reader::csv_reader reader(_tags_path);
    io::reader::csv_reader::token_container_t tag = reader.get_line();

    while(!tag.empty())
    {
        SIGHT_WARN_IF(
            "Unexpected token count : " << tag.size() << " (3 expected : group, element, manufacturer)",
            tag.size() != 3
        );
        SIGHT_THROW_IF("Unable to read private tag file", tag.size() < 2);

        private_tags.push_back(io::dicom::helper::get_gdcm_tag(tag[0], tag[1]));
        tag = reader.get_line();
    }

    return private_tags;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::helper
