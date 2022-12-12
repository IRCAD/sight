/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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
#include "io/session/Helper.hpp"
#include "io/session/macros.hpp"

#include <data/SeriesSet.hpp>

namespace sight::io::session::detail::SeriesSet
{

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto series_set = Helper::safe_cast<data::SeriesSet>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::SeriesSet>(tree, 1);

    for(std::size_t index = 0, end = series_set->size() ; index < end ; ++index)
    {
        children[data::Series::classname() + std::to_string(index)] = series_set->at(index);
    }
}

//------------------------------------------------------------------------------

inline static data::SeriesSet::sptr read(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto series_set = Helper::cast_or_create<data::SeriesSet>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::SeriesSet>(tree, 0, 1);

    // Deserialize vector
    // Clearing is required in case the object is reused
    series_set->clear();

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& it = children.find(data::Series::classname() + std::to_string(index));

        if(it == children.cend())
        {
            break;
        }

        const auto& series = std::dynamic_pointer_cast<data::Series>(it->second);

        if(!series)
        {
            break;
        }

        series_set->push_back(series);
    }

    return series_set;
}

SIGHT_REGISTER_SERIALIZER(data::SeriesSet, write, read);

} // namespace sight::io::session::detail::SeriesSet
