/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
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

#include "io/base/reader/CsvReader.hpp"
#include "io/dicom/exception/InvalidTag.hpp"

#include <core/exceptionmacros.hpp>

#include <boost/numeric/conversion/cast.hpp>

#include <filesystem>
#include <fstream>

namespace sight::io::dicom::helper
{

//------------------------------------------------------------------------------

gdcm::Tag getGdcmTag(const std::string& group, const std::string& element)
{
    SIGHT_ASSERT("Group and element can not be empty", !group.empty() && !element.empty());

    using DestType = std::uint16_t;
    DestType groupDest   = 0;
    DestType elementDest = 0;

    try
    {
        const std::uint64_t groupL   = std::stoul(group, nullptr, 16);
        const std::uint64_t elementL = std::stoul(element, nullptr, 16);

        groupDest   = boost::numeric_cast<DestType>(groupL);
        elementDest = boost::numeric_cast<DestType>(elementL);
    }
    catch(std::out_of_range& e)
    {
        SIGHT_THROW_EXCEPTION(
            io::dicom::exception::InvalidTag(
                std::string("Unable to read DICOM tag from '").append(group).append(",").append(element).append("' : ")
                .append(e.what())
            )
        );
    }
    catch(std::invalid_argument& e)
    {
        SIGHT_THROW_EXCEPTION(
            io::dicom::exception::InvalidTag(
                std::string("Unable to read DICOM tag from '").append(group).append(",").append(element).append("' : ")
                .append(e.what())
            )
        );
    }
    catch(boost::bad_numeric_cast& e)
    {
        SIGHT_THROW_EXCEPTION(
            io::dicom::exception::InvalidTag(
                std::string("Unable to read DICOM tag from '").append(group).append(",").append(element).append("' : ")
                .append(e.what())
            )
        );
    }

    return {groupDest, elementDest};
}

//------------------------------------------------------------------------------

PrivateTagVecType loadPrivateTags(const std::filesystem::path& tagsPath)
{
    SIGHT_ASSERT(
        "File '" + tagsPath.string() + "' must exists",
        std::filesystem::exists(tagsPath) && std::filesystem::is_regular_file(tagsPath)
    );

    PrivateTagVecType privateTags;
    io::base::reader::CsvReader reader(tagsPath);
    io::base::reader::CsvReader::TokenContainerType tag = reader.getLine();

    while(!tag.empty())
    {
        SIGHT_WARN_IF(
            "Unexpected token count : " << tag.size() << " (3 expected : group, element, manufacturer)",
            tag.size() != 3
        );
        SIGHT_THROW_IF("Unable to read private tag file", tag.size() < 2);

        privateTags.push_back(io::dicom::helper::getGdcmTag(tag[0], tag[1]));
        tag = reader.getLine();
    }

    return privateTags;
}

//------------------------------------------------------------------------------

} // namespace sight::io::dicom::helper
