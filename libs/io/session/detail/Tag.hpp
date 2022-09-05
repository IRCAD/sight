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

#include <data/PointList.hpp>
#include <data/Tag.hpp>

namespace sight::io::session::detail::Tag
{

constexpr static auto s_PointList {"PointList"};
constexpr static auto s_Type {"Type"};
constexpr static auto s_Size {"Size"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto tag = Helper::safeCast<data::Tag>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Tag>(tree, 1);

    Helper::writeString(tree, s_Type, tag->getType());
    tree.put(s_Size, tag->getSize());
    children[s_PointList] = tag->getPointList();
}

//------------------------------------------------------------------------------

inline static data::Tag::sptr deserialize(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto tag = Helper::safeCast<data::Tag>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Tag>(tree, 0, 1);

    tag->setType(Helper::readString(tree, s_Type));
    tag->setSize(tree.get<double>(s_Size));
    tag->setPointList(std::dynamic_pointer_cast<data::PointList>(children.at(s_PointList)));

    return tag;
}

} // namespace sight::io::session::detail::Tag
