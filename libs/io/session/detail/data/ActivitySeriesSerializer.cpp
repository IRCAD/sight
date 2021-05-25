/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#include "ActivitySeriesSerializer.hpp"

#include "SeriesSerializer.hpp"

#include <core/crypto/Base64.hpp>

#include <data/ActivitySeries.hpp>
#include <data/Composite.hpp>

namespace sight::io::session
{

namespace detail::data
{

/// Serialization function
void ActivitySeriesSerializer::serialize(
    const zip::ArchiveWriter::sptr& archive,
    boost::property_tree::ptree& tree,
    const sight::data::Object::csptr& object,
    std::map<std::string, sight::data::Object::csptr>& children,
    const core::crypto::secure_string& password
) const
{
    const auto& activitySeries = sight::data::ActivitySeries::dynamicCast(object);
    SIGHT_ASSERT(
        "Object '"
        << (object ? object->getClassname() : sight::data::Object::classname())
        << "' is not a '"
        << sight::data::ActivitySeries::classname()
        << "'",
        activitySeries
    );

    // Since ActivitySeries inherits from Series, we could use SeriesSerializer
    const SeriesSerializer seriesSerializer;
    seriesSerializer.serialize(archive, tree, activitySeries, children, password);

    // Now serialize the remaining
    // Serialize children properties
    children["Data"] = activitySeries->getData();

    // Serialize trivial properties
    writeToTree(tree, "ActivityConfigId", activitySeries->getActivityConfigId());
}

} // detail::data

} // namespace sight::io::session
