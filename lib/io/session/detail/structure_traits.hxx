/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

#include <sight/io/session/config.hpp>

#include "io/session/helper.hpp"
#include "io/session/macros.hpp"

#include <data/color.hpp>
#include <data/structure_traits.hpp>

namespace sight::io::session::detail::structure_traits
{

constexpr static auto TYPE {"Type"};
constexpr static auto CLASS {"Class"};
constexpr static auto NATIVE_EXP {"NativeExp"};
constexpr static auto NATIVE_GEOMETRIC_EXP {"NativeGeometricExp"};
constexpr static auto ATTACHMENT_TYPE {"attachment_t"};
constexpr static auto ANATOMIC_REGION {"AnatomicRegion"};
constexpr static auto PROPERTY_CATEGORY {"PropertyCategory"};
constexpr static auto PROPERTY_TYPE {"property_t"};
constexpr static auto CATEGORY {"Category"};
constexpr static auto CATEGORIES {"Categories"};
constexpr static auto COLOR {"Color"};

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer& /*unused*/,
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
    helper::write_string(_tree, TYPE, structure_traits->type());
    _tree.put(CLASS, structure_traits->get_class());
    helper::write_string(_tree, NATIVE_EXP, structure_traits->get_native_exp());
    helper::write_string(_tree, NATIVE_GEOMETRIC_EXP, structure_traits->get_native_geometric_exp());
    helper::write_string(_tree, ATTACHMENT_TYPE, structure_traits->get_attachment_type());
    helper::write_string(_tree, ANATOMIC_REGION, structure_traits->get_anatomic_region());
    helper::write_string(_tree, PROPERTY_CATEGORY, structure_traits->get_property_category());
    helper::write_string(_tree, PROPERTY_TYPE, structure_traits->get_property_type());

    // Categories
    boost::property_tree::ptree categories_tree;

    for(const auto& category : structure_traits->get_categories())
    {
        categories_tree.add(CATEGORY, category);
    }

    _tree.add_child(CATEGORIES, categories_tree);

    // Color
    _children[COLOR] = structure_traits->get_color();
}

//------------------------------------------------------------------------------

inline static data::structure_traits::sptr read(
    zip::archive_reader& /*unused*/,
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
    structure_traits->set_type(helper::read_string(_tree, TYPE));
    structure_traits->set_class(static_cast<data::structure_traits::structure_class>(_tree.get<int>(CLASS)));
    structure_traits->set_native_exp(helper::read_string(_tree, NATIVE_EXP));
    structure_traits->set_native_geometric_exp(helper::read_string(_tree, NATIVE_GEOMETRIC_EXP));
    structure_traits->set_attachment_type(helper::read_string(_tree, ATTACHMENT_TYPE));
    structure_traits->set_anatomic_region(helper::read_string(_tree, ANATOMIC_REGION));
    structure_traits->set_property_category(helper::read_string(_tree, PROPERTY_CATEGORY));
    structure_traits->set_property_type(helper::read_string(_tree, PROPERTY_TYPE));

    // Categories
    // Clearing is required in case the object is reused
    data::structure_traits::category_container_t categories;

    for(const auto& category_tree : _tree.get_child(CATEGORIES))
    {
        categories.push_back(static_cast<data::structure_traits::category>(category_tree.second.get_value<int>()));
    }

    structure_traits->set_categories(categories);

    // Color
    structure_traits->set_color(std::dynamic_pointer_cast<data::color>(_children.at(COLOR)));

    return structure_traits;
}

SIGHT_REGISTER_SERIALIZER(data::structure_traits, write, read);

} // namespace sight::io::session::detail::structure_traits
