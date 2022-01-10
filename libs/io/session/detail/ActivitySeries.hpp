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
#include "io/session/detail/Series.hpp"
#include "io/session/Helper.hpp"

#include <data/ActivitySeries.hpp>
#include <data/Composite.hpp>

namespace sight::io::session
{

namespace detail::ActivitySeries
{

constexpr static auto s_Data {"Data"};
constexpr static auto s_ActivityConfigId {"ActivityConfigId"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& password = ""
)
{
    const auto activitySeries = Helper::safeCast<data::ActivitySeries>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::ActivitySeries>(tree, 1);

    // Since ActivitySeries inherits from Series, we could use SeriesSerializer
    detail::Series::serialize(archive, tree, activitySeries, children, password);

    // Now serialize the remaining
    // Serialize children properties
    children[s_Data] = activitySeries->getData();

    // Serialize trivial properties
    Helper::writeString(tree, s_ActivityConfigId, activitySeries->getActivityConfigId());
}

//------------------------------------------------------------------------------

inline static data::ActivitySeries::sptr deserialize(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto activitySeries = Helper::safeCast<data::ActivitySeries>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::ActivitySeries>(tree, 0, 1);

    // Since ActivitySeries inherits from Series, we could use SeriesDeserializer
    Series::deserialize(archive, tree, children, activitySeries, password);

    // Deserialize the remaining
    // Deserialize children properties
    activitySeries->setData(std::dynamic_pointer_cast<data::Composite>(children.at(s_Data)));

    // Deserialize trivial properties
    activitySeries->setActivityConfigId(Helper::readString(tree, s_ActivityConfigId));

    return activitySeries;
}

} // namespace detail::ActivitySeries

} // namespace sight::io
