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
#include <data/Graph.hpp>
#include <data/Node.hpp>

namespace sight::io::session
{

namespace detail::Graph
{

constexpr static auto s_Up {"Up"};
constexpr static auto s_Down {"Down"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter&,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& children,
    const core::crypto::secure_string& = ""
)
{
    const auto graph = Helper::safeCast<data::Graph>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Graph>(tree, 1);

    // Serialize node
    std::size_t index = 0;
    for(const auto& node : graph->getNodes())
    {
        children[data::Node::classname() + std::to_string(index++)] = node;
    }

    // Serialize connections
    index = 0;
    for(const auto& connection : graph->getConnections())
    {
        // Serialize edge
        const std::string edgeIndex = data::Edge::classname() + std::to_string(index++);
        children[edgeIndex] = connection.first;

        // Serialize nodes
        children[s_Up + edgeIndex]   = connection.second.first;
        children[s_Down + edgeIndex] = connection.second.second;
    }
}

//------------------------------------------------------------------------------

inline static data::Graph::sptr deserialize(
    zip::ArchiveReader&,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& children,
    data::Object::sptr object,
    const core::crypto::secure_string& = ""
)
{
    // Create or reuse the object
    auto graph = Helper::safeCast<data::Graph>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Graph>(tree, 0, 1);

    // Deserialize node
    // Clearing is required in case the object is reused
    graph->getNodes().clear();

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        const auto& nodeIt = children.find(data::Node::classname() + std::to_string(index));

        // If we didn't found a matching node, exit the loop
        if(nodeIt == children.cend())
        {
            break;
        }

        graph->addNode(std::dynamic_pointer_cast<data::Node>(nodeIt->second));
    }

    // Deserialize connections
    // Clearing is required in case the object is reused
    graph->getConnections().clear();

    for(std::size_t index = 0, end = children.size() ; index < end ; ++index)
    {
        // Deserialize edge
        const std::string edgeIndex = data::Edge::classname() + std::to_string(index);
        const auto& edgeIt          = children.find(edgeIndex);
        const auto& upIt            = children.find(s_Up + edgeIndex);
        const auto& downIt          = children.find(s_Down + edgeIndex);

        // If we didn't found a matching edge or node, exit the loop
        if(edgeIt == children.cend() && upIt == children.cend() && downIt == children.cend())
        {
            break;
        }

        const auto& edge     = std::dynamic_pointer_cast<data::Edge>(edgeIt->second);
        const auto& upNode   = std::dynamic_pointer_cast<data::Node>(upIt->second);
        const auto& downNode = std::dynamic_pointer_cast<data::Node>(downIt->second);

        graph->addEdge(edge, upNode, downNode);
    }

    return graph;
}

} // namespace detail::Graph

} // namespace sight::io
