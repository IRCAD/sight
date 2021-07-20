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

#include <data/PlaneList.hpp>
#include <data/Reconstruction.hpp>
#include <data/Resection.hpp>

namespace sight::io::session
{

namespace detail::Resection
{

constexpr static auto s_Name {"Name"};
constexpr static auto s_PlaneList {"PlaneList"};
constexpr static auto s_IsSafePart {"IsSafePart"};
constexpr static auto s_IsValid {"IsValid"};
constexpr static auto s_IsVisible {"IsVisible"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter&,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& password = ""
)
{
    const auto resection = Helper::safeCast<data::Resection>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Resection>(tree, 1);

    // Serialize attributes
    Helper::writeString(tree, s_Name, resection->getName(), password);
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

inline static data::Resection::sptr deserialize(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto resection = Helper::safeCast<data::Resection>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Resection>(tree, 0, 1);

    // Deserialize attributes
    resection->setName(Helper::readString(tree, s_Name, password));
    resection->setIsSafePart(tree.get<bool>(s_IsSafePart));
    resection->setIsValid(tree.get<bool>(s_IsValid));
    resection->setIsVisible(tree.get<bool>(s_IsVisible));

    resection->setPlaneList(std::dynamic_pointer_cast<data::PlaneList>(children.at(s_PlaneList)));

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
            inputs.push_back(std::dynamic_pointer_cast<data::Reconstruction>(inputIt->second));
        }

        if(outputIt != children.cend())
        {
            outputs.push_back(std::dynamic_pointer_cast<data::Reconstruction>(outputIt->second));
        }
    }

    return resection;
}

} // namespace detail::Resection

} // namespace sight::io
