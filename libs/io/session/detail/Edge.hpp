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

#include <data/Edge.hpp>

namespace sight::io::session
{

namespace detail::Edge
{

constexpr static auto s_FromPortID {"FromPortID"};
constexpr static auto s_ToPortID {"ToPortID"};
constexpr static auto s_Nature {"Nature"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& password = ""
)
{
    const auto edge = Helper::safeCast<sight::data::Edge>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<sight::data::Edge>(tree, 1);

    Helper::writeString(tree, s_FromPortID, edge->getFromPortID(), password);
    Helper::writeString(tree, s_ToPortID, edge->getToPortID(), password);
    Helper::writeString(tree, s_Nature, edge->getNature(), password);
}

//------------------------------------------------------------------------------

inline static data::Edge::sptr deserialize(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto edge = Helper::safeCast<sight::data::Edge>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<sight::data::Edge>(tree, 0, 1);

    edge->setIdentifiers(
        Helper::readString(tree, s_FromPortID, password),
        Helper::readString(tree, s_ToPortID, password)
    );

    edge->setNature(Helper::readString(tree, s_Nature, password));

    return edge;
}

} // namespace detail::Edge

} // namespace sight::io
