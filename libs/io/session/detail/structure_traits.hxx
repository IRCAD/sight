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
constexpr static auto s_AttachmentType {"attachment_t"};
constexpr static auto s_AnatomicRegion {"AnatomicRegion"};
constexpr static auto s_PropertyCategory {"PropertyCategory"};
constexpr static auto s_PropertyType {"property_t"};
constexpr static auto s_Category {"Category"};
constexpr static auto s_Categories {"Categories"};
constexpr static auto s_Color {"Color"};

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& _children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto structure_traits = helper::safe_cast<data::structure_traits>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::structure_traits>(_tree, 1);

    // Serialize attributes
    helper::write_string(_tree, s_Type, structure_traits->getType());
    _tree.put(s_Class, structure_traits->getClass());
    helper::write_string(_tree, s_NativeExp, structure_traits->getNativeExp());
    helper::write_string(_tree, s_NativeGeometricExp, structure_traits->getNativeGeometricExp());
    helper::write_string(_tree, s_AttachmentType, structure_traits->get_attachment_type());
    helper::write_string(_tree, s_AnatomicRegion, structure_traits->getAnatomicRegion());
    helper::write_string(_tree, s_PropertyCategory, structure_traits->getPropertyCategory());
    helper::write_string(_tree, s_PropertyType, structure_traits->get_property_type());

    // Categories
    boost::property_tree::ptree categories_tree;

    for(const auto& category : structure_traits->getCategories())
    {
        categories_tree.add(s_Category, category);
    }

    _tree.add_child(s_Categories, categories_tree);

    // Color
    _children[s_Color] = structure_traits->getColor();
}

//------------------------------------------------------------------------------

inline static data::structure_traits::sptr read(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& _children,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto structure_traits = helper::cast_or_create<data::structure_traits>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::structure_traits>(_tree, 0, 1);

    // Deserialize attributes
    structure_traits->setType(helper::read_string(_tree, s_Type));
    structure_traits->setClass(static_cast<data::structure_traits::StructureClass>(_tree.get<int>(s_Class)));
    structure_traits->setNativeExp(helper::read_string(_tree, s_NativeExp));
    structure_traits->setNativeGeometricExp(helper::read_string(_tree, s_NativeGeometricExp));
    structure_traits->set_attachment_type(helper::read_string(_tree, s_AttachmentType));
    structure_traits->setAnatomicRegion(helper::read_string(_tree, s_AnatomicRegion));
    structure_traits->setPropertyCategory(helper::read_string(_tree, s_PropertyCategory));
    structure_traits->set_property_type(helper::read_string(_tree, s_PropertyType));

    // Categories
    // Clearing is required in case the object is reused
    data::structure_traits::CategoryContainer categories;

    for(const auto& category_tree : _tree.get_child(s_Categories))
    {
        categories.push_back(static_cast<data::structure_traits::Category>(category_tree.second.get_value<int>()));
    }

    structure_traits->setCategories(categories);

    // Color
    structure_traits->setColor(std::dynamic_pointer_cast<data::color>(_children.at(s_Color)));

    return structure_traits;
}

SIGHT_REGISTER_SERIALIZER(data::structure_traits, write, read);

} // namespace sight::io::session::detail::structure_traits
