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

#include <data/Node.hpp>
#include <data/ROITraits.hpp>
#include <data/StructureTraits.hpp>

namespace sight::io::session::detail::ROITraits
{

constexpr static auto s_Identifier {"Identifier"};
constexpr static auto s_EvaluatedExp {"EvaluatedExp"};
constexpr static auto s_MaskOpNode {"MaskOpNode"};
constexpr static auto s_StructureTraits {"StructureTraits"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto roiTraits = Helper::safe_cast<data::ROITraits>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::ROITraits>(tree, 1);

    // Serialize attributes
    Helper::writeString(tree, s_Identifier, roiTraits->getIdentifier());
    Helper::writeString(tree, s_EvaluatedExp, roiTraits->getEvaluatedExp());
    children[s_MaskOpNode]      = roiTraits->getMaskOpNode();
    children[s_StructureTraits] = roiTraits->getStructureTraits();
}

//------------------------------------------------------------------------------

inline static data::ROITraits::sptr deserialize(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto roiTraits = Helper::cast_or_create<data::ROITraits>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::ROITraits>(tree, 0, 1);

    // Deserialize attributes
    roiTraits->setIdentifier(Helper::readString(tree, s_Identifier));
    roiTraits->setEvaluatedExp(Helper::readString(tree, s_EvaluatedExp));
    roiTraits->setMaskOpNode(std::dynamic_pointer_cast<data::Node>(children.at(s_MaskOpNode)));
    roiTraits->setStructureTraits(std::dynamic_pointer_cast<data::StructureTraits>(children.at(s_StructureTraits)));

    return roiTraits;
}

} // namespace sight::io::session::detail::ROITraits
