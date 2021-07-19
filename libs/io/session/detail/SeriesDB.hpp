/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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
#include "io/session/detail/Helper.hpp"

#include <data/Series.hpp>
#include <data/SeriesDB.hpp>

namespace sight::io::session
{

namespace detail::SeriesDB
{

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& password = ""
)
{
    const auto seriesDB = Helper::safeCast<data::SeriesDB>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::SeriesDB>(tree, 1);

    std::size_t index = 0;
    for(const auto& child : seriesDB->getContainer())
    {
        children[data::Series::classname() + std::to_string(index++)] = child;
    }
}

//------------------------------------------------------------------------------

inline static data::SeriesDB::sptr deserialize(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto seriesDB = Helper::safeCast<data::SeriesDB>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::SeriesDB>(tree, 0, 1);

    // Deserialize vector
    auto& series = seriesDB->getContainer();

    // Clearing is required in case the object is reused
    series.clear();

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& it = children.find(data::Series::classname() + std::to_string(index));

        if(it == children.cend())
        {
            break;
        }

        series.push_back(data::Series::dynamicCast(it->second));
    }

    return seriesDB;
}

} // namespace detail::SeriesDB

} // namespace sight::io
