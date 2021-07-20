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

#include <data/Node.hpp>

namespace sight::io::session
{

namespace detail::Node
{

constexpr static auto s_Object {"Object"};
constexpr static auto s_Input {"Input"};
constexpr static auto s_Output {"Output"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter&,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& = ""
)
{
    const auto node = Helper::safeCast<data::Node>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Node>(tree, 1);

    children[s_Object] = node->getObject();

    size_t index = 0;
    for(const auto& input : node->getInputPorts())
    {
        children[s_Input + std::to_string(index++)] = input;
    }

    index = 0;
    for(const auto& output : node->getOutputPorts())
    {
        children[s_Output + std::to_string(index++)] = output;
    }
}

//------------------------------------------------------------------------------

inline static data::Node::sptr deserialize(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& = ""
)
{
    // Create or reuse the object
    auto node = Helper::safeCast<data::Node>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Node>(tree, 0, 1);

    node->setObject(children.at(s_Object));

    // Clearing is required in case the object is reused
    node->getInputPorts().clear();
    node->getOutputPorts().clear();

    for(size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& inputIt  = children.find(s_Input + std::to_string(index));
        const auto& outputIt = children.find(s_Output + std::to_string(index));

        // If we didn't found a matching input or output, exit the loop
        if(inputIt == children.cend() && outputIt == children.cend())
        {
            break;
        }

        if(inputIt != children.cend())
        {
            node->addInputPort(std::dynamic_pointer_cast<data::Port>(inputIt->second));
        }

        if(outputIt != children.cend())
        {
            node->addOutputPort(std::dynamic_pointer_cast<data::Port>(outputIt->second));
        }
    }

    return node;
}

} // namespace detail::Node

} // namespace sight::io
