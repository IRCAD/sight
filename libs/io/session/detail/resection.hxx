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

constexpr static auto s_Name {"Name"};
constexpr static auto s_PlaneList {"PlaneList"};
constexpr static auto s_IsSafePart {"IsSafePart"};
constexpr static auto s_IsValid {"IsValid"};
constexpr static auto s_IsVisible {"IsVisible"};

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& /*unused*/,
    boost::property_tree::ptree& tree,
    data::object::csptr object,
    std::map<std::string, data::object::csptr>& children,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    const auto resection = helper::safe_cast<data::resection>(object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::resection>(tree, 1);

    // Serialize attributes
    helper::write_string(tree, s_Name, resection->getName());
    tree.put(s_IsSafePart, resection->getIsSafePart());
    tree.put(s_IsValid, resection->getIsValid());
    tree.put(s_IsVisible, resection->getIsVisible());

    children[s_PlaneList] = resection->getPlaneList();

    // Serialize intputs
    std::size_t index = 0;
    for(const auto& input : resection->getInputs())
    {
        children["I" + std::to_string(index++)] = input;
    }

    // Serialize outputs
    index = 0;
    for(const auto& output : resection->getOutputs())
    {
        children["O" + std::to_string(index++)] = output;
    }
}

//------------------------------------------------------------------------------

inline static data::resection::sptr read(
    zip::ArchiveReader& /*unused*/,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::object::sptr>& children,
    data::object::sptr object,
    const core::crypto::secure_string& /*unused*/ = ""
)
{
    // Create or reuse the object
    auto resection = helper::cast_or_create<data::resection>(object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::resection>(tree, 0, 1);

    // Deserialize attributes
    resection->setName(helper::read_string(tree, s_Name));
    resection->setIsSafePart(tree.get<bool>(s_IsSafePart));
    resection->setIsValid(tree.get<bool>(s_IsValid));
    resection->setIsVisible(tree.get<bool>(s_IsVisible));

    resection->setPlaneList(std::dynamic_pointer_cast<data::plane_list>(children.at(s_PlaneList)));

    // Deserialize intputs / outputs
    auto& inputs = resection->getInputs();
    inputs.clear();

    auto& outputs = resection->getOutputs();
    outputs.clear();

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& inputIt  = children.find("I" + std::to_string(index));
        const auto& outputIt = children.find("O" + std::to_string(index));

        if(inputIt == children.cend() && outputIt == children.cend())
        {
            break;
        }

        if(inputIt != children.cend())
        {
            inputs.push_back(std::dynamic_pointer_cast<data::reconstruction>(inputIt->second));
        }

        if(outputIt != children.cend())
        {
            outputs.push_back(std::dynamic_pointer_cast<data::reconstruction>(outputIt->second));
        }
    }

    return resection;
}

SIGHT_REGISTER_SERIALIZER(data::resection, write, read);

} // namespace sight::io::session::detail::resection
