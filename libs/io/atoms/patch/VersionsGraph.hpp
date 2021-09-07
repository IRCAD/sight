/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "io/atoms/config.hpp"
#include "io/atoms/patch/LinkDescriptor.hpp"
#include "io/atoms/patch/VersionDescriptor.hpp"

#include <core/BaseObject.hpp>
#include <core/mt/types.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

namespace sight::io::atoms::patch
{

/**
 * @brief Versions graph description.
 **/
class IO_ATOMS_CLASS_API VersionsGraph : public core::BaseObject
{
public:

    SIGHT_DECLARE_CLASS(VersionsGraph, io::atoms::patch::VersionsGraph, new VersionsGraph);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    /**
     * @name Typedefs
     * @{ */
    typedef io::atoms::patch::VersionDescriptor NodeType;
    typedef io::atoms::patch::LinkDescriptor EdgeType;
    typedef ::boost::adjacency_list< ::boost::listS, ::boost::vecS, ::boost::directedS, NodeType, EdgeType> GraphType;
    typedef ::boost::graph_traits<GraphType>::vertex_descriptor NodeIDType;
    typedef ::boost::graph_traits<GraphType>::edge_descriptor EdgeIDType;
    typedef std::map<NodeType, NodeIDType, io::atoms::patch::VersionDescriptor::Compare> ExistingNodesType;
    typedef std::map<EdgeType, EdgeIDType, io::atoms::patch::LinkDescriptor::Compare> ExistingEdgesType;
    typedef std::vector<NodeIDType> VersionSeriesType;
    typedef std::pair<LinkDescriptor::VersionIDType, bool> LinkedVersionType;
    /**  @} */

    /**
     * Default constructor
     */
    IO_ATOMS_API VersionsGraph();

    /**
     * Default destructor
     */
    IO_ATOMS_API ~VersionsGraph();

    /**
     * @brief Add a new version described by the given node.
     * @param node new version to be added to the versions graph.
     */
    IO_ATOMS_API void addNode(NodeType node);

    /**
     * @brief Add a new edge described by the given edge.
     * @param edge new edge to be added to the versions graph.
     */
    IO_ATOMS_API void addEdge(EdgeType edge);

    /**
     * @brief Compute shortest path between two data version (i.e nodes).
     * @param origin the source version name
     * @param target the target version name
     */
    IO_ATOMS_API VersionSeriesType shortestPath(const std::string& origin, const std::string& target);

    /**
     * @brief Returns the node matching given node ID.
     */
    IO_ATOMS_API NodeType getNode(const NodeIDType& nodeID);

    /**
     * @brief Returns node ID matching given version name.
     */
    IO_ATOMS_API NodeIDType getNode(const std::string& name) const;

    /**
     * @brief Returns the edge located between given node IDs.
     */
    IO_ATOMS_API EdgeType getEdge(const NodeIDType& origin, const NodeIDType& target);

    /**
     * @brief Returns the linked version of the current version and a boolean to say if a match has been found.
     * (Do not rely on the versionID if returned bool is set to false).
     */
    IO_ATOMS_API LinkedVersionType getLinkedVersion(
        const NodeIDType& originID,
        const NodeIDType& targetID,
        LinkDescriptor::VersionIDType current
    );

    /**
     * @brief Get connected versions
     */
    IO_ATOMS_API std::vector<std::string> getConnectedVersions(const std::string& currentVersion);

private:

    /**
     * @brief Create or update a node
     * @param node the created/updated node
     */
    IO_ATOMS_API NodeIDType createOrUpdateNode(const NodeType& node);

    /**
     * @brief Create an edge
     * @param edge the created edge
     */
    IO_ATOMS_API EdgeIDType createEdge(const EdgeType& edge);

    /**
     * @brief Compute shortest path between two data versions.
     * @param origin the source node
     * @param target the target node
     */
    IO_ATOMS_API VersionSeriesType shortestPath(const NodeType& origin, const NodeType& target);

    /// Mutex to protect concurrent access for m_graph
    mutable core::mt::ReadWriteMutex m_graphMutex;

    /// Mutex to protect concurrent access for m_nodes
    mutable core::mt::ReadWriteMutex m_nodesMutex;

    /// Mutex to protect concurrent access for m_edges
    mutable core::mt::ReadWriteMutex m_edgesMutex;

    /// Versions graph
    GraphType m_graph;

    /// Nodes
    ExistingNodesType m_nodes;

    /// Edges
    ExistingEdgesType m_edges;
};

} // fwAtomsPatch
