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

#pragma once

#include "io/session/config.hpp"
#include "io/session/detail/Helper.hpp"

#include <data/Histogram.hpp>

namespace sight::io::session
{

namespace detail::Histogram
{

constexpr static auto s_Value {"Value"};
constexpr static auto s_Values {"Values"};
constexpr static auto s_BinsWidth {"BinsWidth"};
constexpr static auto s_MinValue {"MinValue"};
constexpr static auto s_MaxValue {"MaxValue"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter&,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>&,
    const core::crypto::secure_string& = ""
)
{
    const auto histogram = Helper::safeCast<data::Histogram>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Histogram>(tree, 1);

    // Serialize values
    boost::property_tree::ptree valuesTree;
    for(const auto& value : histogram->getValues())
    {
        valuesTree.add(s_Value, value);
    }

    tree.add_child(s_Values, valuesTree);

    tree.put(s_BinsWidth, histogram->getBinsWidth());
    tree.put(s_MinValue, histogram->getMinValue());
    tree.put(s_MaxValue, histogram->getMaxValue());
}

//------------------------------------------------------------------------------

inline static data::Histogram::sptr deserialize(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>&,
    data::Object::sptr object,
    const core::crypto::secure_string& = ""
)
{
    // Create or reuse the object
    auto histogram = Helper::safeCast<data::Histogram>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Histogram>(tree, 0, 1);

    // Deserialize values
    std::vector<long> values;

    for(const auto& valueTree : tree.get_child(s_Values))
    {
        const auto& value = valueTree.second.get_value<long>();
        values.push_back(value);
    }

    histogram->setValues(values);

    histogram->setBinsWidth(tree.get<float>(s_BinsWidth));
    histogram->setMinValue(tree.get<float>(s_MinValue));
    histogram->setMaxValue(tree.get<float>(s_MaxValue));

    return histogram;
}

} // namespace detail::Histogram

} // namespace sight::io
