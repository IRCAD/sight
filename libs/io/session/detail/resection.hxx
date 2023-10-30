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

#include <data/plane_list.hpp>
#include <data/reconstruction.hpp>
#include <data/resection.hpp>

namespace sight::io::session::detail::resection
{

constexpr static auto NAME {"Name"};
constexpr static auto PLANE_LIST {"PlaneList"};
constexpr static auto IS_SAFE_PART {"IsSafePart"};
constexpr static auto IS_VALID {"IsValid"};
constexpr static auto IS_VISIBLE {"IsVisible"};

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer& /*unused*/,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& _children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto resection = helper::safe_cast<data::resection>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::resection>(_tree, 1);

    // Serialize attributes
    helper::write_string(_tree, NAME, resection->get_name());
    _tree.put(IS_SAFE_PART, resection->get_is_safe_part());
    _tree.put(IS_VALID, resection->get_is_valid());
    _tree.put(IS_VISIBLE, resection->get_is_visible());

    _children[PLANE_LIST] = resection->get_plane_list();

    // Serialize intputs
    std::size_t index = 0;
    for(const auto& input : resection->get_inputs())
    {
        _children["I" + std::to_string(index++)] = input;
    }

    // Serialize outputs
    index = 0;
    for(const auto& output : resection->get_outputs())
    {
        _children["O" + std::to_string(index++)] = output;
    }
}

//------------------------------------------------------------------------------

inline static data::resection::sptr read(
    zip::archive_reader& /*unused*/,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& _children,
    data::object::sptr _object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto resection = helper::cast_or_create<data::resection>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::resection>(_tree, 0, 1);

    // Deserialize attributes
    resection->set_name(helper::read_string(_tree, NAME));
    resection->set_is_safe_part(_tree.get<bool>(IS_SAFE_PART));
    resection->set_is_valid(_tree.get<bool>(IS_VALID));
    resection->set_is_visible(_tree.get<bool>(IS_VISIBLE));

    resection->set_plane_list(std::dynamic_pointer_cast<data::plane_list>(_children.at(PLANE_LIST)));

    // Deserialize intputs / outputs
    auto& inputs = resection->get_inputs();
    inputs.clear();

    auto& outputs = resection->get_outputs();
    outputs.clear();

    for(std::size_t index = 0, end = _children.size() ; index < end ; ++index)
    {
        const auto& input_it  = _children.find("I" + std::to_string(index));
        const auto& output_it = _children.find("O" + std::to_string(index));

        if(input_it == _children.cend() && output_it == _children.cend())
        {
            break;
        }

        if(input_it != _children.cend())
        {
            inputs.push_back(std::dynamic_pointer_cast<data::reconstruction>(input_it->second));
        }

        if(output_it != _children.cend())
        {
            outputs.push_back(std::dynamic_pointer_cast<data::reconstruction>(output_it->second));
        }
    }

    return resection;
}

SIGHT_REGISTER_SERIALIZER(data::resection, write, read);

} // namespace sight::io::session::detail::resection
