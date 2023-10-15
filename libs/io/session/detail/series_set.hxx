/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "io/session/config.hpp"
#include "io/session/helper.hpp"
#include "io/session/macros.hpp"

#include <data/series_set.hpp>

namespace sight::io::session::detail::series_set
{

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::object::csptr object,
    std::map<std::string, data::object::csptr>& children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto series_set = helper::safe_cast<data::series_set>(object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::series_set>(tree, 1);

    for(std::size_t index = 0, end = series_set->size() ; index < end ; ++index)
    {
        children[data::series::classname() + std::to_string(index)] = series_set->at(index);
    }
}

//------------------------------------------------------------------------------

inline static data::series_set::sptr read(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::object::sptr>& children,
    data::object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto series_set = helper::cast_or_create<data::series_set>(object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::series_set>(tree, 0, 1);

    // Deserialize vector
    // Clearing is required in case the object is reused
    series_set->clear();

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& it = children.find(data::series::classname() + std::to_string(index));

        if(it == children.cend())
        {
            break;
        }

        const auto& series = std::dynamic_pointer_cast<data::series>(it->second);

        if(!series)
        {
            break;
        }

        series_set->push_back(series);
    }

    return series_set;
}

SIGHT_REGISTER_SERIALIZER(data::series_set, write, read);

} // namespace sight::io::session::detail::series_set
