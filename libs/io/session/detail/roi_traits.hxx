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

#include <data/Node.hpp>
#include <data/roi_traits.hpp>
#include <data/structure_traits.hpp>

namespace sight::io::session::detail::roi_traits
{

constexpr static auto s_Identifier {"Identifier"};
constexpr static auto s_EvaluatedExp {"EvaluatedExp"};
constexpr static auto s_MaskOpNode {"MaskOpNode"};
constexpr static auto s_StructureTraits {"structure_traits"};

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::object::csptr object,
    std::map<std::string, data::object::csptr>& children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto roiTraits = helper::safe_cast<data::roi_traits>(object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::roi_traits>(tree, 1);

    // Serialize attributes
    helper::write_string(tree, s_Identifier, roiTraits->getIdentifier());
    helper::write_string(tree, s_EvaluatedExp, roiTraits->getEvaluatedExp());
    children[s_MaskOpNode]      = roiTraits->getMaskOpNode();
    children[s_StructureTraits] = roiTraits->getStructureTraits();
}

//------------------------------------------------------------------------------

inline static data::roi_traits::sptr read(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::object::sptr>& children,
    data::object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto roiTraits = helper::cast_or_create<data::roi_traits>(object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::roi_traits>(tree, 0, 1);

    // Deserialize attributes
    roiTraits->setIdentifier(helper::read_string(tree, s_Identifier));
    roiTraits->setEvaluatedExp(helper::read_string(tree, s_EvaluatedExp));
    roiTraits->setMaskOpNode(std::dynamic_pointer_cast<data::Node>(children.at(s_MaskOpNode)));
    roiTraits->setStructureTraits(std::dynamic_pointer_cast<data::structure_traits>(children.at(s_StructureTraits)));

    return roiTraits;
}

SIGHT_REGISTER_SERIALIZER(data::roi_traits, write, read);

} // namespace sight::io::session::detail::roi_traits
