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

#include <data/color.hpp>
#include <data/structure_traits.hpp>

namespace sight::io::session::detail::structure_traits
{

constexpr static auto s_Type {"Type"};
constexpr static auto s_Class {"Class"};
constexpr static auto s_NativeExp {"NativeExp"};
constexpr static auto s_NativeGeometricExp {"NativeGeometricExp"};
constexpr static auto s_AttachmentType {"AttachmentType"};
constexpr static auto s_AnatomicRegion {"AnatomicRegion"};
constexpr static auto s_PropertyCategory {"PropertyCategory"};
constexpr static auto s_PropertyType {"PropertyType"};
constexpr static auto s_Category {"Category"};
constexpr static auto s_Categories {"Categories"};
constexpr static auto s_Color {"Color"};

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::object::csptr object,
    std::map<std::string, data::object::csptr>& children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto structureTraits = helper::safe_cast<data::structure_traits>(object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::structure_traits>(tree, 1);

    // Serialize attributes
    helper::write_string(tree, s_Type, structureTraits->getType());
    tree.put(s_Class, structureTraits->getClass());
    helper::write_string(tree, s_NativeExp, structureTraits->getNativeExp());
    helper::write_string(tree, s_NativeGeometricExp, structureTraits->getNativeGeometricExp());
    helper::write_string(tree, s_AttachmentType, structureTraits->getAttachmentType());
    helper::write_string(tree, s_AnatomicRegion, structureTraits->getAnatomicRegion());
    helper::write_string(tree, s_PropertyCategory, structureTraits->getPropertyCategory());
    helper::write_string(tree, s_PropertyType, structureTraits->getPropertyType());

    // Categories
    boost::property_tree::ptree categoriesTree;

    for(const auto& category : structureTraits->getCategories())
    {
        categoriesTree.add(s_Category, category);
    }

    tree.add_child(s_Categories, categoriesTree);

    // Color
    children[s_Color] = structureTraits->getColor();
}

//------------------------------------------------------------------------------

inline static data::structure_traits::sptr read(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::object::sptr>& children,
    data::object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto structureTraits = helper::cast_or_create<data::structure_traits>(object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::structure_traits>(tree, 0, 1);

    // Deserialize attributes
    structureTraits->setType(helper::read_string(tree, s_Type));
    structureTraits->setClass(static_cast<data::structure_traits::StructureClass>(tree.get<int>(s_Class)));
    structureTraits->setNativeExp(helper::read_string(tree, s_NativeExp));
    structureTraits->setNativeGeometricExp(helper::read_string(tree, s_NativeGeometricExp));
    structureTraits->setAttachmentType(helper::read_string(tree, s_AttachmentType));
    structureTraits->setAnatomicRegion(helper::read_string(tree, s_AnatomicRegion));
    structureTraits->setPropertyCategory(helper::read_string(tree, s_PropertyCategory));
    structureTraits->setPropertyType(helper::read_string(tree, s_PropertyType));

    // Categories
    // Clearing is required in case the object is reused
    data::structure_traits::CategoryContainer categories;

    for(const auto& categoryTree : tree.get_child(s_Categories))
    {
        categories.push_back(static_cast<data::structure_traits::Category>(categoryTree.second.get_value<int>()));
    }

    structureTraits->setCategories(categories);

    // Color
    structureTraits->setColor(std::dynamic_pointer_cast<data::color>(children.at(s_Color)));

    return structureTraits;
}

SIGHT_REGISTER_SERIALIZER(data::structure_traits, write, read);

} // namespace sight::io::session::detail::structure_traits
