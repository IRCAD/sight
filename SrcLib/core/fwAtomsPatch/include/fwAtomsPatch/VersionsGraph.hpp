/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSPATCH_VERSIONSGRAPH_HPP__
#define __FWATOMSPATCH_VERSIONSGRAPH_HPP__

#include "fwAtomsPatch/LinkDescriptor.hpp"
#include "fwAtomsPatch/VersionDescriptor.hpp"
#include "fwAtomsPatch/config.hpp"

#include <fwCore/BaseObject.hpp>
#include <fwCore/mt/types.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

namespace fwAtomsPatch
{

/**
 * @brief Versions graph description.
 **/
class FWATOMSPATCH_CLASS_API VersionsGraph : public ::fwCore::BaseObject
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (VersionsGraph)(::fwAtomsPatch::VersionsGraph), (()), new VersionsGraph);
    fwCoreAllowSharedFromThis();

    /**
     * @name Typedefs
     * @{ */
    typedef ::fwAtomsPatch::VersionDescriptor NodeType;
    typedef ::fwAtomsPatch::LinkDescriptor EdgeType;
    typedef ::boost::adjacency_list< ::boost::listS, ::boost::vecS, ::boost::directedS, NodeType, EdgeType> GraphType;
    typedef ::boost::graph_traits<GraphType>::vertex_descriptor NodeIDType;
    typedef ::boost::graph_traits<GraphType>::edge_descriptor EdgeIDType;
    typedef std::map<NodeType, NodeIDType, ::fwAtomsPatch::VersionDescriptor::Compare> ExistingNodesType;
    typedef std::map<EdgeType, EdgeIDType, ::fwAtomsPatch::LinkDescriptor::Compare> ExistingEdgesType;
    typedef std::vector< NodeIDType > VersionSeriesType;
    typedef std::pair<LinkDescriptor::VersionIDType, bool> LinkedVersionType;
    /**  @} */

    /**
     * Default constructor
     */
    FWATOMSPATCH_API VersionsGraph();

    /**
     * Default destructor
     */
    FWATOMSPATCH_API ~VersionsGraph();

    /**
     * @brief Add a new version described by the given node.
     * @param node new version to be added to the versions graph.
     */
    FWATOMSPATCH_API void addNode(NodeType node);

    /**
     * @brief Add a new edge described by the given edge.
     * @param edge new edge to be added to the versions graph.
     */
    FWATOMSPATCH_API void addEdge(EdgeType edge);

    /**
     * @brief Compute shortest path between two data version (i.e nodes).
     * @param origin the source version name
     * @param target the target version name
     */
    FWATOMSPATCH_API VersionSeriesType shortestPath(const std::string& origin, const std::string& target);

    /**
     * @brief Returns the node matching given node ID.
     */
    FWATOMSPATCH_API NodeType getNode(const NodeIDType& nodeID);

    /**
     * @brief Returns node ID matching given version name.
     */
    FWATOMSPATCH_API NodeIDType getNode(const std::string& name) const;

    /**
     * @brief Returns the edge located between given node IDs.
     */
    FWATOMSPATCH_API EdgeType getEdge(const NodeIDType& origin, const NodeIDType& target);

    /**
     * @brief Returns the linked version of the current version and a boolean to say if a match has been found.
     * (Do not rely on the versionID if returned bool is set to false).
     */
    FWATOMSPATCH_API LinkedVersionType getLinkedVersion(
        const NodeIDType& originID, const NodeIDType& targetID, LinkDescriptor::VersionIDType current);

    /**
     * @brief Get connected versions
     */
    FWATOMSPATCH_API std::vector< std::string > getConnectedVersions(const std::string& currentVersion);

private:

    /**
     * @brief Create or update a node
     * @param node the created/updated node
     */
    FWATOMSPATCH_API NodeIDType createOrUpdateNode(const NodeType& node);

    /**
     * @brief Create an edge
     * @param edge the created edge
     */
    FWATOMSPATCH_API EdgeIDType createEdge(const EdgeType& edge);

    /**
     * @brief Compute shortest path between two data versions.
     * @param origin the source node
     * @param target the target node
     */
    FWATOMSPATCH_API VersionSeriesType shortestPath(const NodeType& origin, const NodeType& target);

    /// Mutex to protect concurrent access for m_graph
    mutable ::fwCore::mt::ReadWriteMutex m_graphMutex;

    /// Mutex to protect concurrent access for m_nodes
    mutable ::fwCore::mt::ReadWriteMutex m_nodesMutex;

    /// Mutex to protect concurrent access for m_edges
    mutable ::fwCore::mt::ReadWriteMutex m_edgesMutex;

    /// Versions graph
    GraphType m_graph;

    /// Nodes
    ExistingNodesType m_nodes;

    /// Edges
    ExistingEdgesType m_edges;
};

} // fwAtomsPatch

#endif /* __FWATOMSPATCH_VERSIONSGRAPH_HPP__ */
